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
#include "CullingAlgorithms.h"

#include <iostream>
#include<list>
#include<memory>


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

void DrawModels(int i, int j, int k, Shader& ourShader, Model& ourModel);

void DrawModels(Shader& ourShader, Model& ourModel);

//settings
const unsigned int screenWidth = 1200;
const unsigned int screenHeight = 800;

//camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
Camera secondCam(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;


//timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool wireFrame = false;

//culling bools
bool isFrustumCulling = false;
bool isZCulling = false;

//object cube variables
int xAxisObjects = 25;
int yAxisObjects = 25;
int zAxisObjects = 25;

//model transform
glm::vec3 modelRotation = glm::vec3(0.0f, 0.0f, 0.0f);

//-----SET ENVIRONMENT BOOLS HERE-----
enum EnvironmentEnum
{
    DENSE,  //a densely packed environment
    SPARSE, //a sparsely packed environment
    DYNAMIC, //a dynamimc environment
    DEFAULT //default value for testing
};

void DrawModels(int i, int j, int k, Shader& ourShader, Model& ourModel, glm::vec3 modelScale)
{
    glm::mat4 iteratedModel = glm::mat4(1.0f);
    iteratedModel = glm::translate(iteratedModel, glm::vec3(25.0f * i, 25.0f * j, 25.0f * k));  //this changes the default position of the model
    iteratedModel = glm::scale(iteratedModel, modelScale);	// scale
    iteratedModel = glm::rotate(iteratedModel, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));

    ourShader.setMat4("model", iteratedModel);
    ourModel.Draw(ourShader);
}

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
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "DissertationArtefact", nullptr, nullptr);
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

    glDepthFunc(GL_LESS);

    glCullFace(GL_BACK);    //tell opengl to cull the back faces
    glFrontFace(GL_FRONT);  //tell opengl which faces are the front faces

    enum EnvironmentEnum chosenEnvironment;

    //----------SELECT ENVIRONMENT HERE----------
    //------CHOICES: DENSE, SPARSE, DYNAMIC------

    //chosenEnvironment = DENSE;
    chosenEnvironment = SPARSE;
    //chosenEnvironment = DYNAMIC;
    //chosenEnvironment = DEFAULT;

    //-------------------------------------------

#pragma region Model fbx

    //Make sure there is the exact number of models that need to be rendered
    //otherwise the model faces are processed, but not draw, which will add to the face count

    Model ourModel("Sphere.fbx");
    BoundingBoxObjectClass ourBoundingBox(ourModel);
    ourBoundingBox.transform.SetPos({ 0, 0, 0 });
    const float scale = 10.0;
    ourBoundingBox.transform.SetSca({ scale, scale, scale });

    switch (chosenEnvironment)
    {
        case(DENSE): //densely packed scene
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

                        lastBoundingBox->transform.SetPos({ i * 2.5f, j * 2.5f, k * 2.5f });    //this line of code changes the position when frustum culling is active - i can't remember why, but it just does
                    }
                }
            }
            ourBoundingBox.UpdateSelfAndChild();
            break;
        }
        case(SPARSE): //sparsely packed scene
        {
            std::cout << "baseline model drawing for sparse scene" << std::endl;

            BoundingBoxObjectClass* lastBoundingBox = &ourBoundingBox;

            for (int i = 0; i < xAxisObjects; i++)
            {
                for (int j = 0; j < yAxisObjects; j++)
                {
                    for (int k = 0; k < zAxisObjects; k++)
                    {
                        ourBoundingBox.AddChild(ourModel);
                        lastBoundingBox = ourBoundingBox.children.back().get();

                        lastBoundingBox->transform.SetPos({ i * 10.0f, j * 10.0f, k * 10.0f });
                    }
                }
            }
            ourBoundingBox.UpdateSelfAndChild();
            break;
        }
        case(DYNAMIC): //dynamic scene (its got moving parts in it :O)
        {
            std::cout << "baseline model drawing for dyanmic scene" << std::endl;
            break;
        }
        default: //robustness check to prevent random enum values
        {
            throw std::invalid_argument("Invalid chosen environment enum chosen, please choose a valid enum"); //throw an exception to prevent incorrect environments
            break;
        }

    }


#pragma endregion

    glfwSwapInterval(0);    //disable vsync to allow for unlimited framerate

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
        //std::cout << "Current Frame: " << currentFrame << std::endl;    //currentFrame acts as a timer

        processInput(window);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, farPlane);
        const Frustum camView = CreateCameraBounds(camera, (float)screenWidth / (float)screenHeight, glm::radians(camera.Zoom), 0.1f, farPlane);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        unsigned int total = 0, display = 0;
     
        switch (chosenEnvironment) //set the value for the chosenEnvironment enum at the top
        {
            case(DENSE):    //densely packed scene
            {
                if (isFrustumCulling)
                {
                    RunFrustumCulling(ourBoundingBox, camView, ourShader, display, total);
                }
                else
                {
                    for (int i = 0; i < xAxisObjects; i++)
                    {
                        for (int j = 0; j < yAxisObjects; j++)
                        {
                            for (int k = 0; k < zAxisObjects; k++)
                            {
                                glm::vec3 iteratedModelPos = glm::vec3(0.0f * i, 0.0f * j, 0.0f * k);

                                if (isZCulling)
                                {
                                    glm::vec4 viewPos = view * glm::vec4(iteratedModelPos, 1.0f);

                                    int retFlag;
                                    RunZCulling(viewPos, retFlag);
                                    if (retFlag == 3) continue;
                                }

                                DrawModels(i, j, k, ourShader, ourModel, glm::vec3(10.0f, 10.0f, 10.0f));
                            }
                        }
                    }
                }
                break;
            }
            case(SPARSE):   //sparsely packed scene
            {
                if (isFrustumCulling)
                {
                    RunFrustumCulling(ourBoundingBox, camView, ourShader, display, total);
                }
                else
                {
                    for (int i = 0; i < xAxisObjects; i++)
                    {
                        for (int j = 0; j < yAxisObjects; j++)
                        {
                            for (int k = 0; k < zAxisObjects; k++)
                            {
                                glm::vec3 iteratedModelPos = glm::vec3(00.0f * i, 0.0f * j, 0.0f * k);

                                if (isZCulling)
                                {
                                    glm::vec4 viewPos = view * glm::vec4(iteratedModelPos, 1.0f);

                                    int retFlag;
                                    RunZCulling(viewPos, retFlag);
                                    if (retFlag == 3) continue;
                                }

                                DrawModels(i, j, k, ourShader, ourModel, glm::vec3(10.0f, 10.0f, 10.0f));
                            }
                        }
                    }
                }
                break;
            }
            case(DYNAMIC):  //dynamic scene (its got moving parts in it :O)
            {
                std::cout << "render the dyanmic environment" << std::endl;
                break;
            }
            default:    //robustness check to prevent random enum values
                throw std::invalid_argument("Invalid chosen environment enum chosen, please choose a valid enum"); //throw an exception to prevent incorrect environments
                break;
        }
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

    RunBackFaceCulling(window);
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
        if (!isZCulling)
        {
            std::cout << "enable z-culling culling" << std::endl;
            isZCulling = true;
            farPlane = 1000.0f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        if (isZCulling)
        {
            std::cout << "disable z-culling culling" << std::endl;
            isZCulling = false;
            farPlane = 10000000000000000000.0f;
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