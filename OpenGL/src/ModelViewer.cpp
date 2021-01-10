#include "ModelViewer.h"

#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <iostream>
#include <random>

#include "Model.h"
#include "Shader.h"
#include "Timer.h"
#include "utility.h"


Camera ModelViewer::mCamera = Camera();
float ModelViewer::mScrHeight = 600;
float ModelViewer::mScrWidth = 800;
float ModelViewer::lastX = mScrWidth / 2.0f;
float ModelViewer::lastY = mScrHeight / 2.0f;
bool ModelViewer::firstMouse = false;

ModelViewer::ModelViewer()
{
    mCamera.mPosition = glm::vec3(0.0f, 3.0f, 7.0f);
    mCamera.updateCameraVectors();
}

int ModelViewer::run()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

    GLFWwindow* window = glfwCreateWindow(mScrWidth, mScrHeight, "OpenGL Tutorial", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen

    if (!window)
    {
        std::cout << "Failed to create GLFW window" << "\n";
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();
    std::cout << glGetString(GL_VERSION) << "\n\n";

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << "\n\n";

    // ----------------------------------------- IMGUI SETUP -------------------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // -------------------------------------- SHADER PROGRAM SETUP --------------------------------------------------
    Shader modelShader("res/shaders/model.vert", "res/shaders/model.frag");
    //modelShader.AddShader(GL_GEOMETRY_SHADER, "res/shaders/explode.geom");
    Shader normalVisualShader("res/shaders/normal_visualize.vert", "res/shaders/singleColor.frag");
    normalVisualShader.AddShader(GL_GEOMETRY_SHADER, "res/shaders/normal_visualize.geom");
    

    // VERTEX ARRAY OBJECT CONFIGURATION
    // ---------------------------------
    Model backpack("res/models/backpack/backpack.obj");


    // OpenGL CONFIGURATION
    //---------------------
    glEnable(GL_DEPTH_TEST);


    //SETTING SECTION
    //----------------
    float lastFrame = 0.0f;
    float normalLength = 0.15f;
    float normalColor[3] = {1.0f, 1.0f, 0.0f};
    bool isShowNorm = false;
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    // RENDER LOOP
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // Timing logic
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Handle input
        ProcessInput(window, deltaTime);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Setting Panel");

            ImGui::Text("Normal vector visualization");
            ImGui::Checkbox("Show Normal Vector", &isShowNorm);
            ImGui::SliderFloat("Lenght", &normalLength, 0.01f, 0.5f);
            ImGui::ColorPicker3("Color", normalColor);
            if (ImGui::Button("Default"))
            {
                normalLength = 0.15f;
                normalColor[0] = 1.0f;
                normalColor[1] = 1.0f;
                normalColor[2] = 0.0f;
                isShowNorm = false;
            }
            ImGui::SameLine(); ImGui::Text("Set all values to defaults");

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();

            ImGui::Begin("Control");
            ImGui::Text("W - move forward");
            ImGui::Text("A - move left");
            ImGui::Text("S - move backward");
            ImGui::Text("D - move right");
            ImGui::Text("SPACE BAR - move upward");
            ImGui::Text("CTRL- move downward");
            ImGui::Text("F - freeze the screen and show cursor");
            ImGui::Text("SHIFT - Accelerate the cameara speed");

            ImGui::End();
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(currentFrame * 12.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 view = mCamera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(mCamera.mFOV), mScrWidth / mScrHeight, 0.1f, 100.0f);

        // Set uniform values
        modelShader.Use();
        modelShader.SetUniformMat4("model", model);
        modelShader.SetUniformMat4("view", view);
        modelShader.SetUniformMat4("projection", projection);
        modelShader.SetUniform1f("material.shininess", 0.4f * 128.0f);
        //modelShader.SetUniform1f("time", currentFrame);
        modelShader.SetUniformVec3("ambient", glm::vec3(0.7f));
        modelShader.SetUniformVec3("viewPos", mCamera.mPosition);

        //point light
        modelShader.SetUniformVec3("pointLight.position", lightPos);
        modelShader.SetUniformVec3("pointLight.diffuse", 0.0f, 0.0f, 0.0f);
        modelShader.SetUniform1f("pointLight.linear", 0.07f);
        modelShader.SetUniform1f("pointLight.quadratic", 0.017f);

        backpack.Draw(modelShader);

        if (isShowNorm)
        {
            normalVisualShader.Use();
            normalVisualShader.SetUniformMat4("model", model);
            normalVisualShader.SetUniformMat4("view", view);
            normalVisualShader.SetUniformMat4("projection", projection);
            normalVisualShader.SetUniformVec3("color", normalColor[0], normalColor[1], normalColor[2]);
            normalVisualShader.SetUniform1f("magnitude", normalLength);
            backpack.DrawTextureless(normalVisualShader);
        }

        // Render GUI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void ModelViewer::ProcessInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        mCamera.mBoosted = true;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        mCamera.mBoosted = false;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mCamera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mCamera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mCamera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mCamera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        mCamera.ProcessKeyboard(CameraMovement::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        mCamera.ProcessKeyboard(CameraMovement::DOWN, deltaTime);
}

void ModelViewer::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    mScrWidth = width;
    mScrHeight = height;
}

void ModelViewer::mouse_callback(GLFWwindow* window, double xPos, double yPos)
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

    mCamera.ProcessMouseMovement(xoffset, yoffset);
}

void ModelViewer::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    mCamera.ProcessMouseScroll(yoffset);
}

void ModelViewer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        mCamera.mFreezed = !mCamera.mFreezed;
        if (mCamera.mFreezed)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetCursorPos(window, mScrWidth / 2, mScrHeight / 2);
        }
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}