// oculus
#include <iostream>
#include "UtilOculusRift.h"

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
