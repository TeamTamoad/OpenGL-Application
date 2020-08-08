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
#include "VertexArray.h"
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

// Main camera
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
    Shader instanceShader("res/shaders/instance.vert", "res/shaders/instance.frag");
    Shader modelShader("res/shaders/model.vert", "res/shaders/model.frag");
    Shader skyboxShader("res/shaders/cubemap.vert", "res/shaders/cubemap.frag");

    // DATA SECTION
    // ------------
    std::vector<float> skyboxVertices = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    constexpr unsigned int amount = 10000;
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
    Model planet("res/models/planet/planet.obj");
    Model rock("res/models/rock/rock.obj");
    
    // Skybox VAO
    VertexArray skyboxVAO;
    VertexLayout cubeLayout;
    cubeLayout.Push<float>(3);
    skyboxVAO.AddBuffer(VertexBuffer(skyboxVertices), cubeLayout);


    // Instance buffer object
    VertexBuffer instanceVBO(rockModelMatrices);
    rock.SetUpInstaceBuffer(instanceVBO);

    
    // TEXTURE CONFIGURATION
    // --------------------- 
    std::vector<std::string> skyboxSources = {
        "res/cubemaps/ulukai/corona_rt.png",
        "res/cubemaps/ulukai/corona_lf.png",
        "res/cubemaps/ulukai/corona_up.png",
        "res/cubemaps/ulukai/corona_dn.png",
        "res/cubemaps/ulukai/corona_bk.png",
        "res/cubemaps/ulukai/corona_ft.png"
    };
    GLuint cubeMapTex = loadCubeMap(skyboxSources);



    // OpenGL CONFIGURATION
    //---------------------
    glEnable(GL_DEPTH_TEST);


    //UNIFORM SHADER SETTING SECTION
    //------------------------------
    
    camera.SetBoostSpeed(7);

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
        modelShader.SetUniformVec3("ambient", glm::vec3(0.1f));
        modelShader.SetUniformVec3("viewPos", camera.mPosition);

        modelShader.SetUniformVec3("pointLight.position", lightPos);
        modelShader.SetUniformVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
        modelShader.SetUniform1f("pointLight.linear", 0.00003f);
        modelShader.SetUniform1f("pointLight.quadratic", 0.00001f);
       

        instanceShader.Use();
        instanceShader.SetUniformMat4("view", view);
        instanceShader.SetUniformMat4("projection", projection);
        instanceShader.SetUniform1f("material.shininess", 0.4f * 128.0f);
        instanceShader.SetUniformVec3("ambient", glm::vec3(0.1f));
        instanceShader.SetUniformVec3("viewPos", camera.mPosition);

        instanceShader.SetUniformVec3("pointLight.position", lightPos);
        instanceShader.SetUniformVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
        instanceShader.SetUniform1f("pointLight.linear", 0.00003f);
        instanceShader.SetUniform1f("pointLight.quadratic", 0.00001f);


        skyboxShader.Use();
        glDepthFunc(GL_LEQUAL);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.SetUniformMat4("view", view);
        skyboxShader.SetUniformMat4("projection", projection);
        skyboxShader.SetUniform1i("cubemapTex", 0);
        skyboxVAO.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTex);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

        planet.Draw(modelShader);
        rock.DrawInstace(instanceShader, amount);
        
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
        camera.mBoosted = true;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        camera.mBoosted = false;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::DOWN, deltaTime);
}

// Sources sorted by Right, Left, Top, Bottom, Front and Back
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