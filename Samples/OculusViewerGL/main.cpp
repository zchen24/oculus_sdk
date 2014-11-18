// Zihan Chen
// 2014-11-15
// Oculus Rift Test Code

#include <iostream>
#include <signal.h>
#include <stdio.h>

#include <kdl/frames.hpp>
#include <kdl/frames_io.hpp>

// opengl
#include <GL/glew.h>
#include <GL/glut.h>


// oculus
#include "OVR.h"
#include "OVR_Profile.h"
#include "OVR_Stereo.h"
#include "OVR_CAPI_GL.h"
#include "OVR_CAPI_Keys.h"
#include "CAPI/GL/CAPI_GL_HSWDisplay.h"


//static ovrHmd hmd;

static const ovrHmdDesc* hmd;


KDL::Frame UtilOvrPoseToFrame(ovrPosef pose);
std::string UtilLoadShader(const std::string& fileName);


void InitDK2()
{
    ovr_Initialize();

    int numHmd = ovrHmd_Detect();
    std::cout << "Detected " << numHmd << " Oculus Device" << std::endl;

    hmd = ovrHmd_Create(0);
    if (hmd == NULL) {
        std::cerr << "Warning: no oculus device detected, creating virtual hmd"
                  << std::endl;
        hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
    }

    // Show info
    std::cout << hmd->ProductName << std::endl;
    std::cout << "TrackingCaps = " << hmd->TrackingCaps << std::endl;
    std::cout << "CameraFrustumNearZInMeters = " << hmd->CameraFrustumNearZInMeters << std::endl;
    std::cout << "CameraFrustumFarZInMeters = " << hmd->CameraFrustumFarZInMeters << std::endl;
}


void InitProfile()
{
    bool createDir = false;
    OVR::String path = OVR::GetBaseOVRPath(createDir);
    {
        using namespace OVR;
        ProfileManager* pm = ProfileManager::GetInstance();
        HMDInfo hmdInfoDummy;
        ProfileDeviceKey dkey(&hmdInfoDummy);
        dkey.ProductName = hmd->ProductName;
        dkey.PrintedSerial = hmd->SerialNumber;
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
    }
}

void InitRender()
{
    std::cout << "\n ----------- Init Rendering ------------" << std::endl;

    // Initialize Display
    ovrSizei recTex0Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left,
                                                    hmd->DefaultEyeFov[0], 1.0f);
    ovrSizei recTex1Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right,
                                                    hmd->DefaultEyeFov[1], 1.0f);

    ovrSizei renderTargetSize;
    renderTargetSize.w = recTex0Size.w + recTex1Size.w;
    renderTargetSize.h = std::max(recTex0Size.h, recTex1Size.h);
    const int eyeRenderMultisample = 1;


    // get eye fov
    // might want to adjust FOV based on hardware limit
    ovrFovPort eyeFov[2];
    eyeFov[0] = hmd->DefaultEyeFov[0];
    eyeFov[1] = hmd->DefaultEyeFov[1];

    ovrEyeRenderDesc eyeRenderDesc[2];

    eyeRenderDesc[0] = ovrHmd_GetRenderDesc(hmd, ovrEye_Left, eyeFov[0]);
    eyeRenderDesc[1] = ovrHmd_GetRenderDesc(hmd, ovrEye_Right, eyeFov[1]);

    // Add HSW info later here
}


void InitTracking()
{
    // Configure Tracking
    unsigned int supportedTrackingCaps, requiredTrackingCaps;
    supportedTrackingCaps = ovrTrackingCap_Orientation |
            ovrTrackingCap_MagYawCorrection;
    requiredTrackingCaps = supportedTrackingCaps;
    ovrBool ret = ovrHmd_ConfigureTracking(hmd, supportedTrackingCaps, requiredTrackingCaps);
    if (ret)
        std::cout << "Tracking Initialized" << std::endl;
    else
        std::cerr << "Can not initialize tracking" << std::endl;

    //
}

void Cleanup(int sig)
{
    if (sig == SIGINT) {
        std::cerr << "caught signal" << std::endl;
    }

    std::cout << "Shutting down DK2" << std::endl;
    ovrHmd_Destroy(hmd);
    ovr_Shutdown();
    exit(0);
}

void display(void)
{
    // clear all pixels
    glClear (GL_COLOR_BUFFER_BIT);

    // draw white polygon (rectangle)
    glColor3f (1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
        glVertex3f (0.25, 0.25, 0.0);
        glVertex3f (0.75, 0.25, 0.0);
        glVertex3f (0.75, 0.75, 0.0);
        glVertex3f (0.25, 0.75, 0.0);
    glEnd();


    std::cout << "are you calling me?" << std::endl;

    glFlush ();
}


void OnIdle(void)
{
    // Idle
    std::cout << "yes idling called like run()" << std::endl;

    // Get data & print out
    ovrTrackingState ts = ovrHmd_GetTrackingState(hmd, ovr_GetTimeInSeconds());
    if (ts.StatusFlags & ovrStatus_OrientationTracked)
    {
        ovrPoseStatef poseState = ts.HeadPose;
        KDL::Frame kdlPose = UtilOvrPoseToFrame(poseState.ThePose);
        std::cout << kdlPose << std::endl;
    }
    else {
        std::cerr << "Not tracked, StatusFlag = 0x" << std::hex << ts.StatusFlags << std::endl;
    }
}



void renderInit (void)
{
/*  select clearing (background) color       */
    glClearColor (0.0, 0.0, 0.0, 0.0);

/*  initialize viewing values  */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}


void renderInitShader(void)
{
    // id
    GLuint vsId = glCreateShader(GL_VERTEX_SHADER);
    GLuint psId = glCreateShader(GL_FRAGMENT_SHADER);

    // source    
    std::string vsSourceString = UtilLoadShader("oculus.vs");
    std::string fsSourceString = UtilLoadShader("oculus.fs");
    const char* vsSource = vsSourceString.c_str();
    const char* fsSource = fsSourceString.c_str();
    GLint vsLength, fsLength;
    vsLength = vsSourceString.length();
    fsLength = fsSourceString.length();
    glShaderSource(vsId, 1, &vsSource, &vsLength);
    glShaderSource(psId, 1, &fsSource, &fsLength);

    // compile
    glCompileShader(vsId);
    glCompileShader(psId);

    // link & use
    GLuint promId = glCreateProgram();
    glAttachShader(promId, vsId);
    glAttachShader(promId, psId);
    glLinkProgram(promId);
    glUseProgram(promId);
}



int main(int argc, char** argv)
{
    std::cout << "hello Oculus Rift" << std::endl;
    signal(SIGINT, Cleanup);

#if 1
    // OpenGL
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize (250, 250);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("hello");
    glewInit();
    renderInit ();
#endif

    InitDK2();
    InitProfile();
    InitRender();
    InitTracking();

    glutDisplayFunc(display);
    glutIdleFunc(OnIdle);

    renderInitShader();

//    getchar();
    glutMainLoop();
//    getchar();

    Cleanup(0);
    return 0;
}



// ---------------------------------------------
// Utility
// ---------------------------------------------

KDL::Frame UtilOvrPoseToFrame(ovrPosef pose)
{
    KDL::Frame frm;
    frm.p = KDL::Vector(pose.Position.x, pose.Position.y, pose.Position.z);
    frm.M = KDL::Rotation::Quaternion(pose.Orientation.x,
                                      pose.Orientation.y,
                                      pose.Orientation.z,
                                      pose.Orientation.w);
    return frm;
}


std::string UtilLoadShader(const std::string& fileName)
{
    std::ifstream file;
    file.open((fileName).c_str());

    std::string output;
    std::string line;

    if(file.is_open())
    {
        while(file.good())
        {
            getline(file, line);
            output.append(line + "\n");
        }
    }
    else
    {
        std::cerr << "Unable to load shader: " << fileName << std::endl;
    }

    return output;
}

