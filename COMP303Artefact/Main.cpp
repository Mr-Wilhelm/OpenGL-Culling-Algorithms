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



//settings
const unsigned int screenWidth = 1200;
const unsigned int screenHeight = 800;

//camera
Camera camera(glm::vec3(250.0f, 250.0f, 1000.0f));
Camera secondCam(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;


//timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastTimeWritten = 0.0f;

bool wireFrame = false;

//data gathering variables
int iteration = 1;
std::string fileName = "DummyData.csv";
std::list<std::string> dataList;


float totalFrames = 0;  //total frames used to calculate average
unsigned long int totalModels = 0;  //total number of models per frame used to calculate average
unsigned long long int totalPolygons = 0;    //total number of polygons per frame used to  calculate average

bool hasStarted = false;

//-----SET ENVIRONMENT BOOLS HERE-----
enum EnvironmentEnum
{
    DENSE,  //a densely packed environment
    SPARSE, //a sparsely packed environment
    DYNAMIC, //a dynamimc environment
    DEFAULT //default value for testing
};



bool WriteData(std::string fileName, std::list<std::string> csvData)
{
    std::ofstream fileObject;
    fileObject.open(fileName, std::ios_base::app);  //append to the file rather than overwriting it

    for (const auto& data : csvData)
    {
        fileObject << data << std::endl;
    }
    fileObject.close();

    return true;
}

int main()
{
    //GLFW setup
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //Create window
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

    //prevent the mouse frmo being seen
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //GLAD setup
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    stbi_set_flip_vertically_on_load(true);

    Shader ourShader("shader.vs", "shader.fs"); //declare shader files

    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);

    glCullFace(GL_BACK);    //tell opengl to cull the back faces
    glFrontFace(GL_FRONT);  //tell opengl which faces are the front faces

    enum EnvironmentEnum chosenEnvironment; //declare environment enum

    //----------SELECT ENVIRONMENT HERE----------
    //------CHOICES: DENSE, SPARSE, DYNAMIC------

    //chosenEnvironment = DENSE;
    chosenEnvironment = SPARSE;
    //chosenEnvironment = DYNAMIC;
    //chosenEnvironment = DEFAULT;

    //-------------------------------------------

#pragma region Model Initialisation

    //Make sure there is the exact number of models that need to be rendered
    //otherwise the model faces are processed, but not draw, which will add to the face count

    Model ourModel("Sphere.fbx");   //initialise model variable
    BoundingBoxObjectClass ourBoundingBox(ourModel);    //initialise bounding box variable
    ourBoundingBox.transform.SetPos({ 0, 0, 0 });   //set positions of the bounding box relative to the model
    const float scale = 10.0;   //set scale of bounding boxes
    ourBoundingBox.transform.SetSca({ scale, scale, scale });   //set scale relative to the model

    switch (chosenEnvironment)
    {
        case(DENSE): //densely packed scene
        {
            BoundingBoxObjectClass* lastBoundingBox = &ourBoundingBox;  //get bounding box

            //iterate three dimensionally
            for (int i = 0; i < xAxisObjects; i++)
            {
                for (int j = 0; j < yAxisObjects; j++)
                {
                    for (int k = 0; k < zAxisObjects; k++)
                    {
                        ourBoundingBox.AddChild(ourModel);  //add bounding box as a child to the model
                        lastBoundingBox = ourBoundingBox.children.back().get(); //get the previous bounding box

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
            BoundingBoxObjectClass* lastBoundingBox = &ourBoundingBox;

            for (int i = 0; i < xAxisObjects; i++)
            {
                for (int j = 0; j < yAxisObjects; j++)
                {
                    for (int k = 0; k < zAxisObjects; k++)
                    {
                        ourBoundingBox.AddChild(ourModel);
                        lastBoundingBox = ourBoundingBox.children.back().get();

                        lastBoundingBox->transform.SetPos({ i * 40.0f, j * 40.0f, k * 40.0f });

                        lastBoundingBox->x = i; //assign the index of lastBoundingBox via a pointer - https://www.geeksforgeeks.org/arrow-operator-in-c-c-with-examples/
                        lastBoundingBox->y = j;
                        lastBoundingBox->z = k;
                    }
                }

            }
            ourBoundingBox.UpdateSelfAndChild();
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
    unsigned int total = 0;  //models rendered beforehand
    unsigned int display = 0;   //models currently being rendered on screen

    //Main Loop
    while (!glfwWindowShouldClose(window))
    {
        //std::cout << "build test";

        float currentFrame = static_cast<float>(glfwGetTime()); //getting deltaTime
        deltaTime = currentFrame - lastFrame;
        fpsCounter++;



        //std::cout << "Current Frame: " << currentFrame << std::endl;    //currentFrame acts as a timer
        if (deltaTime >= 1.0f / 30.0f)
        {
            if (currentFrame >= 14.0f && currentFrame <= 15.0f) //this sets models to 0 when it starts gathering data
            {
                fpsCounter = 0;
                total = 0;
                display = 0;
            }

            if (currentFrame >= 15.0f)  //start collecting data after 15 seconds, startup stats are inconsistent and can mess with averages. Prevent unusually high framerate average values
            {
                totalFrames += (1.0 / deltaTime) * fpsCounter;  //update totals
                totalPolygons += (display * numPolygons) - globalPolysZCulled;
                totalModels += display - globalModelsZCulled;

                std::string framerate = std::to_string((1.0 / deltaTime) * fpsCounter); //get framerate
                //TODO write to a list and send it all at the end
                std::string data = std::to_string(chosenEnvironment) +
                    " ," +
                    std::to_string(currentFrame) +  //time
                    " , " +
                    std::to_string(iteration) +  //iteration
                    " ," +
                    std::to_string((1.0 / deltaTime) * fpsCounter) +   //framerate
                    " ," +
                    std::to_string((display * numPolygons) - globalPolysZCulled) +     //poly count
                    " ," +
                    std::to_string(display - globalModelsZCulled);    //model count

                glfwSetWindowTitle(window, data.c_str());  //assign it to the title of the window (to avoid having to make UI)

                if (currentFrame - lastTimeWritten >= 1)
                {
                    //bool framerateWrite = WriteFramerate(fileName, std::to_string(chosenEnvironment), std::to_string(iteration), std::to_string(currentFrame), framerate, std::to_string(display * numPolygons), std::to_string(display));  //increment the counter after each run
                    dataList.push_back(data);
                }

                lastFrame = currentFrame;
                fpsCounter = 0;
                total = 0;
                display = 0;
            }
            
        }

        if (currentFrame >= 30.0f)
        {
            std::string data2 = std::to_string(iteration) +
                " ," + std::to_string(totalFrames / dataList.size()) +
                " ," + std::to_string(totalPolygons / dataList.size()) +
                " ," + std::to_string(totalModels / dataList.size());
            dataList.clear();
            dataList.push_back(data2);
            bool writeData = WriteData(fileName, dataList);
            glfwSetWindowShouldClose(window, true);
        }

        processInput(window);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, farPlane);   //projection matrix
        const Frustum camView = CreateCameraBounds(camera, (float)screenWidth / (float)screenHeight, glm::radians(camera.Zoom), 0.1f, farPlane);    //cam view frustum
        glm::mat4 view = camera.GetViewMatrix();    //get view matrix
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);


     
        switch (chosenEnvironment) //set the value for the chosenEnvironment enum at the top
        {
            case(DENSE):    //densely packed scene
            {
                DrawDenseEnvironment(ourBoundingBox, camView, ourShader, display, total, view, ourModel);
                break;
            }
            case(SPARSE):   //sparsely packed scene
            {
                DrawSparseEnvironment(ourBoundingBox, camView, ourShader, display, total, view, ourModel);
                break;
            }
            case(DYNAMIC):  //dynamic scene (its got moving parts in it :O)
            {
                DrawDynamicEnvironment(ourBoundingBox, currentFrame, camView, ourShader, display, total, view, ourModel);

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
//generic code for input processing
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