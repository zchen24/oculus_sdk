
#include <iostream>
#include "ROSViewer.h"



ROSViewerApp::ROSViewerApp():
    IsHmdDebug(false)
{

}


ROSViewerApp::~ROSViewerApp()
{
    // close window
    pPlatform->DestroyWindow();

    // close oculus
    if (hmd) {
        ovrHmd_Destroy(hmd);
        hmd = 0;
    }
    ovr_Shutdown();
}


int ROSViewerApp::OnStartup(int argc, const char **argv)
{
    // *** Oculus HMD & Sensor Initialization

    // Create DeviceManager and first available HMDDevice from it.
    // Sensor object is created from the HMD, to ensure that it is on the
    // correct device.

    ovr_Initialize();
    hmd = ovrHmd_Create(0);
    if (!hmd)
    {
        std::cerr << "Unable to create HMD: "
                  << ovrHmd_GetLastError(NULL) << std::endl;
        hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
        IsHmdDebug = true;
        if (!hmd) return -1;
    }

    std::cout << "HmdCaps = " << std::hex << hmd->HmdCaps << std::endl;

    if (hmd->HmdCaps & ovrHmdCap_ExtendDesktop)
    {
        WindowSize = hmd->Resolution;
        printf("width = %d, height = %d\n", hmd->Resolution.w,
               hmd->Resolution.h);
    }
    else
    {
        // direct app-rendered mode
        WindowSize = Sizei(1100, 618);  // avoid rotated output bug
    }

    // ***** Setup System Window & rendering.
    if (!SetupWindowAndRendering(argc, argv)) {
        return -1;
    } else {
        std::cout << "System Window Setup Done" << std::endl;
    }

    // Initialize FovSideTanMax, which allows us to change all Fov sides at once - Fov
    // starts at default and is clamped to this value.
    mFovSideTanLimit = FovPort::Max(hmd->MaxEyeFov[0], hmd->MaxEyeFov[1]).GetMaxSideTan();
    mFovSideTanMax   = FovPort::Max(hmd->DefaultEyeFov[0], hmd->DefaultEyeFov[1]).GetMaxSideTan();

    mPositionTrackingEnabled = (hmd->TrackingCaps & ovrTrackingCap_Position) ? true : false;
    mPixelLuminanceOverdrive = (hmd->DistortionCaps & ovrDistortionCap_Overdrive) ? true : false;
    mHqAaDistortion = (hmd->DistortionCaps & ovrDistortionCap_HqDistortion) ? true : false;

    // Configure HMD Stereo Settings
    CalculateHmdValues();


    // Identify Scene File & Prepare for Loading
    // Populate some scene here
//    ovrHmd_StartPerfLog(hmd, "logROSViewer.txt", NULL);

    return 0;
}


void ROSViewerApp::OnIdle()
{
    std::cout << "Loop " << ovr_GetTimeInSeconds() << std::endl;

//    ovrHmd_BeginFrame(hmd, 0);

    // Fancy stuff here

//    ovrHmd_EndFrame(hmd, EyeRenderPose, EyeTexture);
}


void ROSViewerApp::OnKey(OVR::KeyCode key, int chr, bool down, int modifiers)
{
    std::cout << "Key Pressed" << std::endl;
}


void ROSViewerApp::OnMouseMove(int x, int y, int modifiers)
{
    std::cout << "Mouse Moved" << std::endl;
}


void ROSViewerApp::OnResize(int width, int height)
{
    std::cout << "Window Resized" << std::endl;
}



// ----------------------------------------
bool ROSViewerApp::SetupWindowAndRendering(int argc, const char **argv)
{
    // Create Window
    void* windowHandle = pPlatform->SetupWindow(WindowSize.w, WindowSize.h);
    if (!windowHandle) return false;

    // Report relative mouse moiton in OnMouseMove
    pPlatform->SetMouseMode(Mouse_Relative);

    // Initial Rendering
    const char* graphics = "GL";

    // Select render based on comand line args
//    for (size_t i = 0; i < argc; i++) {
//        if(!strcmp(argv[i], "-fs"))
//        {
//            RenderParams.Fullscreen = true;
//        }
//    }

    String winTitle = "Oculus ROS Viewer ";
    winTitle += graphics;
    if (hmd->ProductName[0]) {
        winTitle += " : ";
        winTitle += hmd->ProductName;
    }
    pPlatform->SetWindowTitle(winTitle);

    mRenderParams.Display = DisplayId(hmd->DisplayDeviceName, hmd->DisplayId);
    mRenderParams.SrgbBackBuffer = false;
    mRenderParams.Multisample = 0;
    mRenderParams.Resolution = hmd->Resolution;

    // Setup Graphics
    mRender = pPlatform->SetupGraphics(OVR_DEFAULT_RENDER_DEVICE_SET,
                                       graphics, mRenderParams);
    if (!mRender) return false;

    return true;
}


void ROSViewerApp::CalculateHmdValues()
{
    // Initialize eye rendering info
    // Initialize eye rendering information for ovrHmd_Configure.
    // The viewport sizes are re-computed in case RenderTargetSize changed due to HW limitations.
    ovrFovPort eyeFov[2];
    eyeFov[0] = hmd->DefaultEyeFov[0];
    eyeFov[1] = hmd->DefaultEyeFov[1];

    // Clamp Fov based on our dynamically adjustable FovSideTanMax.
    // Most apps should use the default, but reducing Fov does reduce rendering cost.
    eyeFov[0] = FovPort::Min(eyeFov[0], FovPort(mFovSideTanMax));
    eyeFov[1] = FovPort::Min(eyeFov[1], FovPort(mFovSideTanMax));

    // ------------ Stereo Rendering ------------
    Sizei recommenedTex0Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left,  eyeFov[0], 1.0);
    Sizei recommenedTex1Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right, eyeFov[1], 1.0);

    // Render is shared by Both Eyes
    Sizei  rtSize(recommenedTex0Size.w + recommenedTex1Size.w,
                  Alg::Max(recommenedTex0Size.h, recommenedTex1Size.h));


    // Don't draw more then recommended size; this also ensures that resolution reported
    // in the overlay HUD size is updated correctly for FOV/pixel density change.
    EyeRenderSize[0] = Sizei::Min(Sizei(rtSize.w/2, rtSize.h), recommenedTex0Size);
    EyeRenderSize[1] = Sizei::Min(Sizei(rtSize.w/2, rtSize.h), recommenedTex1Size);

    // Store texture pointers that will be passed for rendering.
    // Same texture is used, but with different viewports.
    EyeTexture[0]                       = RenderTargets[Rendertarget_BothEyes].OvrTex;
    EyeTexture[0].Header.TextureSize    = rtSize;
    EyeTexture[0].Header.RenderViewport = Recti(Vector2i(0), EyeRenderSize[0]);
    EyeTexture[1]                       = RenderTargets[Rendertarget_BothEyes].OvrTex;
    EyeTexture[1].Header.TextureSize    = rtSize;
    EyeTexture[1].Header.RenderViewport = Recti(Vector2i((rtSize.w+1)/2, 0), EyeRenderSize[1]);

    // Where we draw
    DrawEyeTargets = RenderTargets;    // ZC: TODO based on multisampling

    // hmd caps
    unsigned int hmdCaps = 0;

    // dynamic prediction
    hmdCaps |= ovrHmdCap_DynamicPrediction;
//    hmdCaps |= ovrHmdCap_DisplayOff;
    hmdCaps |= ovrHmdCap_NoMirrorToWindow;

    // notificaiton overlay
    ovrHmd_SetEnabledCaps(hmd, hmdCaps);

    ovrRenderAPIConfig config = mRender->Get_ovrRenderAPIConfig();

    unsigned int distortionCaps = ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette;
    distortionCaps |= ovrDistortionCap_SRGB;
    distortionCaps |= ovrDistortionCap_Overdrive;
    distortionCaps |= ovrDistortionCap_TimeWarp;
    distortionCaps |= ovrDistortionCap_ProfileNoTimewarpSpinWaits;
    distortionCaps |= ovrDistortionCap_HqDistortion;

    if (!ovrHmd_ConfigureRendering( hmd, &config, distortionCaps, eyeFov, EyeRenderDesc ))
    {
        // Fail exit? TBD
        return;
    }


    // Configure Sensors
    unsigned sensorCaps = ovrTrackingCap_Orientation|ovrTrackingCap_MagYawCorrection;
    sensorCaps |= ovrTrackingCap_Position;
    ovrHmd_ConfigureTracking(hmd, sensorCaps, 0);

    // Calculate projections
    // Calculate projections
    Projection[0] = ovrMatrix4f_Projection(EyeRenderDesc[0].Fov,  0.01f, 10000.0f, true);
    Projection[1] = ovrMatrix4f_Projection(EyeRenderDesc[1].Fov,  0.01f, 10000.0f, true);

    float    orthoDistance = 0.8f; // 2D is 0.8 meter from camera
    Vector2f orthoScale0   = Vector2f(1.0f) / Vector2f(EyeRenderDesc[0].PixelsPerTanAngleAtCenter);
    Vector2f orthoScale1   = Vector2f(1.0f) / Vector2f(EyeRenderDesc[1].PixelsPerTanAngleAtCenter);

    OrthoProjection[0] = ovrMatrix4f_OrthoSubProjection(Projection[0], orthoScale0, orthoDistance,
                                                        EyeRenderDesc[0].HmdToEyeViewOffset.x);
    OrthoProjection[1] = ovrMatrix4f_OrthoSubProjection(Projection[1], orthoScale1, orthoDistance,
                                                        EyeRenderDesc[1].HmdToEyeViewOffset.x);

//    getchar();
}


//-------------------------------------------------------------------------------------
OVR_PLATFORM_APP(ROSViewerApp);

