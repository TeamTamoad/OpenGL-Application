#include "GalaxyAdventure.h"

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


Camera GalaxyAdventure::mCamera = Camera();
float GalaxyAdventure::mScrHeight = 600;
float GalaxyAdventure::mScrWidth = 800;
float GalaxyAdventure::lastX = mScrWidth / 2.0f;
float GalaxyAdventure::lastY = mScrHeight / 2.0f;
bool GalaxyAdventure::firstMouse = false;

GalaxyAdventure::GalaxyAdventure()
{
    mCamera.mPosition = glm::vec3(0.0f, 20.0f, 110.0f);
    mCamera.mMovementSpeed = 5.0f;
    mCamera.mBoostSpeed = 5.0f;
    mCamera.updateCameraVectors();
}

int GalaxyAdventure::run()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

    GLFWwindow* window = glfwCreateWindow(mScrWidth, mScrHeight, "OpenGL Tutorial", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(1920, 1080, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen

    if (!window)
    {
        std::cout << "Failed to create GLFW window" << "\n";
        glfwTerminate();
        return -1;
    }

    // Set callback functions
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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
    Shader instanceShader("res/shaders/instance.vert", "res/shaders/instance.frag");
    Shader modelShader("res/shaders/model.vert", "res/shaders/model.frag");
    Shader skyboxShader("res/shaders/cubemap.vert", "res/shaders/cubemap.frag");


    // ----------------------------------------- DATA SECTION -------------------------------------------------------
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

    // Prepare model matrices for rock
    std::vector<glm::mat4> rockModelMatrices = prepareRockData(100000, glfwGetTime());

    //Light levels
    struct PointLight
    {
        float linear;
        float quadratic;
    };

    std::vector<PointLight> lightParameters = {
        { 0.027f,  0.0028f   },
        { 0.022f,  0.0019f   },
        { 0.014f,  0.0007f   },
        { 0.007f,  0.0002f   },
        { 0.004f,  0.00015f  },
        { 0.002f,  0.00010f  },
        { 0.0017f, 0.00005f  },
        { 0.0014f, 0.000007f }
    };

    // ----------------------------------------- VERTEX ARRAY OBJECT SECTION -----------------------------------------
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


    //SETTING SECTION
    //----------------
    float lastFrame = 0.0f;

    float ambientValue = 0.1f;
    int   lightLevel = 6;
    float lightHeight = 20.0f;
    float lightAngle = 20.0f;
    float planetScale = 5.0f;
    int   rockRenderAmount = 10000;

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
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Setting Panel");

            ImGui::Text("Light setting");
            ImGui::SliderFloat("Ambient", &ambientValue, 0.0f, 1.0f);
            ImGui::SliderInt("Light Level", &lightLevel, 1, lightParameters.size());
            ImGui::SliderFloat("Height", &lightHeight, -40.0f, 40.0f);
            ImGui::SliderFloat("Angle", &lightAngle, 0.0f, 360.0f);

            ImGui::Text("Object setting");
            ImGui::SliderFloat("Planet scale", &planetScale, 1.0f, 10.0f);
            ImGui::SliderInt("Rock amount", &rockRenderAmount, 0, 100000);

            ImGui::Text("Camera setting");
            ImGui::SliderFloat("Speed", &mCamera.mMovementSpeed, 1.0f, 10.0f);
            ImGui::SliderFloat("Speed-up", &mCamera.mBoostSpeed, 1.0f, 10.0f);

            if (ImGui::Button("Default"))
            {
                ambientValue = 0.1f;
                lightLevel = 6;
                rockRenderAmount = 10000;
                lightHeight = 20.0f;
                lightAngle = 20.0f;
                planetScale = 5.0f;
                mCamera.mMovementSpeed = 5.0f;
                mCamera.mBoostSpeed = 5.0f;
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
        model = glm::scale(model, glm::vec3(planetScale));
        glm::mat4 view = mCamera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(mCamera.mFOV), mScrWidth / mScrHeight, 0.1f, 250.0f);

        // Calculate light position
        glm::vec3 lightPos(0.0f, 0.0f, 120.0f);
        lightPos += glm::vec3(0.0f, lightHeight, 0.0f);
        lightPos = glm::rotate(lightPos, glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));

        modelShader.Use();
        modelShader.SetUniformMat4("model", model);
        modelShader.SetUniformMat4("view", view);
        modelShader.SetUniformMat4("projection", projection);
        modelShader.SetUniform1f("material.shininess", 0.4f * 128.0f);
        modelShader.SetUniformVec3("ambient", glm::vec3(ambientValue));
        modelShader.SetUniformVec3("viewPos", mCamera.mPosition);

        modelShader.SetUniformVec3("pointLight.position", lightPos);
        modelShader.SetUniformVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
        modelShader.SetUniform1f("pointLight.linear", lightParameters[size_t(lightLevel) - 1].linear);
        modelShader.SetUniform1f("pointLight.quadratic", lightParameters[size_t(lightLevel) - 1].quadratic);


        instanceShader.Use();
        instanceShader.SetUniformMat4("view", view);
        instanceShader.SetUniformMat4("projection", projection);
        instanceShader.SetUniform1f("material.shininess", 0.4f * 128.0f);
        instanceShader.SetUniformVec3("ambient", glm::vec3(ambientValue));
        instanceShader.SetUniformVec3("viewPos", mCamera.mPosition);

        instanceShader.SetUniformVec3("pointLight.position", lightPos);
        instanceShader.SetUniformVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
        instanceShader.SetUniform1f("pointLight.linear", lightParameters[size_t(lightLevel) - 1].linear);
        instanceShader.SetUniform1f("pointLight.quadratic", lightParameters[size_t(lightLevel) - 1].quadratic);


        skyboxShader.Use();
        glDepthFunc(GL_LEQUAL);
        view = glm::mat4(glm::mat3(mCamera.GetViewMatrix()));
        skyboxShader.SetUniformMat4("view", view);
        skyboxShader.SetUniformMat4("projection", projection);
        skyboxShader.SetUniform1i("cubemapTex", 0);
        skyboxVAO.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTex);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

        planet.Draw(modelShader);
        rock.DrawInstace(instanceShader, rockRenderAmount);

        // Render GUI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void GalaxyAdventure::ProcessInput(GLFWwindow* window, float deltaTime)
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

std::vector<glm::mat4> GalaxyAdventure::prepareRockData(int amount, double seed)
{
    std::vector<glm::mat4> rockModelMatrices;
    rockModelMatrices.reserve(amount);

    float radius = 70.0f;
    float offset = 15.0f;
    Timer timer;

    // TODO: Test Prepare model using Random generator
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<float> distOffset(-offset, offset);
    std::uniform_real_distribution<float> distScale(0.05f, 0.25f);
    std::uniform_real_distribution<float> distRotate(0.0f, 360.0f);
    auto randOffset = std::bind(distOffset, rng);
    auto randScale = std::bind(distScale, rng);
    auto randRotate = std::bind(distRotate, rng);

    timer.setStart();
    for (unsigned int i = 0; i < amount; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);

        // 1. translation: displace along circle with 'radius' in range [-offset, offset] 
        float angle = (float)i / (float)amount * 360.0f;
        float x = sin(angle) * radius + randOffset();
        float y = randOffset() * 0.4f;
        float z = cos(angle) * radius + randOffset();
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: scale between 0.05 and 0.25f
        model = glm::scale(model, glm::vec3(randScale()));

        // 3. rotation: add random rotation around a(semi)randomly picked rotation axis vector
        model = glm::rotate(model, randRotate(), glm::vec3(0.5f, 0.1f, 0.3f));

        rockModelMatrices.push_back(model);
    }
    timer.printDuration("Prepare rock models");
    return rockModelMatrices;
}

void GalaxyAdventure::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    mScrWidth = width;
    mScrHeight = height;
}

void GalaxyAdventure::mouse_callback(GLFWwindow* window, double xPos, double yPos)
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

void GalaxyAdventure::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    mCamera.ProcessMouseScroll(yoffset);
}

void GalaxyAdventure::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
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