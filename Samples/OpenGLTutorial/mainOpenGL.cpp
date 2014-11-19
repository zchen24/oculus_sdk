
// include sequence matters!!!!
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <opencv2/opencv.hpp>

// ---------------------------------------------
// NOTES
//   * buffer & texture are handled differently
//

// - pic1.tga
// - pic2.tga


// Utility functions
std::string UtilLoadShader(const std::string& fileName);
void UtilCheckShader(GLint shaderId);
void *UtilReadTGA(const char *filename, int *width, int *height);


// some global variables here
static struct {
    // buffer & textures
    GLuint vertexBuffer;
    GLuint elementBuffer;
    GLuint textures[2];

    // uniforms
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;

    struct {
        GLuint fadeFactor;
        GLuint textures[2];
    }uniforms;

    // attributes
    struct {
        GLuint position;
    }attributes;

    // others
    GLfloat fadeFactor;

} gResources;

static const GLfloat gVertexBufferData[] =
{
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f, 1.0f,
    1.0f, 1.0f
};

static const GLushort gElementBufferData[] =
{
    0, 1, 2, 3
};



// Utility function
static GLuint makeBuffer(GLenum target,
                         const void* bufferData,
                         GLsizei bufferSize)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, bufferSize, bufferData, GL_STATIC_DRAW);
    return buffer;
}

#if 0
static GLuint makeTexture(const char* filename)
{
    // open img with opencv
    cv::Mat image = cv::imread(filename, 1);
    if (image.empty()) {
        std::cerr << "Failed to load image" << std::endl;
        return 0;
    } else {
        std::cout << "Image loaded: " << filename << std::endl;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(
                GL_TEXTURE_2D, 0,
                GL_RGB8,
                image.cols, image.rows, 0,
                GL_BGR, GL_UNSIGNED_BYTE,
                image.data
                );

    return texture;
}
#endif

static GLuint makeTexture(const char *filename)
{
    int width, height;
    void *pixels = UtilReadTGA(filename, &width, &height);
    GLuint texture;

    if (!pixels)
        return 0;

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
        pixels                      /* pixels */
    );
    free(pixels);
    return texture;
}



static GLuint makeShaders()
{
    // id
    GLuint vsId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fsId = glCreateShader(GL_FRAGMENT_SHADER);

    // source
    std::string vsSourceString = UtilLoadShader("./shaderGL/sample.vs");
    std::string fsSourceString = UtilLoadShader("./shaderGL/sample.fs");
    const char* vsSource = vsSourceString.c_str();
    const char* fsSource = fsSourceString.c_str();
    GLint vsLength, fsLength;
    vsLength = vsSourceString.length();
    fsLength = fsSourceString.length();
    glShaderSource(vsId, 1, &vsSource, &vsLength);
    glShaderSource(fsId, 1, &fsSource, &fsLength);

    // compile
    glCompileShader(vsId);
    glCompileShader(fsId);

    UtilCheckShader(vsId);
    UtilCheckShader(fsId);

    // link & use
    GLuint promId = glCreateProgram();
    glAttachShader(promId, vsId);
    glAttachShader(promId, fsId);
    glLinkProgram(promId);

    GLint statusLink;
    glGetProgramiv(promId, GL_LINK_STATUS, &statusLink);
    if (!statusLink){
        std::cerr << "Failed to link shader program" << std::endl;
        glDeleteProgram(promId);
        return 0;
    }


    // get handles
    gResources.vertexShader = vsId;
    gResources.fragmentShader = fsId;
    gResources.program = promId;

    gResources.uniforms.fadeFactor =
            glGetUniformLocation(gResources.program, "fadeFactor");
    gResources.uniforms.textures[0] =
            glGetUniformLocation(gResources.program, "textures[0]");
    gResources.uniforms.textures[1] =
            glGetUniformLocation(gResources.program, "textures[1]");

    gResources.attributes.position =
            glGetAttribLocation(gResources.program, "position");

    return 1;
}

// return false, if can not make resources
bool makeResources()
{
    // make buffers
    gResources.vertexBuffer = makeBuffer(
                GL_ARRAY_BUFFER,
                gVertexBufferData,
                sizeof(gVertexBufferData));

    gResources.elementBuffer = makeBuffer(
                GL_ELEMENT_ARRAY_BUFFER,
                gElementBufferData,
                sizeof(gElementBufferData));

    // make textures
//    gResources.textures[0] = makeTexture("./assetsGL/hello1.png");
//    gResources.textures[1] = makeTexture("./assetsGL/hello2.png");

    gResources.textures[0] = makeTexture("./assetsGL/hello1.tga");
    gResources.textures[1] = makeTexture("./assetsGL/hello2.tga");

    if (gResources.textures[0] == 0 || gResources.textures[1] == 0)
    {
        return false;
    }

    // make shaders/program
    makeShaders();

    return true;
}

void render()
{
    // clear to white
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // setup shader program
    glUseProgram(gResources.program);

    // fade factro
    glUniform1f(gResources.uniforms.fadeFactor, gResources.fadeFactor);

    // texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gResources.textures[0]);
    glUniform1i(gResources.uniforms.textures[0], 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gResources.textures[1]);
    glUniform1i(gResources.uniforms.textures[1], 1);

    glBindBuffer(GL_ARRAY_BUFFER, gResources.vertexBuffer);
    glVertexAttribPointer(
                gResources.attributes.position,
                2,
                GL_FLOAT,
                GL_FALSE,
                sizeof(GLfloat)*2,
                (void*)0);
    glEnableVertexAttribArray(gResources.attributes.position);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gResources.elementBuffer);
    glDrawElements(
                GL_TRIANGLE_STRIP,
                4,
                GL_UNSIGNED_SHORT,
                (void*)0);

    glDisableVertexAttribArray(gResources.attributes.position);
    glutSwapBuffers();
}


void updateFadeFactor()
{
    int milliseconds = glutGet(GLUT_ELAPSED_TIME);
    gResources.fadeFactor = sinf((float)milliseconds * 0.001f) * 0.5f + 0.5f;
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(400, 300);
    glutCreateWindow("Hello World");
    glutDisplayFunc(&render);
    glutIdleFunc(&updateFadeFactor);


    glewInit();
    std::cout << glGetString(GL_VERSION) << std::endl;

    // makeResource()
    if (!makeResources()) {
        std::cerr << "Failed to load resources" << std::endl;
        return 1;
    }

    glutMainLoop();
    return 0;
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

void UtilCheckShader(GLint shaderId)
{
    // can be more robust
    GLint status;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);

    std::cout << "Shader " << shaderId << ": " << status << std::endl;
}


static short le_short(unsigned char *bytes)
{
    return bytes[0] | ((char)bytes[1] << 8);
}



void *UtilReadTGA(const char *filename, int *width, int *height)
{
    struct tga_header {
       char  id_length;
       char  color_map_type;
       char  data_type_code;
       unsigned char  color_map_origin[2];
       unsigned char  color_map_length[2];
       char  color_map_depth;
       unsigned char  x_origin[2];
       unsigned char  y_origin[2];
       unsigned char  width[2];
       unsigned char  height[2];
       char  bits_per_pixel;
       char  image_descriptor;
    } header;
    int i, color_map_size, pixels_size;
    FILE *f;
    size_t read;
    void *pixels;

    f = fopen(filename, "rb");

    if (!f) {
        fprintf(stderr, "Unable to open %s for reading\n", filename);
        return NULL;
    }

    read = fread(&header, 1, sizeof(header), f);

    if (read != sizeof(header)) {
        fprintf(stderr, "%s has incomplete tga header\n", filename);
        fclose(f);
        return NULL;
    }
    if (header.data_type_code != 2) {
        fprintf(stderr, "%s is not an uncompressed RGB tga file\n", filename);
        fclose(f);
        return NULL;
    }
    if (header.bits_per_pixel != 24) {
        fprintf(stderr, "%s is not a 24-bit uncompressed RGB tga file\n", filename);
        fclose(f);
        return NULL;
    }

    for (i = 0; i < header.id_length; ++i)
        if (getc(f) == EOF) {
            fprintf(stderr, "%s has incomplete id string\n", filename);
            fclose(f);
            return NULL;
        }

    color_map_size = le_short(header.color_map_length) * (header.color_map_depth/8);
    for (i = 0; i < color_map_size; ++i)
        if (getc(f) == EOF) {
            fprintf(stderr, "%s has incomplete color map\n", filename);
            fclose(f);
            return NULL;
        }

    *width = le_short(header.width); *height = le_short(header.height);
    pixels_size = *width * *height * (header.bits_per_pixel/8);
    pixels = malloc(pixels_size);

    read = fread(pixels, 1, pixels_size, f);
    fclose(f);

    if (read != pixels_size) {
        fprintf(stderr, "%s has incomplete image\n", filename);
        free(pixels);
        return NULL;
    }

    return pixels;
}
