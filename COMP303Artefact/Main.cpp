#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "CamView.h"

#include<list>
#include<memory>

#include <iostream>

class BoundingBoxObject : public Model //declare a bounding box inheriting from model
{
public:
    std::list<std::unique_ptr<BoundingBoxObjectClass>> children;
    BoundingBoxObjectClass* parent;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void DrawModels(Shader& ourShader, Model& ourModel);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
Camera secondCam(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float farPlane;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool wireFrame = false;

//culling bools
bool isBackCulling = false;
bool isFrustumCulling = false;
bool isOcclusionCulling = false;

//object cube variables
int xAxisObjects = 25;
int yAxisObjects = 25;
int zAxisObjects = 25;

//model transform
glm::vec3 modelRotation = glm::vec3(0.0f, 0.0f, 0.0f);

int main()
{
    //initialise GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //Creating Window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Setting up GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    stbi_set_flip_vertically_on_load(true);

    Shader ourShader("shader.vs", "shader.fs");

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);     // draw in wireframe
    glEnable(GL_DEPTH_TEST);

    glCullFace(GL_BACK);    //tell opengl to cull the back faces
    glFrontFace(GL_FRONT);  //tell opengl which faces are the front faces

#pragma region Model fbx

    //Make sure there is the exact number of models that need to be rendered
    //otherwise the model faces are processed, but not draw, which will add to the face count

    Model ourModel("Sphere.fbx");
    BoundingBoxObjectClass ourBoundingBox(ourModel);
    ourBoundingBox.transform.SetPos({ 0, 0, 0 });
    const float scale = 5.0;
    ourBoundingBox.transform.SetSca({ scale, scale, scale });

    {
        BoundingBoxObjectClass* lastBoundingBox = &ourBoundingBox;

        for (int i = 0; i < xAxisObjects; i++)
        {
            for (int j = 0; j < yAxisObjects; j++)
            {
                for (int k = 0; k < zAxisObjects; k++)
                {
                    ourBoundingBox.AddChild(ourModel);
                    lastBoundingBox = ourBoundingBox.children.back().get();

                    lastBoundingBox->transform.SetPos({ i * 5.0f, j * 5.0f, k * 5.0f });
                }
            }
        }
    }
    ourBoundingBox.UpdateSelfAndChild();

#pragma endregion

    glfwSwapInterval(0);    //disable vsync

    unsigned int fpsCounter = 0;

    //Main Loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime()); //getting deltaTime
        deltaTime = currentFrame - lastFrame;
        fpsCounter++;

        if (deltaTime >= 1.0f / 30.0f)
        {
            std::string framerate = std::to_string((1.0 / deltaTime) * fpsCounter);
            glfwSetWindowTitle(window, framerate.c_str());
            lastFrame = currentFrame;
            fpsCounter = 0;
        }

        processInput(window);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000000.0f);
        const Frustum camView = CreateCameraBounds(camera, (float)SCR_WIDTH / (float)SCR_HEIGHT, glm::radians(camera.Zoom), 0.1f, 10000000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        unsigned int total = 0, display = 0;
        if (isFrustumCulling)
        {
            ourBoundingBox.DrawSelfAndChild(camView, ourShader, display, total);
            //std::cout << "total processed in CPU: " << total << " / total sent to GPU: " << display << std::endl;
        }
        else
        {
            for (int i = 0; i < xAxisObjects; i++)
            {
                for (int j = 0; j < yAxisObjects; j++)
                {
                    for (int k = 0; k < zAxisObjects; k++)
                    {
                        glm::mat4 iteratedModel = glm::mat4(1.0f);
                        iteratedModel = glm::translate(iteratedModel, glm::vec3(25.0f * i, 25.0f * j, 25.0f * k));
                        iteratedModel = glm::scale(iteratedModel, glm::vec3(5.0f, 5.0f, 5.0f));	// scale
                        iteratedModel = glm::rotate(iteratedModel, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));

                        ourShader.setMat4("model", iteratedModel);
                        ourModel.Draw(ourShader);
                    }
                }
            }
        }


        //ourBoundingBox.UpdateSelfAndChild();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
void processInput(GLFWwindow* window)
{
    //key and mouse input checks
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        if (!isBackCulling)
        {
            glEnable(GL_CULL_FACE);
            isBackCulling = true;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        if (isBackCulling)
        {
            glDisable(GL_CULL_FACE);
            isBackCulling = false;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        if (!isFrustumCulling)
        {
            std::cout << "enable frustum culling" << std::endl;
            isFrustumCulling = true;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        if (isFrustumCulling)
        {
            std::cout << "disable frustum culling" << std::endl;
            isFrustumCulling = false;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        if (!isOcclusionCulling)
        {
            std::cout << "enable occlusion culling" << std::endl;
            isOcclusionCulling = true;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        if (isOcclusionCulling)
        {
            std::cout << "disable occlusion culling" << std::endl;
            isOcclusionCulling = false;
        }
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

        
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    //callback for mouse movement
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    //callback for scroll wheel
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}