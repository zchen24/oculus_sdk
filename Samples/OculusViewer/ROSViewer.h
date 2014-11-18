

#ifndef INC_ROSViewer_h
#define INC_ROSViewer_h


#include "OVR_Kernel.h"

#include "../CommonSrc/Platform/Platform_Default.h"
#include "../CommonSrc/Render/Render_Device.h"
#include "../CommonSrc/Render/Render_XmlSceneLoader.h"
#include "../CommonSrc/Platform/Gamepad.h"
#include "../CommonSrc/Util/OptionMenu.h"
#include "../CommonSrc/Util/RenderProfiler.h"

#include "Util/Util_Render_Stereo.h"
using namespace OVR::Util::Render;


#include "Player.h"
#include "Sensors/OVR_DeviceConstants.h"

// Filename to be loaded by default, searching specified paths.
#define WORLDDEMO_ASSET_FILE  "Tuscany.xml"

#define WORLDDEMO_ASSET_PATH1 "Assets/Tuscany/"
#define WORLDDEMO_ASSET_PATH2 "../../../Assets/Tuscany/"
// This path allows the shortcut to work.
#define WORLDDEMO_ASSET_PATH3 "Samples/OculusWorldDemo/Assets/Tuscany/"
#define WORLDDEMO_ASSET_PATH4 "../Assets/Tuscany/"

using namespace OVR;
using namespace OVR::OvrPlatform;
using namespace OVR::Render;


class ROSViewerApp : public Application
{
    //
public:
    ROSViewerApp();
    ~ROSViewerApp();

    virtual int OnStartup(int argc, const char **argv);
    virtual void OnIdle();

    virtual void OnKey(OVR::KeyCode key, int chr, bool down, int modifiers);
    virtual void OnMouseMove(int x, int y, int modifiers);
    virtual void OnResize(int width, int height);

    // Window & Rendering
    bool SetupWindowAndRendering(int argc, const char** argv);
    void CalculateHmdValues();

    void InitMainFilePath();
    void PopulatePreloadScene();

protected:

    // Render
    RenderDevice*  mRender;
    RendererParams mRenderParams;

    struct RenderTarget
    {
        Ptr<Texture>     pTex;
        ovrTexture       OvrTex;
    };
    enum RendertargetsEnum
    {
        Rendertarget_Left,
        Rendertarget_Right,
        Rendertarget_BothEyes,    // Used when both eyes are rendered to the same target.
        Rendertarget_LAST
    };
    RenderTarget        RenderTargets[Rendertarget_LAST];
    RenderTarget*       DrawEyeTargets; // the buffers we'll actually render to (could be MSAA)


    // Oculus
    const ovrHmdDesc* hmd;
    ovrEyeRenderDesc    EyeRenderDesc[2];
    Matrix4f            Projection[2];          // Projection matrix for eye.
    Matrix4f            OrthoProjection[2];     // Projection for 2D.
    ovrPosef            EyeRenderPose[2];       // Poses we used for rendering.
    ovrTexture          EyeTexture[2];
    Sizei               EyeRenderSize[2];       // Saved render eye sizes; base for dynamic sizing.

    // Window
    Sizei WindowSize;

    float mFovSideTanMax;
    float mFovSideTanLimit; // Limit value for Fov.

    // state
    bool IsHmdDebug;

    bool mPositionTrackingEnabled;
    bool mPixelLuminanceOverdrive;
    bool mHqAaDistortion;



    // Scene Player
    Scene LoadingScene;

};

#endif














