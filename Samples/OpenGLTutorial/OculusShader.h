#ifndef MY_OCULUS_SHADER_H
#define MY_OCULUS_SHADER_H


#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include <OVR_CAPI.h>
#include <opencv2/opencv.hpp>
#include "OculusRift.h"


// FrameWork Reference:
//   https://github.com/PaulSolt/GLUT-Object-Oriented-Framework/blob/master/src/GlutFramework.h


class OculusShader
{
public:
    static OculusShader* Instance();

    // Callback Function
    static void CallbackKeyFuncWrapper(unsigned char key, int x, int y);
    static void CallbackRenderWrapper(void);
    static void CallbackOnIdleWrapper(void);

private:
    // private
    OculusShader();
    OculusShader(const OculusShader&){}
    OculusShader& operator =(const OculusShader&){}


    /*
     * Functions for creating OpenGL objects:
     */
    GLuint makeBuffer(GLenum target,
                             const void *buffer_data,
                             GLsizei buffer_size);

    GLuint makeTexture(const char *filename);
    GLuint makeShader(GLenum type, const char *filename);
    GLuint makeProgram(GLuint vertex_shader, GLuint fragment_shader);
    void* fileContents(const char *filename, GLint *length);

    // Render Function
    void CallbackRender(void);
    void CallbackOnIdle(void);
    void CallbackKeyFunc(unsigned char key, int x, int y);

    // Instance
    static OculusShader* mInstance;
    cv::VideoCapture mCap;
    MyOculusRift mOculus;


    // ------------------------




    // Shader related stuff
    GLuint mOculusVertexBuffer;
    GLuint mOculusVignetteBuffer;
    GLuint mOculusElementBuffer;
    GLsizei mOculusElementCount;

    GLuint mVertexBuffer, mElementBuffer;
    GLuint mTexture;
    GLuint mVertexShader, mFragmentShader;
    GLuint mProgram;

    // "pointer" to GPU stuff
    struct {
        GLint fadeFactorLoc;
        GLint textureLoc;
        GLint eyeToSourceUVScaleLoc;
        GLint eyeToSourceUVOffsetLoc;
    } mUniforms;

    struct {
        GLint positionLoc;
        GLint vignetteLoc;
    } mAttributes;

    GLfloat mFadeFactor;
};



#endif  // MY_OCULUS_SHADER_H
