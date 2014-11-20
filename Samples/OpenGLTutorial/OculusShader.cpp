
#include <opencv2/opencv.hpp>
//#include <iostream>
#include <stdio.h>

#include "OculusShader.h"

/*
 * Data used to seed our vertex array and element array buffers:
 */
static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f
};
static const GLushort g_element_buffer_data[] = { 0, 1, 2, 3 };


//static OculusShader::instance = NULL;

OculusShader* OculusShader::mInstance = NULL;


OculusShader* OculusShader::Instance()
{
    if (!mInstance) {
        mInstance = new OculusShader();
    }
    return mInstance;
}


OculusShader::OculusShader()
{
    mCap.open(0);
    if (!mCap.isOpened()) {
        std::cerr << "Failed to open webcam" << std::endl;
    }

    // make shader
    mVertexShader = makeShader(GL_VERTEX_SHADER,
                               "./shaderGL/oculus.v.glsl");
    mFragmentShader = makeShader(GL_FRAGMENT_SHADER,
                                 "./shaderGL/oculus.f.glsl");
    if (mVertexShader == 0 | mFragmentShader == 0)
        std::cerr << "shaders failed to load" << std::endl;

    mProgram = makeProgram(mVertexShader, mFragmentShader);
    if (mProgram == 0)
        std::cerr << "Failed to create program" << std::endl;


    // initialize stuff here
    mVertexBuffer = makeBuffer(GL_ARRAY_BUFFER,
                               g_vertex_buffer_data,
                               sizeof(g_vertex_buffer_data));
    mElementBuffer = makeBuffer(GL_ELEMENT_ARRAY_BUFFER,
                                g_element_buffer_data,
                                sizeof(g_element_buffer_data));
    mTexture = makeTexture("./assetsGL/hello.png");
    if (mTexture == 0) {
        std::cerr << "failed to make texture" << std::endl;
    }

    // get params loc
    mUniforms.eyeToSourceUVScaleLoc =
            glGetUniformLocation(mProgram, "EyeToSourceUVScale");
    mUniforms.eyeToSourceUVOffsetLoc =
            glGetUniformLocation(mProgram, "EyeToSourceUVOffset");

    mUniforms.fadeFactorLoc =
            glGetUniformLocation(mProgram, "fade_factor");
    mUniforms.textureLoc =
            glGetUniformLocation(mProgram, "texture");
    mAttributes.positionLoc =
            glGetAttribLocation(mProgram, "position");
}


void OculusShader::CallbackRenderWrapper(void)
{
    mInstance->CallbackRender();
}

void OculusShader::CallbackOnIdleWrapper()
{
    mInstance->CallbackOnIdle();
}

void OculusShader::CallbackKeyFuncWrapper(unsigned char key, int x, int y)
{
    mInstance->CallbackKeyFunc(key, x, y);
}





// -----------------------------------------------------------------

void OculusShader::CallbackRender()
{
    glUseProgram(mProgram);


//    GLfloat eyeToSourceUVScale[2];
//    eyeToSourceUVScale[0] = 1.0; eyeToSourceUVScale[1] = 1.0;
//    glUniform2fv(g_resources.uniforms.EyeToSourceUVScaleLoc,
//                 2, eyeToSourceUVScale);


    // Update uniforms values
    GLfloat eyeToSourceUVOffset[2];
    eyeToSourceUVOffset[0] = 1.0; eyeToSourceUVOffset[1] = 1.0;
    glUniform2fv(mUniforms.eyeToSourceUVOffsetLoc,
                 2, eyeToSourceUVOffset);
    glUniform1f(mUniforms.fadeFactorLoc, mFadeFactor);


    // for textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glUniform1i(mUniforms.textureLoc, 0);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glVertexAttribPointer(
        mAttributes.positionLoc,  /* attribute */
        2,                                /* size */
        GL_FLOAT,                         /* type */
        GL_FALSE,                         /* normalized? */
        sizeof(GLfloat)*2,                /* stride */
        (void*)0                          /* array buffer offset */
    );
    glEnableVertexAttribArray(mAttributes.positionLoc);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementBuffer);
    glDrawElements(
        GL_TRIANGLE_STRIP,  /* mode */
        4,                  /* count */
        GL_UNSIGNED_SHORT,  /* type */
        (void*)0            /* element array buffer offset */
    );

    glDisableVertexAttribArray(mAttributes.positionLoc);
    glutSwapBuffers();
}

void OculusShader::CallbackOnIdle()
{
    int milliseconds = glutGet(GLUT_ELAPSED_TIME);
    mFadeFactor = sinf((float)milliseconds * 0.001f) * 0.5f + 0.5f;
    glutPostRedisplay();

    // grep image and update texture from webcam;
    cv::Mat frame;
    mCap >> frame;

    // opencv version
    int width, height;
    cv::flip(frame, frame, 0);  // flip around x
    width = frame.cols;
    height = frame.rows;
    glTexImage2D(
        GL_TEXTURE_2D, 0,           /* target, level */
        GL_RGB8,                    /* internal format */
        width, height, 0,           /* width, height, border */
        GL_BGR, GL_UNSIGNED_BYTE,   /* external format, type */
        frame.data                  /* pixels */
    );
}


void OculusShader::CallbackKeyFunc(unsigned char key, int x, int y)
{
    std::cout << "key = " << key << std::endl;
}


/*
 * Functions for creating OpenGL objects:
 */
GLuint OculusShader::makeBuffer(GLenum target,
                                const void *buffer_data,
                                GLsizei buffer_size)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, buffer_size, buffer_data, GL_STATIC_DRAW);
    return buffer;
}


GLuint OculusShader::makeTexture(const char *filename)
{
    // opencv version
    int width, height;
    GLuint texture;
    cv::Mat image = cv::imread(filename);
    cv::flip(image, image, 0);  // flip around x
    width = image.cols;
    height = image.rows;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    glTexImage2D(
        GL_TEXTURE_2D, 0,           /* target, level */
        GL_RGB8,                    /* internal format */
        width, height, 0,           /* width, height, border */
        GL_BGR, GL_UNSIGNED_BYTE,   /* external format, type */
        image.data                  /* pixels */
    );

    return texture;
}


GLuint OculusShader::makeShader(GLenum type, const char *filename)
{
    GLint length;
    GLchar *source = (GLchar*)fileContents(filename, &length);
    GLuint shader;
    GLint shader_ok;

    if (!source)
        return 0;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar**)&source, &length);
    free(source);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
    if (!shader_ok) {
        fprintf(stderr, "Failed to compile %s:\n", filename);
//        show_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint OculusShader::makeProgram(GLuint vertex_shader, GLuint fragment_shader)
{
    GLint program_ok;

    GLuint program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
    if (!program_ok) {
        fprintf(stderr, "Failed to link shader program:\n");
//        show_info_log(program, glGetProgramiv, glGetProgramInfoLog);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}


/*
 * Boring, non-OpenGL-related utility functions
 */

void* OculusShader::fileContents(const char *filename, GLint *length)
{
    FILE *f = fopen(filename, "r");
    void *buffer;

    if (!f) {
        fprintf(stderr, "Unable to open %s for reading\n", filename);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    *length = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = malloc(*length+1);
    *length = fread(buffer, 1, *length, f);
    fclose(f);
    ((char*)buffer)[*length] = '\0';

    return buffer;
}
