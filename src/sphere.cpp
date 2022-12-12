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

// Last position of the mouse on click
double xpos, ypos;

// camera setup and matrix calculations
glm::vec3 cameraPos;
glm::vec3 cameraTarget;
glm::vec3 cameraDirection;
glm::vec3 cameraUp;
glm::vec3 cameraRight;
glm::mat4 viewMatrix;
glm::mat4 projMatrix;

float camRadius = 5.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Update the position of the first vertex if the left button is pressed
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Get the position of the mouse in the window
        glfwGetCursorPos(window, &xpos, &ypos);
        std::cout << xpos << " " << ypos << std::endl;
    }

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // temp variables
    glm::mat3 rot;
    // Update the position of the first vertex if the keys 1,2, or 3 are pressed
    switch (key)
    {
    case GLFW_KEY_A:
        rot = glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f), cameraUp);
        cameraPos = rot * cameraPos;
        cameraDirection = glm::normalize(cameraPos - cameraTarget);
        cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
        break;
    case GLFW_KEY_D:
        rot = glm::rotate(glm::mat4(1.0f), glm::radians(5.0f), cameraUp);
        cameraPos = rot * cameraPos;
        cameraDirection = glm::normalize(cameraPos - cameraTarget);
        cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
        break;
    case GLFW_KEY_W:
        rot = glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f), cameraRight);
        cameraPos = rot * cameraPos;
        cameraDirection = glm::normalize(cameraPos - cameraTarget);
        cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
        break;
    case GLFW_KEY_S:
        rot = glm::rotate(glm::mat4(1.0f), glm::radians(5.0f), cameraRight);
        cameraPos = rot * cameraPos;
        cameraDirection = glm::normalize(cameraPos - cameraTarget);
        cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
        break;
    case GLFW_KEY_UP:
        cameraPos -= cameraDirection * 0.25f;
        break;
    case GLFW_KEY_DOWN:
        cameraPos += cameraDirection * 0.25f;
        break;
    case GLFW_KEY_R:
        cameraPos = glm::vec3(0.0f, 0.0f, camRadius);
        cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        cameraDirection = glm::normalize(cameraPos - cameraTarget);
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
        break;
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GL_TRUE);
        break;
    default:
        break;
    }

}

BufferObject QuadBuffer;
BufferObject SphereObj;
BufferObject TriangleObj;

std::vector<glm::vec3> quadcoords(2);
std::vector<float> spherecoords(3);
std::vector<float> trianglecoords(3);


int main(void)
{
    GLFWwindow* window;
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    window = glfwCreateWindow(800, 600, "Hello OpenGL", NULL, NULL);
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

    float sphereObj[4];
    sphereObj[0] = 0.0f; //center.x
    sphereObj[1] = 0.0f; //center.y
    sphereObj[2] = -2.0f; //center.z
    sphereObj[3] = 1.0f; //radius

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_1D, tex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 4, 0, GL_RED, GL_FLOAT, sphereObj);    
    glGenerateMipmap(GL_TEXTURE_1D);


    Program quad_program;
    // load fragment shader file 
    std::ifstream fragShaderFileFrame("../shader/sphereFragment.glsl");
    std::stringstream fragCodeFrame;
    fragCodeFrame << fragShaderFileFrame.rdbuf();
    // load vertex shader file
    std::ifstream vertShaderFileFrame("../shader/vertex.glsl");
    std::stringstream vertCodeFrame;
    vertCodeFrame << vertShaderFileFrame.rdbuf();

    quad_program.init(vertCodeFrame.str(), fragCodeFrame.str(), "outColor");
    quad_program.bind();
    quad_program.bindVertexAttribArray("position", QuadBuffer);

     while (!glfwWindowShouldClose(window))
    {
        
        // Get the size of the window
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glViewport(0, 0, 1600, 1200);
        VAO.bind();
        quad_program.bind();

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);     
        glBindTexture(GL_TEXTURE_1D, tex);

		glDrawArrays(GL_TRIANGLES, 0, 6); 
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    
    }

}

