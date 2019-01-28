#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>
#include <camera.h>
#include <Mesh.h>
#include <Model.h>
#include <GraphNode.h>
#include <GameManager.h>
#include <TextRenderer.h>

#include <iostream>
#include <cstdlib>
#include <ctime>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera* camera = new Camera(glm::vec3(0.0f, 0.0f, 30.0f));

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// speed
float skyboxSpeed = 0.1f;

float cubeVertices[] = {
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

float skyboxVertices[] = {
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

glm::vec3 lightDirection(-5.16f, 1.05f, 8.082f);
glm::vec3 lightPosition(0.0f, 0.0f, 1.0f);
glm::vec3 WorldLightDirection(0.0f);

glm::mat4 lightSpaceMatrix(1);
unsigned int depthMap;

float horizontalDirection = 0.0f;
float verticalDirection = 0.0f;
float spacebarPushed = false;
float enterPushed = false;

vector<std::string> faces
        {
                "res/textures/skybox/right.jpg",
                "res/textures/skybox/left.jpg",
                "res/textures/skybox/top.jpg",
                "res/textures/skybox/bottom.jpg",
                "res/textures/skybox/front.jpg",
                "res/textures/skybox/back.jpg"
        };

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Asteroids", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Initialize OpenGL loader
    #if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
        bool err = gl3wInit() != 0;
    #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
        bool err = glewInit() != GLEW_OK;
    #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
        bool err = !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    #endif
        if (err)
        {
            fprintf(stderr, "Failed to initialize OpenGL loader!\n");
            return -1;
        }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    srand(static_cast <unsigned> (time(0)));

    // build and compile shaders
    // -------------------------
    shared_ptr<Shader> ourShader = std::make_shared<Shader>(
            "res/shaders/shadowMapping.vert",
            "res/shaders/shadowMapping.frag");

    shared_ptr<Shader> skyBoxShader = std::make_shared<Shader>(
            "res/shaders/skybox.vs",
            "res/shaders/skybox.fs");

    shared_ptr<Shader>textShader = std::make_shared<Shader>(
            "res/shaders/text.vert",
            "res/shaders/text.frag");

    shared_ptr<Shader> depthShader = std::make_shared<Shader>(
            "res/shaders/depthShader.vert",
            "res/shaders/depthShader.frag");

    shared_ptr<Shader> debugDepth = std::make_shared<Shader>(
            "res/shaders/debugDepth.vert",
            "res/shaders/debugDepth.frag");

    shared_ptr<Shader> reflectShader = std::make_shared<Shader>(
            "res/shaders/reflect.vs",
            "res/shaders/reflect.fs");

    shared_ptr<Shader> refractShader = std::make_shared<Shader>(
            "res/shaders/reflect.vs",
            "res/shaders/refract.fs");

    Model* spaceShip = new Model("res\\models\\wraith\\Wraith Raider Starship.obj");
    Model* bullet = new Model("res\\models\\bullet\\bullet.obj");
    Model* ast = new Model("res\\models\\meteor\\rock large.obj");
    Model* moonModel = new Model("res\\models\\moon\\moon.obj");

    spaceShip->SetShader(ourShader.get());
    bullet->SetShader(ourShader.get());
    ast->SetShader(ourShader.get());
    moonModel->SetShader(ourShader.get());

    GraphNode* root = new GraphNode();
    GraphNode* gameRoot = new GraphNode();
    GraphNode* ship = new GraphNode(spaceShip, glm::vec2(-2.0f, -2.0f), glm::vec2(2.0f, 2.0f));
    GraphNode* laserBullet = new GraphNode(bullet, glm::vec2(-0.5f, -0.3f), glm::vec2(0.5f, 0.3f));
    GraphNode* asteroid = new GraphNode(ast, glm::vec2(-2.0f, -2.0f), glm::vec2(2.0f, 2.0f));
    GraphNode* moonPivot = new GraphNode();
    GraphNode* moon = new GraphNode(moonModel);
    shared_ptr<GraphNode> graph(gameRoot);

    root->AddChild(gameRoot);
    root->AddChild(ship);
    root->AddChild(laserBullet);
    root->AddChild(asteroid);
    root->AddChild(moonPivot);
    moonPivot->AddChild(moon);

    ship->setPosition(-10.0f, 0, 0);
    ship->Scale(glm::vec3(0.005f, 0.008f, 0.008f));
    ship->Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    ship->Rotate(75.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    laserBullet->setPosition(0, 0, 1);
    laserBullet->Scale(glm::vec3(0.05f, 0.05f, 0.05f));
    laserBullet->Active(false);

    asteroid->setPosition(50, 0, 0);
    asteroid->Scale(glm::vec3(3.0f, 5.0f, 5.0f));
    asteroid->Active(false);

    moonPivot->setPosition(0, 0, -10);
    moon->setPosition(moonPivot->getPosition().x, moonPivot->getPosition().y, moonPivot->getPosition().z);
    moon->Scale(glm::vec3(0.05, 0.05, 0.05));

    shared_ptr<TextRenderer> textPtr = std::make_shared<TextRenderer>(SCR_WIDTH, SCR_HEIGHT, textShader.get());
    textPtr.get()->Load("res/fonts/arial.ttf", 30);

    GameManager gameManager(graph, &horizontalDirection, &verticalDirection);
    gameManager.setPlayer(ship);
    gameManager.setBullet(laserBullet);
    gameManager.SetAsteroid(asteroid);
    gameManager.SetTextRenderer(textPtr);

    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_WIDTH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    //glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    ///////////////////////////////////////////////
    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    // cube VAO2
    unsigned int cubeVAO2, cubeVBO2;
    glGenVertexArrays(1, &cubeVAO2);
    glGenBuffers(1, &cubeVBO2);
    glBindVertexArray(cubeVAO2);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));


    // skybox VAO
    unsigned int cubemapTexture = loadCubemap(faces);
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    skyBoxShader->use();
    skyBoxShader->setInt("skybox", 0);

    // reflect
    reflectShader->use();
    reflectShader->setInt("skybox", 0);
    glm::mat4 reflectModel = glm::mat4(1.0f);
    reflectModel = glm::scale(reflectModel, glm::vec3(3.0f));
    reflectModel = glm::translate(reflectModel, glm::vec3(6.0f, -3.0f, 0.0f));

    // refract
    refractShader->use();
    refractShader->setInt("skybox", 0);
    glm::mat4 refractModel = glm::mat4(1.0f);
    refractModel = glm::scale(refractModel, glm::vec3(3.0f));
    refractModel = glm::translate(refractModel, glm::vec3(6.0f, 3.0f, 0.0f));


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // ------
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (gameManager.GetState() == IN_GAME)
        {
            moon->Rotate(3.0f * deltaTime, glm::vec3(0.2f, 1.0f, 0.3f));
            moonPivot->Rotate(14.0f * deltaTime, glm::vec3(0.4f, 0.8f, 0.4f));
        }

        // draw scene as normal
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera->GetViewMatrix();

        glm::mat4 DirLightPosition(1);
        DirLightPosition[3][0] = lightDirection.x;
        DirLightPosition[3][1] = lightDirection.y;
        DirLightPosition[3][2] = lightDirection.z;
        DirLightPosition = graph->GetWorldTransform() * DirLightPosition;
        WorldLightDirection.x = DirLightPosition[3][0];
        WorldLightDirection.y = DirLightPosition[3][1];
        WorldLightDirection.z = DirLightPosition[3][2];

        glm::mat4 lightProjection(1), lightView(1);

        float near_plane = -10.0f, far_plane = 100.0f;
        lightProjection = glm::ortho(-100.0f, 100.0f, -80.0f, 80.0f, near_plane, far_plane);
        lightView = glm::lookAt(WorldLightDirection, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        //directional light shadow map drawing
        depthShader->use();
        depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        root->SetShader(depthShader.get());
        root->Draw();
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        /* 1. wygeneruj mape glbokosci
         2. normalnie wyrenderuj scene korzystajac z mapy glebokosci (cubemap)*/

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        root->SetShader(ourShader.get());
        ourShader->use();
        ourShader->setMat4("view", view);
        ourShader->setMat4("projection", projection);
        ourShader->setVec3("viewPos", camera->Position);
        ourShader->setVec3("lightPos", lightPosition);
        ourShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        ourShader->setInt("shadowMap", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        root->Draw();

        // reflect
        reflectShader->use();

        if (gameManager.GetState() == IN_GAME)
        {
            reflectModel = glm::rotate(reflectModel, deltaTime * 0.01f, glm::vec3(0.3f, 0.7f, 0.0f));

        }

        reflectShader->setMat4("model", reflectModel);
        reflectShader->setMat4("view", view);
        reflectShader->setMat4("projection", projection);
        reflectShader->setVec3("cameraPos", camera->Position);
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // refract
        refractShader->use();

        if (gameManager.GetState() == IN_GAME)
        {
            refractModel = glm::rotate(refractModel, deltaTime * 0.02f, glm::vec3(0.3f, 0.7f, 0.0f));

        }

        refractShader->setMat4("model", refractModel);
        refractShader->setMat4("view", view);
        refractShader->setMat4("projection", projection);
        refractShader->setVec3("cameraPos", camera->Position);
        glBindVertexArray(cubeVAO2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyBoxShader->use();
        model = glm::mat4(1.0f);

        if (gameManager.GetState() == IN_GAME)
        {
            model = glm::rotate(model, currentFrame * skyboxSpeed, glm::vec3(-0.5f, 0.8f, -0.2f));

        }

        view = glm::mat4(glm::mat3(camera->GetViewMatrix())); // remove translation from the view matrix
        skyBoxShader->setMat4("model", model);
        skyBoxShader->setMat4("view", view);
        skyBoxShader->setMat4("projection", projection);

        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        gameManager.SetFrameTime(deltaTime);
        gameManager.spacebarPushed(spacebarPushed);
        gameManager.enterPushed(enterPushed);
        gameManager.GameOps();

        root->Update(deltaTime * 5);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVAO);
    root->~GraphNode();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        verticalDirection = 1.0f;
        horizontalDirection = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        verticalDirection = -1.0f;
        horizontalDirection = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        horizontalDirection = -1.0f;
        verticalDirection = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        horizontalDirection = 1.0f;
        verticalDirection = 0.0f;
    }
    else {
        horizontalDirection = 0.0f;
        verticalDirection = 0.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        spacebarPushed = true;
    else
        spacebarPushed = false;

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        enterPushed = true;
    else
        enterPushed = false;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
