#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include <math.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>

#include "OculusRift.h"
#include "OculusShader.h"

/*
 * Global data used by our render callback:
 */

static void show_info_log(
    GLuint object,
    PFNGLGETSHADERIVPROC glGet__iv,
    PFNGLGETSHADERINFOLOGPROC glGet__InfoLog
)
{
    GLint log_length;
    char *log;

    glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
    log = (char*)malloc(log_length);
    glGet__InfoLog(object, log_length, NULL, log);
    fprintf(stderr, "%s", log);
    free(log);
}


/*
 * Entry point
 */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(800, 450);
    glutCreateWindow("Hello World");
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;

    glewInit();
    if (!GLEW_VERSION_2_0) {
        fprintf(stderr, "OpenGL 2.0 not available\n");
        return 1;
    }

    OculusShader* shader = OculusShader::Instance();
    glutDisplayFunc(&OculusShader::CallbackRenderWrapper);
    glutIdleFunc(&OculusShader::CallbackOnIdleWrapper);
    glutKeyboardFunc(&OculusShader::CallbackKeyFuncWrapper);

    glutMainLoop();
    return 0;
}


