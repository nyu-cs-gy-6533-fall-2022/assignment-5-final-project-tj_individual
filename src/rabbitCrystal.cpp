#include "Helpers.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#else
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#endif

// OpenGL Mathematics Library
#include <glm/glm.hpp> // glm::vec3
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

std::vector<glm::vec3> positions;
std::vector<glm::vec3> indices;

float minX = 5.0f;
float maxX = -5.0f;
float minY = 5.0f;
float maxY = -5.0f;
float maxZ = -5.0f;

float scale, midX, midY;
struct triVer {
    int a;
    int b;
};
triVer loadOffFile(std::string path){ 
    float x, y, z;
    int a, b, c;

    
    float longerSide;

    int numVertices;
    int numTriangles;

    std::ifstream inputFile;
    inputFile.open(path, std::ifstream::binary);
    if(inputFile.is_open()){
        std::string s;
        getline(inputFile,s);
        inputFile >> numVertices;
        positions.resize(numVertices);

        inputFile >> numTriangles;
        indices.resize(numTriangles);
        inputFile.ignore();
        inputFile.ignore();

        for(int i = 0; i < numVertices; i++){
            inputFile >> x;
            if (x < minX){
                minX = x;
            }
            if (x > maxX){
                maxX = x;
            }
            inputFile >> y;
            if (y < minY){
                minY = y;
            }
            if (y > maxY){
                maxY = y;
            }
            
            inputFile >> z;
            if (z > maxZ) {
                maxZ = z;
            }
            positions[i] = glm::vec3(x, y, z);
        }
        if (maxX - minX > maxY - minY){
            longerSide = maxX- minX;
        }
        else {
            longerSide = maxY - minY;
        }
        scale = 2.0f/longerSide;
        midX = scale * (minX + maxX) / 2.0f;
        midY = scale * (minY + maxY) / 2.0f;
        maxZ = scale * maxZ;

        for (int i = 0; i < numTriangles; i++){
            inputFile >> a;
            inputFile >> a;
            inputFile >> b;
            inputFile >> c;
            indices[i] = glm::vec3(a, b, c);            
        }
    } 
    std::cout << "Num vertices: " << numVertices << " Num Triangles: " << numTriangles << std::endl;

    return {numVertices, numTriangles};
}
void loadVertexNormal(triVer s, float* vertices){
    int numVertices = s.a;
    int numTriangles = s.b;
    std::vector<glm::vec3> vertexNormals(numVertices, glm::vec3(0));

    for (int i = 0; i < numTriangles; i++){
        int v1 = int(indices[i].x);
        int v2 = int(indices[i].y);
        int v3 = int(indices[i].z);
    }   


    int counter = 0;
    for (int i = 0; i < numTriangles; i++){
        int v1 = int(indices[i].x);
        int v2 = int(indices[i].y);
        int v3 = int(indices[i].z);

        vertices[counter] = positions[v1].x*scale-midX;
        vertices[counter+1] = positions[v1].y*scale-midY;
        vertices[counter+2] = positions[v1].z*scale-maxZ-2.0f;

        vertices[counter+3] = positions[v2].x*scale-midX;
        vertices[counter+4] = positions[v2].y*scale-midY;
        vertices[counter+5] = positions[v2].z*scale-maxZ-2.0f;

        vertices[counter+6] = positions[v3].x*scale-midX;
        vertices[counter+7] = positions[v3].y*scale-midY;
        vertices[counter+8] = positions[v3].z*scale-maxZ-2.0f;
        counter += 9;
    }
}

GLFWwindow* window;
int windowWidth = 800;
int windowHeight = 600;
int setup() {
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    window = glfwCreateWindow(windowWidth, windowHeight, "Hello OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    #ifndef __APPLE__
      glewExperimental = true;
      GLenum err = glewInit();
      if(GLEW_OK != err)
      {
        /* Problem: glewInit failed, something is seriously wrong. */
       std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
      }
      glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
      std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    #endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    std::cout << "OpenGL version recieved: " << major << "." << minor << "." << rev << std::endl;
    std::cout << "Supported OpenGL is " << (const char*)glGetString(GL_VERSION) << std::endl;
    std::cout << "Supported GLSL is " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    return 0;
}


BufferObject QuadBuffer;
BufferObject SphereObj;
BufferObject TriangleObj;

std::vector<glm::vec3> quadcoords(2);
std::vector<float> spherecoords(3);
std::vector<float> trianglecoords(3);

void setUpLights(const Program &quad_program_){
 // light uniforms
    float light1[3] = { 2.9f, 2.9f, 0.0f };
    float light2[3] = { -2.9f, 2.9f, 0.0f };
    float light3[3] = { 0.0f, -0.9f, 0.0f };

    glUniform3fv(quad_program_.uniform("light1"), 1, &light1[0]);
    glUniform3fv(quad_program_.uniform("light2"), 1, &light2[0]);
    glUniform3fv(quad_program_.uniform("light3"), 1, &light3[0]);
 
}

void setUpPlanes(const Program &quad_program_){
       //plane uniforms
    // 1: ground; 2: back panel; 3: Ceiling
    // 4: left wall; 5: right wall
    float planePoint1[3] = {0.0f, -1.0f, 0.0f};
    float planeNormal1[3] = {0.0f, 1.0f, 0.0f};

    float planePoint2[3] = {0.0f, 0.0f, -5.0f};
    float planeNormal2[3] = {0.0f, 0.0f, 1.0f};

    float planePoint3[3] = {0.0f, 3.0f, 0.0f};
    float planeNormal3[3] = {0.0f, -1.0f, 0.0f};

    float planePoint4[3] = {-3.0f, 0.0f, 0.0f};
    float planeNormal4[3] = {1.0f, 0.0f, 0.0f};

    float planePoint5[3] = {3.0f, 0.0f, 0.0f};
    float planeNormal5[3] = {-1.0f, 0.0f, 0.0f};



    glUniform3fv(quad_program_.uniform("planePoint1"), 1, &planePoint1[0]);
    glUniform3fv(quad_program_.uniform("planeNormal1"), 1, &planeNormal1[0]);

    glUniform3fv(quad_program_.uniform("planePoint2"), 1, &planePoint2[0]);
    glUniform3fv(quad_program_.uniform("planeNormal2"), 1, &planeNormal2[0]);

    glUniform3fv(quad_program_.uniform("planePoint3"), 1, &planePoint3[0]);
    glUniform3fv(quad_program_.uniform("planeNormal3"), 1, &planeNormal3[0]);

    glUniform3fv(quad_program_.uniform("planePoint4"), 1, &planePoint4[0]);
    glUniform3fv(quad_program_.uniform("planeNormal4"), 1, &planeNormal4[0]);

    glUniform3fv(quad_program_.uniform("planePoint5"), 1, &planePoint5[0]);
    glUniform3fv(quad_program_.uniform("planeNormal5"), 1, &planeNormal5[0]);
}

int main(void)
{
    setup();

    VertexArrayObject VAO;
    VAO.init();
    VAO.bind();

    QuadBuffer.init();

    quadcoords.resize(6);
    quadcoords[0] = glm::vec3(-1.0f, -1.0f, 0.0f);
    quadcoords[1] = glm::vec3(1.0f, -1.0f, 0.0f);
    quadcoords[2] = glm::vec3(-1.0f, 1.0f, 0.0f);
    quadcoords[3] = glm::vec3(-1.0f, 1.0f, 0.0f);
    quadcoords[4] = glm::vec3(1.0f, -1.0f, 0.0f);
    quadcoords[5] = glm::vec3(1.0f, 1.0f, 0.0f);
    QuadBuffer.update(quadcoords);

    triVer rabbitSpec = loadOffFile("../data/rabbit.off");
    int triangleNum = rabbitSpec.b;

    // every 9 values represent 1 triangle:
    // vertexA.x, vertexA.y, vertexA.z
    // vertexB.x, vertexB.y, vertexB.
    // vertexC.x, vertexC.y, vertexC.z
    float vertexRabbit[triangleNum*9];
    loadVertexNormal(rabbitSpec, vertexRabbit);

    int lenArr = triangleNum*9;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_1D, tex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, lenArr, 0, GL_RED, GL_FLOAT, vertexRabbit);    
    glGenerateMipmap(GL_TEXTURE_1D);

    Program quad_program;
    // load fragment shader file 
    std::ifstream fragShaderFileFrame("../shader/rabbitFragmentCrystal.glsl");
    std::stringstream fragCodeFrame;
    fragCodeFrame << fragShaderFileFrame.rdbuf();
    // load vertex shader file
    std::ifstream vertShaderFileFrame("../shader/vertex.glsl");
    std::stringstream vertCodeFrame;
    vertCodeFrame << vertShaderFileFrame.rdbuf();

    quad_program.init(vertCodeFrame.str(), fragCodeFrame.str(), "outColor");
    quad_program.bind();
    quad_program.bindVertexAttribArray("position", QuadBuffer);

    glUniform1i(quad_program.uniform("arrLen"), lenArr);

    setUpLights(quad_program);
    setUpPlanes(quad_program);

    /*
     * Texture Program
     */

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GLuint FramebufferName;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	// The texture we're going to render to
	GLuint renderedTexture;
	glGenTextures(1, &renderedTexture);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Give an empty image to OpenGL ( the last "0" means "empty" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 480, 360, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	// Poor filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	// The depth buffer
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	// Create and compile our GLSL program from the shaders
    Program texture_program;
    // load fragment shader file 
    std::ifstream fragShaderFile("../shader/textureFragment.glsl");
    std::stringstream fragCode;
    fragCode << fragShaderFile.rdbuf();
    // load vertex shader file
    std::ifstream vertShaderFile("../shader/textureVertex.glsl");
    std::stringstream vertCode;
    vertCode << vertShaderFile.rdbuf();

    texture_program.init(vertCode.str(), fragCode.str(), "newColor");
    texture_program.bind();
    texture_program.bindVertexAttribArray("position", QuadBuffer);

    bool stopRender = false;
     while (!glfwWindowShouldClose(window))
    {  
        if (!stopRender){
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FramebufferName);
            glViewport(0, 0, 240, 180);

            VAO.bind();
            quad_program.bind();

            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);     
            glBindTexture(GL_TEXTURE_1D, tex);

            glDrawArrays(GL_TRIANGLES, 0, 6); 
            stopRender = true;
        }
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glViewport(0, 0, windowWidth*4, windowHeight*4);
        texture_program.bind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6); 

        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();

    }
}

