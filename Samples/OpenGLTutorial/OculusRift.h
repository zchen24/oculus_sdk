
#ifndef MY_OCULUS_RIFT_H
#define MY_OCULUS_RIFT_H

#include "OVR.h"
#include "OVR_Profile.h"
#include "OVR_Stereo.h"
#include "OVR_CAPI_Keys.h"
#include "CAPI/GL/CAPI_GL_HSWDisplay.h"

class MyOculusRift
{
public:
    MyOculusRift();
    ~MyOculusRift();

    void InitProfile();
    void InitRender();
    void InitTracking();

    const ovrHmdDesc* GetHmd(){
        return mHmd;
    }

protected:
    const ovrHmdDesc* mHmd;
    ovrEyeRenderDesc mEyeRenderDesc[2];
    ovrFovPort mEyeFov[2];
    ovrRecti mEyeRenderViewport[2];
    ovrSizei mRenderSize;
};

#endif  // MY_OCULUS_RIFT_H
