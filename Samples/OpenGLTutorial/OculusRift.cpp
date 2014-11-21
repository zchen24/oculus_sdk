// oculus
#include <iostream>
#include "OculusRift.h"
#include "GL/glut.h"

MyOculusRift::MyOculusRift()
{
    ovr_Initialize();

    int numHmd = ovrHmd_Detect();
    std::cout << "Detected " << numHmd << " Oculus Device" << std::endl;

    mHmd = ovrHmd_Create(0);
    if (mHmd == NULL) {
        std::cerr << "Warning: no oculus device detected, creating virtual hmd"
                  << std::endl;
        mHmd = ovrHmd_CreateDebug(ovrHmd_DK2);
    }

    // Show info
    std::cout << mHmd->ProductName << std::endl;
    std::cout << "TrackingCaps = " << mHmd->TrackingCaps << std::endl;
    std::cout << "CameraFrustumNearZInMeters = " << mHmd->CameraFrustumNearZInMeters << std::endl;
    std::cout << "CameraFrustumFarZInMeters = " << mHmd->CameraFrustumFarZInMeters << std::endl;
}


MyOculusRift::~MyOculusRift()
{
    std::cout << "Shutting down DK2" << std::endl;
    ovrHmd_Destroy(mHmd);
    ovr_Shutdown();
}


void MyOculusRift::InitProfile()
{
    bool createDir = false;
    OVR::String path = OVR::GetBaseOVRPath(createDir);
    {
        using namespace OVR;
        ProfileManager* pm = ProfileManager::GetInstance();
        HMDInfo hmdInfoDummy;
        ProfileDeviceKey dkey(&hmdInfoDummy);
        dkey.ProductName = mHmd->ProductName;
        dkey.PrintedSerial = mHmd->SerialNumber;
        dkey.HmdType = HmdType_DK2;
        Ptr<Profile> profile = pm->GetProfile(dkey, "Zihan");

        // Note:
        // Use OVR_KEY to access profile fields
        if (profile) {
            std::cout << "Profile Path = " << path.ToCStr() << std::endl;
            std::cout << "User count = " << pm->GetUserCount() << std::endl;
            std::cout << "User Zihan Exist = " << pm->HasUser("Zihan") << std::endl;
            std::cout << "User custom = " << profile->GetValue(OVR_KEY_GENDER) << std::endl;
        }
        else {
            std::cout << "Can not find User" << std::endl;
        }
    }

#if 0
    //------- Basically Update Mesh Stuff ----------
    // Initialize ovrEyeRenderDesc struct.
    mRenderSize.w = 800;
    mRenderSize.h = 450;

//    ovrSizei texLeftSize = ovrHmd_GetFovTextureSize(
//                mHmd, ovrEye_Left, mHmd->DefaultEyeFov[0], 1.0f);
//    ovrSizei texRightSize = ovrHmd_GetFovTextureSize(
//                mHmd, ovrEye_Right, mHmd->DefaultEyeFov[1], 1.0f);

    mEyeFov[0] = mHmd->DefaultEyeFov[0];
    mEyeFov[1] = mHmd->DefaultEyeFov[1];

    mEyeRenderViewport[0].Pos.x = 0;
    mEyeRenderViewport[0].Pos.y = 0;
    mEyeRenderViewport[0].Size.w = mRenderSize.w/2;
    mEyeRenderViewport[0].Size.h = mRenderSize.h;
    mEyeRenderViewport[1].Pos.x = (mRenderSize.w + 1)/2;
    mEyeRenderViewport[1].Pos.y = 0;
    mEyeRenderViewport[1].Size = mEyeRenderViewport[0].Size;

    mEyeRenderDesc[0] = ovrHmd_GetRenderDesc(mHmd, ovrEye_Left, mEyeFov[0]);
    mEyeRenderDesc[1] = ovrHmd_GetRenderDesc(mHmd, ovrEye_Right, mEyeFov[1]);

    for (size_t eyeNum = 0; eyeNum < 1; eyeNum++)
    {
        ovrDistortionMesh meshData;
        unsigned int distortionCaps =
                ovrDistortionCap_Vignette |
                ovrDistortionCap_Chromatic;

        // create mesh
        ovrHmd_CreateDistortionMesh(mHmd,
                                    mEyeRenderDesc[eyeNum].Eye,
                                    mEyeRenderDesc[eyeNum].Fov,
                                    distortionCaps,
                                    &meshData);

        // allocate & generate distortion mesh vertices
        ovrHmd_GetRenderScaleAndOffset(mEyeRenderDesc[eyeNum].Fov,
                                       mRenderSize,
                                       mEyeRenderViewport[eyeNum],
                                       mUVScaleOffset[eyeNum]);


        // save data here
        GLfloat Position[2][meshData.VertexCount];

        GLfloat VignetteFactor[meshData.VertexCount];
        ovrDistortionVertex* ov = meshData.pVertexData;
        for (size_t i = 0; i < meshData.VertexCount; i++)
        {
            Position[0][i] = ov->ScreenPosNDC.x;
            Position[1][i] = ov->ScreenPosNDC.y;
            VignetteFactor[i] = ov->VignetteFactor;
            ov++;
        }

        // CHANGE SHIT HERE

        // vertex buffer
        GLuint oculusVertexBuffer;
        glGenBuffers(1, &oculusVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, oculusVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(float) * 2 * meshData.VertexCount,
                     meshData.pVertexData,
                     GL_STATIC_DRAW);

        // vignette buffer
        GLuint oculusVignetteBuffer;
        glGenBuffers(1, &oculusVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, oculusVignetteBuffer);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(GLfloat) * meshData.VertexCount,
                     VignetteFactor,
                     GL_STATIC_DRAW);

        // index element buffer
        GLuint oculusElementBuffer;
        glGenBuffers(1, &oculusElementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oculusElementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(unsigned short) * meshData.IndexCount,
                     meshData.pIndexData,
                     GL_STATIC_DRAW);
        ovrHmd_DestroyDistortionMesh(&meshData);
    }
#endif
}


void MyOculusRift::InitTracking()
{
    // Configure Tracking
    unsigned int supportedTrackingCaps, requiredTrackingCaps;
    supportedTrackingCaps =
            ovrTrackingCap_Orientation |
            ovrTrackingCap_MagYawCorrection |
            ovrTrackingCap_Position;
//    requiredTrackingCaps = supportedTrackingCaps;
    requiredTrackingCaps = 0;
    ovrBool ret = ovrHmd_ConfigureTracking(mHmd, supportedTrackingCaps, requiredTrackingCaps);
    if (ret)
        std::cout << "Tracking Initialized" << std::endl;
    else
        std::cerr << "Can not initialize tracking" << std::endl;
}
