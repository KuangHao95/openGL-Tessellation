// FILE: main.cpp
// APPLICATION PROGRAM

#include <cstdlib>
#include <cstdio>
#include <cmath>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "helper/trackball.h"
#include "helper/glslprogram.h"
#include "helper/vboplanepatches.h"

// Shader program.
GLSLProgram shaderProg;

// The rectangular plane made of a 2D array of triangle patches.
Drawable *planePatches = NULL;

// Texture objects.
GLuint texObjID[2] = { 0, 0 };


// Light info. Must be a point light.
const glm::vec4 lightPosition = glm::vec4(5.0f, 5.0f, 5.0f, 0.0f);  // Directional light. Given in eye space.
const glm::vec3 lightAmbient  = glm::vec3(0.2f, 0.2f, 0.2f);
const glm::vec3 lightDiffuse  = glm::vec3(1.0f, 1.0f, 1.0f);
const glm::vec3 lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);


// For rendering window and viewport size.
int winWidth = 1024;    // Window width in pixels.
int winHeight = 768;    // Window height in pixels.


// Toggle between wireframe and no wireframe.
bool showWireframe = true;

// For trackball.
double prevMouseX, prevMouseY;
bool mouseLeftPressed;
bool mouseMiddlePressed;
bool mouseRightPressed;
float cam_curr_quat[4];
float cam_prev_quat[4];
float cam_eye[3], cam_lookat[3], cam_up[3];
const float initial_cam_eye[3] = { 0.0f, 0.0f, 20.0f };  // World coordinates.
const float initial_cam_lookat[3] = { 0.0f, 0.0f, 0.0f };  // World coordinates.



/////////////////////////////////////////////////////////////////////////////
// Draw the objects in the 3D scene.
/////////////////////////////////////////////////////////////////////////////
static void RenderObjects(const glm::mat4 &viewMat, const glm::mat4 &projMat)
{
    const float cubeWidth = 10.0f;
    const float cubeHalfWidth = cubeWidth / 2.0f;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texObjID[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texObjID[1]);

    //shaderProg.setUniform("MatlAmbient", glm::vec3(0.75f, 0.75f, 1.0f));
    //shaderProg.setUniform("MatlDiffuse", glm::vec3(0.75f, 0.75f, 1.0f));
    shaderProg.setUniform("MatlSpecular", glm::vec3(1.0f, 1.0f, 1.0f));
    shaderProg.setUniform("MatlShininess", 16.0f);

    {   // +y
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, cubeHalfWidth, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(cubeWidth));

        glm::mat4 modelViewMat = viewMat * modelMat;
        glm::mat4 modelViewProjMat = projMat * modelViewMat;
        glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(modelViewMat)));

        shaderProg.setUniform("ModelViewMatrix", modelViewMat);
        shaderProg.setUniform("ModelViewProjMatrix", modelViewProjMat);
        shaderProg.setUniform("NormalMatrix", normalMat);

        planePatches->render();
    }

    {   // -y
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, -cubeHalfWidth, 0.0f));
        modelMat = glm::rotate(modelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(cubeWidth));

        glm::mat4 modelViewMat = viewMat * modelMat;
        glm::mat4 modelViewProjMat = projMat * modelViewMat;
        glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(modelViewMat)));

        shaderProg.setUniform("ModelViewMatrix", modelViewMat);
        shaderProg.setUniform("ModelViewProjMatrix", modelViewProjMat);
        shaderProg.setUniform("NormalMatrix", normalMat);

        planePatches->render();
    }

    {   // +x
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(cubeHalfWidth, 0.0f, 0.0f));
        modelMat = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMat = glm::scale(modelMat, glm::vec3(cubeWidth));

        glm::mat4 modelViewMat = viewMat * modelMat;
        glm::mat4 modelViewProjMat = projMat * modelViewMat;
        glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(modelViewMat)));

        shaderProg.setUniform("ModelViewMatrix", modelViewMat);
        shaderProg.setUniform("ModelViewProjMatrix", modelViewProjMat);
        shaderProg.setUniform("NormalMatrix", normalMat);

        planePatches->render();
    }

    {   // -x
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(-cubeHalfWidth, 0.0f, 0.0f));
        modelMat = glm::rotate(modelMat, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMat = glm::scale(modelMat, glm::vec3(cubeWidth));

        glm::mat4 modelViewMat = viewMat * modelMat;
        glm::mat4 modelViewProjMat = projMat * modelViewMat;
        glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(modelViewMat)));

        shaderProg.setUniform("ModelViewMatrix", modelViewMat);
        shaderProg.setUniform("ModelViewProjMatrix", modelViewProjMat);
        shaderProg.setUniform("NormalMatrix", normalMat);

        planePatches->render();
    }

    {   // +z
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, cubeHalfWidth));
        modelMat = glm::rotate(modelMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(cubeWidth));

        glm::mat4 modelViewMat = viewMat * modelMat;
        glm::mat4 modelViewProjMat = projMat * modelViewMat;
        glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(modelViewMat)));

        shaderProg.setUniform("ModelViewMatrix", modelViewMat);
        shaderProg.setUniform("ModelViewProjMatrix", modelViewProjMat);
        shaderProg.setUniform("NormalMatrix", normalMat);

        planePatches->render();
    }

    {   // -z
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, -cubeHalfWidth));
        modelMat = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(cubeWidth));

        glm::mat4 modelViewMat = viewMat * modelMat;
        glm::mat4 modelViewProjMat = projMat * modelViewMat;
        glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(modelViewMat)));

        shaderProg.setUniform("ModelViewMatrix", modelViewMat);
        shaderProg.setUniform("ModelViewProjMatrix", modelViewProjMat);
        shaderProg.setUniform("NormalMatrix", normalMat);

        planePatches->render();
    }
}



/////////////////////////////////////////////////////////////////////////////
// The draw function.
/////////////////////////////////////////////////////////////////////////////
static void MyDrawFunc(void)
{
    glEnable(GL_DEPTH_TEST);  // Need to use depth testing.
    glViewport(0, 0, winWidth, winHeight); // Viewport for main window.

    glClearColor(0.2, 0.3, 0.6, 1.0);  // Set background color.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Perspective projection matrix.
    glm::mat4 projMat = glm::perspective(glm::radians(60.0f), (float)winWidth / winHeight, 0.5f, 100.0f);

    // View transformation matrix.
    glm::mat4 viewMat = glm::lookAt(glm::vec3(cam_eye[0], cam_eye[1], cam_eye[2]),
        glm::vec3(cam_lookat[0], cam_lookat[1], cam_lookat[2]),
        glm::vec3(cam_up[0], cam_up[1], cam_up[2]));

    // Get the additional camera rotation matrix introduced by trackball.  
    GLfloat rot[4][4];
    build_rotmatrix(rot, cam_curr_quat);
    glm::mat4 camRotMat = glm::mat4(rot[0][0], rot[0][1], rot[0][2], rot[0][3],
        rot[1][0], rot[1][1], rot[1][2], rot[1][3],
        rot[2][0], rot[2][1], rot[2][2], rot[2][3],
        rot[3][0], rot[3][1], rot[3][2], rot[3][3]);

    // The final view transformation has the additional rotation from trackball.
    viewMat = viewMat * camRotMat;

    shaderProg.setUniform("LightPosition", lightPosition);
    shaderProg.setUniform("LightAmbient", lightAmbient);
    shaderProg.setUniform("LightDiffuse", lightDiffuse);
    shaderProg.setUniform("LightSpecular", lightSpecular);

    shaderProg.setUniform("ViewMatrix", viewMat);
    shaderProg.setUniform("ViewportWidth", (float)winWidth);
    shaderProg.setUniform("ViewportHeight", (float)winHeight);

    shaderProg.setUniform("ShowWireframe", showWireframe);

    const float mirrorTileDensity = 3.0f;  // (0.0, inf)
    const float mirrorRadius = 0.4f;  // In tile space; (0.0, 0.5]
    const float mirrorRadiusObjectSpace = (1.0f / mirrorTileDensity) * mirrorRadius;

    shaderProg.setUniform("MirrorTileDensity", mirrorTileDensity);
    shaderProg.setUniform("MirrorRadius", mirrorRadius);
    shaderProg.setUniform("MirrorRadiusObjectSpace", mirrorRadiusObjectSpace);

    RenderObjects(viewMat, projMat);
}



/////////////////////////////////////////////////////////////////////////////
// Set up texture map from image file.
/////////////////////////////////////////////////////////////////////////////
static void SetUpTextureMapFromFile(const char *texMapFile, bool mipmap,
                                    GLuint *texObjID = NULL, int *numColorChannels = NULL)
{
    // Enable flipping of images vertically when read in.
    // This is to follow OpenGL's image coordinate system, i.e. bottom-leftmost is (0, 0).
    stbi_set_flip_vertically_on_load(true);

    // Read texture image from file.

    int imgWidth, imgHeight, numComponents;

    GLubyte *imgData = stbi_load(texMapFile, &imgWidth, &imgHeight, &numComponents, 0);

    if (imgData == NULL) {
        fprintf(stderr, "Error: Fail to read image file %s.\n", texMapFile);
        exit(EXIT_FAILURE);
    }
    printf("%s (%d x %d, %d components)\n", texMapFile, imgWidth, imgHeight, numComponents);

    GLuint tid;
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (mipmap)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    const GLint texFormat[4] = { GL_R8, GL_RG8, GL_RGB8, GL_RGBA8 };
    const GLenum dataFormat[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };

    if (1 <= numComponents && numComponents <= 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, texFormat[numComponents - 1], imgWidth, imgHeight, 0,
            dataFormat[numComponents - 1], GL_UNSIGNED_BYTE, imgData);
    }
    else {
        fprintf(stderr, "Error: Unexpected image format.\n");
        exit(EXIT_FAILURE);
    }

    stbi_image_free(imgData);

    if (mipmap) glGenerateMipmap(GL_TEXTURE_2D);

    // Return these values.
    if (texObjID != NULL) *texObjID = tid;
    if (numColorChannels != NULL) *numColorChannels = numComponents;
}



/////////////////////////////////////////////////////////////////////////////
// Set up the environment cubemap.
/////////////////////////////////////////////////////////////////////////////
static void SetUpCubeMapFromFiles(const char *cubeMapFile[6], bool mipmap,
                                  GLuint *texObjID = NULL, int *numColorChannels = NULL)
{
    GLuint target[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    GLuint tid;
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tid);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (mipmap)
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    else
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Enable flipping of images vertically when read in.
    // This is to follow OpenGL's image coordinate system, i.e. bottom-leftmost is (0, 0).
    stbi_set_flip_vertically_on_load(true);

    int imgWidth, imgHeight, numComponents;

    // Read texture images from files.
    for (int t = 0; t < 6; t++) {

        GLubyte *imgData = stbi_load(cubeMapFile[t], &imgWidth, &imgHeight, &numComponents, 0);

        if (imgData == NULL) {
            fprintf(stderr, "Error: Fail to read image file %s.\n", cubeMapFile[t]);
            exit(EXIT_FAILURE);
        }
        printf("%s (%d x %d, %d components)\n", cubeMapFile[t], imgWidth, imgHeight, numComponents);

        const GLint texFormat[4] = { GL_R8, GL_RG8, GL_RGB8, GL_RGBA8 };
        const GLenum dataFormat[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };

        if (1 <= numComponents && numComponents <= 4) {
            glTexImage2D(target[t], 0, texFormat[numComponents - 1], imgWidth, imgHeight, 0,
                dataFormat[numComponents - 1], GL_UNSIGNED_BYTE, imgData);
        }
        else {
            fprintf(stderr, "Error: Unexpected image format.\n");
            exit(EXIT_FAILURE);
        }

        stbi_image_free(imgData);
    }

    if (mipmap) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // Return these values.
    if (texObjID != NULL) *texObjID = tid;
    if (numColorChannels != NULL) *numColorChannels = numComponents;
}




/////////////////////////////////////////////////////////////////////////////
// The init function.
/////////////////////////////////////////////////////////////////////////////
static void MyInit()
{
    // Set up shader program.
    try {
        shaderProg.compileShader("ProcDispMap.vs.glsl", GLSLShader::VERTEX);
        shaderProg.compileShader("ProcDispMap.tcs.glsl", GLSLShader::TESS_CONTROL);
        shaderProg.compileShader("ProcDispMap.tes.glsl", GLSLShader::TESS_EVALUATION);
        shaderProg.compileShader("ProcDispMap.gs.glsl", GLSLShader::GEOMETRY);
        shaderProg.compileShader("ProcDispMap.fs.glsl", GLSLShader::FRAGMENT);
        shaderProg.link();
        shaderProg.validate();
        shaderProg.use();
    }
    catch (GLSLProgramException &e) {
        fprintf(stderr, "Error: %s.\n", e.what());
        exit(EXIT_FAILURE);
    }

    // Create geometry of rectangular plane, 
    // which is made of a 2D array of triangle patches.
    planePatches = new VBOPlanePatches(1.0, 1.0, 5, 5);

    // Cubemap images' filenames.
    const char *cubeMapFile[6] = {
        "images/cm2_right.png", "images/cm2_left.png",
        "images/cm2_top.png", "images/cm2_bottom.png",
        "images/cm2_back.png", "images/cm2_front.png"
    };

    // Set up environment cubemap.
    // To be bound to Texture Unit 0.
    SetUpCubeMapFromFiles(cubeMapFile, true, &(texObjID[0]));

    // Set up wood texture.
    // To be bound to Texture Unit 1.
    SetUpTextureMapFromFile("images/wood.png", true, &(texObjID[1]));


    // Initialization for trackball.
    trackball(cam_curr_quat, 0, 0, 0, 0);
    cam_eye[0] = initial_cam_eye[0];
    cam_eye[1] = initial_cam_eye[1];
    cam_eye[2] = initial_cam_eye[2];
    cam_lookat[0] = initial_cam_lookat[0];
    cam_lookat[1] = initial_cam_lookat[1];
    cam_lookat[2] = initial_cam_lookat[2];
    cam_up[0] = 0.0f;
    cam_up[1] = 1.0f;
    cam_up[2] = 0.0f;
}



/////////////////////////////////////////////////////////////////////////////
// The reshape callback function.
/////////////////////////////////////////////////////////////////////////////
static void MyReshapeFunc(GLFWwindow *window, int w, int h)
{
    winWidth = w;
    winHeight = h;
}



/////////////////////////////////////////////////////////////////////////////
// The keyboard callback function.
/////////////////////////////////////////////////////////////////////////////
static void MyKeyboardFunc(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {

        // Close window
        if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        else if (key == GLFW_KEY_R) {
            // Reset the trackball.
            trackball(cam_curr_quat, 0, 0, 0, 0);
            cam_eye[0] = initial_cam_eye[0];
            cam_eye[1] = initial_cam_eye[1];
            cam_eye[2] = initial_cam_eye[2];
            cam_lookat[0] = initial_cam_lookat[0];
            cam_lookat[1] = initial_cam_lookat[1];
            cam_lookat[2] = initial_cam_lookat[2];
            cam_up[0] = 0.0f;
            cam_up[1] = 1.0f;
            cam_up[2] = 0.0f;
        }
        else if (key == GLFW_KEY_W) {
            showWireframe = !showWireframe;
        }
    }
}



/////////////////////////////////////////////////////////////////////////////
// The mouse-click callback function.
/////////////////////////////////////////////////////////////////////////////
static void MyMouseClickFunc(GLFWwindow* window, int button, int action, int mods) 
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouseLeftPressed = true;
            trackball(cam_prev_quat, 0.0, 0.0, 0.0, 0.0);
        }
        else if (action == GLFW_RELEASE) {
            mouseLeftPressed = false;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            mouseRightPressed = true;
        }
        else if (action == GLFW_RELEASE) {
            mouseRightPressed = false;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
            mouseMiddlePressed = true;
        }
        else if (action == GLFW_RELEASE) {
            mouseMiddlePressed = false;
        }
    }
}



/////////////////////////////////////////////////////////////////////////////
// The mouse motion callback function.
/////////////////////////////////////////////////////////////////////////////
static void MyMouseMotionFunc(GLFWwindow* window, double mouse_x, double mouse_y) 
{
    float rotScale = 1.0f;
    float transScale = 5.0f;

    if (mouseLeftPressed) {
        trackball(cam_prev_quat, rotScale * (2.0f * prevMouseX - winWidth) / (float)winWidth,
            rotScale * (winHeight - 2.0f * prevMouseY) / (float)winHeight,
            rotScale * (2.0f * mouse_x - winWidth) / (float)winWidth,
            rotScale * (winHeight - 2.0f * mouse_y) / (float)winHeight);

        add_quats(cam_prev_quat, cam_curr_quat, cam_curr_quat);
    }
    else if (mouseMiddlePressed) {
        cam_eye[0] -= transScale * (mouse_x - prevMouseX) / (float)winWidth;
        cam_lookat[0] -= transScale * (mouse_x - prevMouseX) / (float)winWidth;
        cam_eye[1] += transScale * (mouse_y - prevMouseY) / (float)winHeight;
        cam_lookat[1] += transScale * (mouse_y - prevMouseY) / (float)winHeight;
    }
    else if (mouseRightPressed) {
        cam_eye[2] += transScale * (mouse_y - prevMouseY) / (float)winHeight;
        cam_lookat[2] += transScale * (mouse_y - prevMouseY) / (float)winHeight;
    }

    // Update mouse point
    prevMouseX = mouse_x;
    prevMouseY = mouse_y;
}



static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}


static void WaitForEnterKeyBeforeExit(void)
{
    fflush(stdin);
    getchar();
}



/////////////////////////////////////////////////////////////////////////////
// The main function.
/////////////////////////////////////////////////////////////////////////////
int main( int argc, char** argv )
{
    atexit(WaitForEnterKeyBeforeExit); // std::atexit() is declared in cstdlib

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(winWidth, winHeight, "main", NULL, NULL);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Register callback functions.
    glfwSetFramebufferSizeCallback(window, MyReshapeFunc);
    glfwSetMouseButtonCallback(window, MyMouseClickFunc);
    glfwSetCursorPosCallback(window, MyMouseMotionFunc);
    glfwSetKeyCallback(window, MyKeyboardFunc);

    // Initialize GLEW.
	//0xC0000005 error pops at glGenVertexArrays function. To circumvent this situation,
	//the glewExperimental global switch is turned on by setting to  GL_TRUE before calling glewInit(),
	//which ensures that all extensions with valid entry points will be exposed.
	//(I learned that on stackoverflow.com)
	glewExperimental = GL_TRUE;
    GLenum err = glewInit();

    if ( err != GLEW_OK ) {
        fprintf( stderr, "Error: %s.\n", glewGetErrorString( err ) );
        exit(EXIT_FAILURE);
    }

    printf( "Status: Using GLEW %s.\n", glewGetString( GLEW_VERSION ) );

    if ( !GLEW_VERSION_4_3 ) {
        fprintf( stderr, "Error: OpenGL 4.3 is not supported.\n" );
        exit(EXIT_FAILURE);
    }

    MyInit();

    while (!glfwWindowShouldClose(window))
    {
        //glfwPollEvents();  // Use this if there is continuous animation.
        glfwWaitEvents();  // Use this if there is no animation.
        MyDrawFunc();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
