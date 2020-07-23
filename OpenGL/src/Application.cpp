// Tutorial.cpp : This file contains the 'main' function. Program execution begins and ends there.

#define GLEW_STATIC
#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <algorithm>
#include <map>
#include <string>
#include <array>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Mesh.h"
#include "Model.h"
#include "Timer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);
GLuint loadCubeMap(const std::vector<std::string>& faces);

// Settings
float SCR_WIDTH = 800;
float SCR_HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 20.0f, 110.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(50.0f, 15.0f, 120.0f);

int main()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Tutorial", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();
    std::cout << glGetString(GL_VERSION) << "\n\n";

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << "\n\n";

    stbi_set_flip_vertically_on_load(true);

    // SHADER PROGRAM CONFIGURATION
    // ----------------------------
    Shader instanceShader("res/shaders/Instancing/instance.vert", "res/shaders/Instancing/instance.frag");
    Shader modelShader("res/shaders/model.vert", "res/shaders/model.frag");

    // DATA SECTION
    // ------------
    constexpr unsigned int amount = 50000;
    std::vector<glm::mat4> rockModelMatrices;
    rockModelMatrices.reserve(amount);

    srand(glfwGetTime());
    float radius = 70.0f;
    float offset = 15.0f;
    for (unsigned int i = 0; i < amount; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        
        // 1. translation: displace along circle with 'radius' in range [-offset, offset] 
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x,y,z));

        // 2. scale: scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05f;
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a(semi)randomly picked rotation axis vector
        model = glm::rotate(model, (float)(rand() % 360), glm::vec3(0.5f, 0.1f, 0.3f));

        rockModelMatrices.push_back(model);
    }
    
    
    // VERTEX ARRAY OBJECT SECTION
    // ---------------------------
    Model planet("F:/Visual Studio File/LearnOpenGL/Models/planet/planet.obj");
    Model rock("F:/Visual Studio File/LearnOpenGL/Models/rock/rock.obj");

    // Instance buffer object
    GLuint instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &rockModelMatrices[0], GL_STATIC_DRAW);

    for (unsigned int i = 0; i < rock.meshes.size(); ++i)
    {
        GLuint VAO = rock.meshes[i].GetVAO();
        glBindVertexArray(VAO);
        // vertex attributes
        constexpr size_t vec4Size = sizeof(glm::vec4);
        
        for (int i = 0; i < 4; i++)
        {
            glEnableVertexAttribArray(i + 3);
            glVertexAttribPointer(i + 3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(i * vec4Size));
            glVertexAttribDivisor(i + 3, 1);
        }
        
        glBindVertexArray(0);
    }

    // TEXTURE CONFIGURATION
    // ---------------------   


    // OpenGL CONFIGURATION
    //---------------------
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_PROGRAM_POINT_SIZE);
    //glEnable(GL_CULL_FACE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //UNIFORM SHADER SETTING SECTION
    //------------------------------
    

    // RENDER LOOP
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // Timing logic
        const float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Handle input
        ProcessInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(5.0f));
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.mFOV), SCR_WIDTH / SCR_HEIGHT, 0.1f, 250.0f);
        
        modelShader.Use();
        modelShader.SetUniformMat4("model", model);
        modelShader.SetUniformMat4("view", view);
        modelShader.SetUniformMat4("projection", projection);
        modelShader.SetUniform1f("material.shininess", 0.4f * 128.0f);
        modelShader.SetUniformVec3("ambient", glm::vec3(0.05f));
        modelShader.SetUniformVec3("viewPos", camera.mPosition);

        modelShader.SetUniformVec3("pointLight.position", lightPos);
        modelShader.SetUniformVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
        modelShader.SetUniform1f("pointLight.linear", 0.0003f);
        modelShader.SetUniform1f("pointLight.quadratic", 0.00010f);
       
        planet.Draw(modelShader);

        // rock
        instanceShader.Use();
        instanceShader.SetUniformMat4("view", view);
        instanceShader.SetUniformMat4("projection", projection);
        instanceShader.SetUniform1f("material.shininess", 0.4f * 128.0f);
        instanceShader.SetUniformVec3("ambient", glm::vec3(0.05f));
        instanceShader.SetUniformVec3("viewPos", camera.mPosition);

        instanceShader.SetUniformVec3("pointLight.position", lightPos);
        instanceShader.SetUniformVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
        instanceShader.SetUniform1f("pointLight.linear", 0.0003f);
        instanceShader.SetUniform1f("pointLight.quadratic", 0.00010f);

        for (size_t i = 0; i < rock.meshes.size(); ++i)
        {
            glBindVertexArray(rock.meshes[i].GetVAO());
            glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.mDoubleSpeed = true;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        camera.mDoubleSpeed = false;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

GLuint loadCubeMap(const std::vector<std::string>& faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    stbi_set_flip_vertically_on_load(false);
    int width, height, nrChannels;
    for (size_t i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
        stbi_image_free(data);
    }
    stbi_set_flip_vertically_on_load(true);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    float xoffset = xPos - lastX;
    float yoffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}