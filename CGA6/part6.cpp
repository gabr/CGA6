// Arkadiusz Gabrys qe83mepi
// Agnieszka Zacher by57zeja

#include "helper.h"
#include "camera.h"

#include <glm/gtc/type_ptr.hpp>

/// The qt-headers required for the images:
#include <QtOpenGL/QGLWidget>
#include <QtGui/qimage.h>

using namespace std;

// global consts
const double PI = 3.14159265359;

// some global variables:
bool wireframe_mode = false;

/**
* @brief cameraSystem:
*  global camera variable
* use and edit
*/
cameraSystem cam;
const float forwardDelta = 2;
const float angleDelta = 2.0f;
glm::vec2 mouseStartPosition;

float t = 0;  // the time parameter (incremented in the idle-function)
float speed = 0.1;  // rotation speed of the light source in degree/frame
GLuint earthTex, moonTex, saturnTex, backgroundTex; // use for the according textures

const double planetSlices = 64;
const double planetStacks = 64;

// sun
const double sunRadius = 25;
const double sunSlices = 100;
const double sunStacks = 100;
glm::vec4 sunColor(1.0, 1.0, 0.0, 0.0);
glm::vec4 lightSource(0.0, 0.0, 0.0, 1.0);

// earth
const double earthRadius = 12;
const float earthDegree = 2.0;
glm::vec4 earthColor(0.0, 0.0, 1.0, 0.0);
// moon
const double moonRadius = 6;
const double moonSlices = 15;
const double moonStacks = 15;
const float moonDegree = 4.0;
glm::vec4 moonColor(0.5, 0.5, 0.5, 0.0);

// saturn
const double saturnRadius = 16;
const float saturnDegree = 1.0;
glm::vec4 saturnColor(1.0, 0.85, 0.6, 0.0);
// rings
const double distanceFromSaturn = 7.0;
const double distanceBetweenRings = 0.4;
const double numberOfRings = 10;
const double numberOfCircleSegments = 60;
glm::vec4 ringsColor(0.8, 0.6, 0.5, 0.0);


//We need to keep track of matrices ourselves

/**
* @brief P,V,M:
* your matrices for setting the scene, no matrix stack anymore
*/
glm::mat4 P, V, M;

/**
* @brief The ShaderUniforms struct:
* every shader has its own uniforms,
* binding of uniforms is done in bindUniforms()
* NEW: location_Texture
*
*/
struct ShaderUniforms
{
    GLuint Shader;
    GLint location_MVP;
    GLint location_MV;
    GLint location_NormalMatrix;
    GLint location_Time;
    GLint location_LightSourceViewSpace;
    GLint location_Color;
    GLint location_Texture;

    void bindUniforms(glm::mat4& M, glm::mat4& V, glm::mat4& P, glm::vec4& LightSource, glm::vec4& Color, GLuint TexdID, float  t)
    {
        location_Time = glGetUniformLocation(Shader, "Time");
        location_MVP = glGetUniformLocation(Shader, "MVP");
        location_MV = glGetUniformLocation(Shader, "MV");
        location_NormalMatrix = glGetUniformLocation(Shader, "NormalMatrix");
        location_LightSourceViewSpace = glGetUniformLocation(Shader, "LightSource");
        location_Color = glGetUniformLocation(Shader, "Color");

        location_Texture = glGetUniformLocation(Shader, "DiffuseTexture");

        // TODO create the matrices MV,MVP and NormalMatrix
        glm::mat4 MV = V * M;
        glm::mat4 MVP = P * V * M;
        glm::mat3 NormalMatrix = glm::mat3(glm::transpose(glm::inverse(MV)));

        glUniformMatrix4fv(location_MVP, 1, false, glm::value_ptr(MVP));
        glUniformMatrix4fv(location_MV, 1, false, glm::value_ptr(MV));
        glUniformMatrix3fv(location_NormalMatrix, 1, false, glm::value_ptr(NormalMatrix));
        glUniform4fv(location_LightSourceViewSpace, 1, glm::value_ptr(LightSource));
        glUniform4fv(location_Color, 1, glm::value_ptr(Color));
        glUniform1f(location_Time, 10 * t);
        //TODO: Bind texture TexID

    }

};
/**
* @brief SunShader, PhongShader:
*  encapsulation of shaders
*/
ShaderUniforms SunShader, TexturePhongShader;


void initGL()
{

    glClearColor(0, 0, 0, 0);   // set the clear color to black
    glEnable(GL_DEPTH_TEST); // turn on the depth test
    glEnable(GL_CULL_FACE);  // turn on backface culling


    // set the camera:
    V = cam.getView();
    lightSource = V * lightSource;

    // enable line smoothing:
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    createProgram_VF("sun_VS.glsl", "sun_FS.glsl", &SunShader.Shader);
    createProgram_VF("Light_and_Tex_VS.glsl", "Light_and_Tex_FS.glsl", &TexturePhongShader.Shader);

    /// TODO Texture generation: &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



    /// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
}

/// TODO: augment the following two geometry-generating functions so that the vertices have 
/// appropriate texture coordinates 

void drawSphere(float r, int slices, int stacks)
{

    //TODO: Provide texture coordinates per vertex
    //glTexCoord2f(..,..); 

    float dTheta = 2.0*PI / (float)stacks;
    float dPhi = PI / (float)slices;

    //The Northpole:
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glVertex3f(0, r, 0);
    for (int i = stacks; i >= 0; i--)
    {
        glNormal3f(sin(dPhi)*cos(i*dTheta), cos(dPhi), sin(dPhi)*sin(i*dTheta));
        glVertex3f(r*sin(dPhi)*cos(i*dTheta), r*cos(dPhi), r*sin(dPhi)*sin(i*dTheta));
    }
    glEnd();

    // Middle Part

    for (int j = 1; j<slices - 1; j++)
    {
        glBegin(GL_QUAD_STRIP);
        for (int i = stacks; i >= 0; i--)
        {

            glNormal3f(sin(j*dPhi)*cos(i*dTheta), cos(j*dPhi), sin(j*dPhi)*sin(i*dTheta));
            glVertex3f(r*sin(j*dPhi)*cos(i*dTheta), r*cos(j*dPhi), r*sin(j*dPhi)*sin(i*dTheta));

            glNormal3f(sin((j + 1)*dPhi)*cos(i*dTheta), cos((j + 1)*dPhi), sin((j + 1)*dPhi)*sin(i*dTheta));
            glVertex3f(r*sin((j + 1)*dPhi)*cos(i*dTheta), r*cos((j + 1)*dPhi), r*sin((j + 1)*dPhi)*sin(i*dTheta));
        }
        glEnd();
    }
    // South Pole:
    glBegin(GL_TRIANGLE_FAN);

    //glTexCoord2f(..,..); 

    glNormal3f(0, -1, 0);
    glVertex3f(0, -r, 0);
    for (int i = 0; i <= stacks; i++)
    {
        //glTexCoord2f(..,..); 
        glNormal3f(sin((slices - 1)*dPhi)*cos(i*dTheta), cos((slices - 1)*dPhi), sin((slices - 1)*dPhi)*sin(i*dTheta));
        glVertex3f(r*sin((slices - 1)*dPhi)*cos(i*dTheta), r*cos((slices - 1)*dPhi), r*sin((slices - 1)*dPhi)*sin(i*dTheta));
    }
    glEnd();

}

// This function draws a textured quad arround the scene.
// TODO

void DrawQuad(float a)
{
    //TODO: Provide Texture Coordinates

    // -Z
    glBegin(GL_QUADS);
    glVertex3f(a, a, -a);
    glVertex3f(-a, a, -a);
    glVertex3f(-a, -a, -a);
    glVertex3f(a, -a, -a);

    // +Z
    glVertex3f(a, a, a);
    glVertex3f(a, -a, a);
    glVertex3f(-a, -a, a);
    glVertex3f(-a, a, a);

    // +X
    glVertex3f(a, a, -a);
    glVertex3f(a, -a, -a);
    glVertex3f(a, -a, a);
    glVertex3f(a, a, a);

    // -X
    glVertex3f(-a, a, -a);
    glVertex3f(-a, a, a);
    glVertex3f(-a, -a, a);
    glVertex3f(-a, -a, -a);

    // +Y
    glVertex3f(-a, a, -a);
    glVertex3f(a, a, -a);
    glVertex3f(a, a, a);
    glVertex3f(-a, a, a);

    // +Y
    glVertex3f(-a, -a, -a);
    glVertex3f(-a, -a, a);
    glVertex3f(a, -a, a);
    glVertex3f(a, -a, -a);
    glEnd();
}

void drawCircle(float r, int num_segments)
{
    glBegin(GL_LINE_LOOP);
    for (int ii = 0; ii < num_segments; ii++)
    {
        float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle

        float x = cosf(theta);//calculate the x component
        float y = sinf(theta);//calculate the y component

        glNormal3f(x, y, 1);
        glVertex2f(r * x, r * y);//output vertex
    }
    glEnd();
}

/// Generating and initializing an openGL texture:

void initTexture(GLint texture_name, GLint w, GLint h, GLubyte *data)
{

    glBindTexture(GL_TEXTURE_2D, texture_name);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}


void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    P = glm::perspective(70.0f, (GLfloat)w / (GLfloat)h, 10.0f, 400.0f);
}

void onIdle()
{
    // set the camera:
    V = cam.getView();
    lightSource = V * glm::vec4(0, 0, 0, 1);

    t += speed;  // increase the time parameter

    glutPostRedisplay();
}


/// TODO: Bind the approproate texture prior to each draw call
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //draw a yellow sun
    glUseProgram(SunShader.Shader);
    M = glm::mat4(1.0f) * glm::rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    SunShader.bindUniforms(M, V, P, lightSource, sunColor, 0, t);
    glutSolidSphere(sunRadius, sunSlices, sunStacks);

    //Texturing from now on
    glUseProgram(TexturePhongShader.Shader);

    //TODO: Set Color for each body 

    //TODO: Set Texture for each body

    //TODO: Draw geometry for background

    //draw a blue earth
    M = glm::rotate(earthDegree * t, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::vec3(50.0f, 0.0f, 0.0f));
    TexturePhongShader.bindUniforms(M, V, P, lightSource, earthColor, 0, t);
    drawSphere(earthRadius, planetSlices, planetStacks);

    // draw the grey earth's moon
    // remember that the transformation of the earth also affects the moon
    M = M * glm::rotate(moonDegree * t, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::vec3(20.0f, 0.0f, 0.0f));
    TexturePhongShader.bindUniforms(M, V, P, lightSource, moonColor, 0, t);
    drawSphere(moonRadius, moonSlices, moonStacks);

    //draw saturn with its rings
    M = glm::rotate(saturnDegree * t, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::vec3(100.0f, 0.0f, 0.0f));
    TexturePhongShader.bindUniforms(M, V, P, lightSource, saturnColor, 0, t);
    drawSphere(saturnRadius, planetSlices, planetStacks);
    // rings
    M = M * glm::rotate(120.0f, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    TexturePhongShader.bindUniforms(M, V, P, lightSource, ringsColor, 0, t);
    for (double i = saturnRadius + distanceFromSaturn;
         i < (saturnRadius + distanceFromSaturn) + (numberOfRings * distanceBetweenRings);
         i += distanceBetweenRings)
         drawCircle(i, numberOfCircleSegments);

    glUseProgram(0); // <-- CO TO JEST
    glutSwapBuffers();
}

void cleanUp()
{

    glDeleteTextures(1, &earthTex);
    glDeleteTextures(1, &moonTex);
    glDeleteTextures(1, &saturnTex);
    glDeleteTextures(1, &backgroundTex);


    glDeleteProgram(SunShader.Shader);
    glDeleteProgram(TexturePhongShader.Shader);

}

/// the mouse handling is done in the next two functions (you additionally need some global variables):
void onMouseDown(int button, int state, int x, int y)
{
    mouseStartPosition = glm::vec2(x, y);
}

void onMouseMove(int x, int y)
{
    int deltaX = x - mouseStartPosition.x;
    int deltaY = y - mouseStartPosition.y;

    cam.yaw(deltaX * angleDelta);
    cam.pitch(deltaY * angleDelta);

    mouseStartPosition = glm::vec2(x, y);
}

// the keyboard handler:
void keyboard(unsigned char key, int x, int y)
{

    switch (key)
    {

    case 27:
        cleanUp();
        exit(1);
        break;

    case 'p': // toggle polygon mode:	
        wireframe_mode = !wireframe_mode;
        if (wireframe_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;

        // increase / decrease the speed of the point light P:     
    case '+':
        speed += 0.01;
        break;
    case '-':
        speed -= 0.01;
        break;

    case 'w':
        cam.moveForward(forwardDelta);
        break;
        
    case 's':
        cam.moveBackward(forwardDelta);
        break;

    case 'a':
        cam.roll(angleDelta);
        break;

    case 'd':
        cam.roll(-angleDelta);
        break;
    }


    glutPostRedisplay();
}


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(1042, 1024);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutCreateWindow("Planet System");

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    glutDisplayFunc(display);

    glutKeyboardFunc(keyboard);
    glutMotionFunc(onMouseMove);
    glutMouseFunc(onMouseDown);

    glutReshapeFunc(reshape);
    glutIdleFunc(onIdle);

    //TODO: you may need these 
    // glutMotionFunc(onMouseMove);
    // glutMouseFunc(onMouseDown);

    initGL();

    glutMainLoop();
}
