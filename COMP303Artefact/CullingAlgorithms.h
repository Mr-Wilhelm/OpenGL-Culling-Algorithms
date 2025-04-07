#pragma once

#include "CamView.h"
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

float farPlane = 1000.0f;
bool isBackCulling;
bool isZCulling;
bool isFrustumCulling;

GLFWwindow* testWindow = glfwCreateWindow(1200, 800, "testWindow", nullptr, nullptr);

//model transform
glm::vec3 modelRotation = glm::vec3(0.0f, 0.0f, 0.0f);
float offsetDiv = 4.0f;
float offsetTime = 1.0f;
float offsetModelPos = 4.0f;

//environment variables
int xAxisObjects = 25;
int yAxisObjects = 25;
int zAxisObjects = 25;

unsigned long int globalModelsZCulled = 0;
unsigned long long int globalPolysZCulled = 0;

//poly count reduction percentage is approx 50% on cubes, and just above that for spheres.
//this program uses spheres, so for backface culling the poly count is reduced by 50% to be conservative.
//https://www.researchgate.net/profile/Norman-Badler/publication/228530273_A_fast_real-time_back-face_culling_approach/links/551043030cf2ba84483d424a/A-fast-real-time-back-face-culling-approach.pdf
//https://www.gamedev.net/reference/articles/article1088.asp
int numPolygons = (12288);    //hard coded due to time constraints

void DrawModels(int i, int j, int k, Shader& ourShader, Model& ourModel);

void DrawModels(Shader& ourShader, Model& ourModel);

void RunZCulling(glm::vec4& viewPos, int& retFlag, unsigned long int& modelsCulled)
{
    retFlag = 1;
    if (viewPos.z > farPlane)   //skip the rendering process of the model. This prevents it from being drawn outright
    {
        modelsCulled++;
        retFlag = 3;
        return;
    }
}
inline void RunBackFaceCulling(GLFWwindow* window)
{
    testWindow = window;
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
}
inline void RunFrustumCulling(BoundingBoxObjectClass& ourBoundingBox, const Frustum& camView, Shader& ourShader, unsigned int& display, unsigned int& total)
{
    ourBoundingBox.DrawSelfAndChild(camView, ourShader, display, total);
}

int TestFunction(int a, int b)
{
    return a + b;
}

void DrawModels(glm::vec3 modelPos, int i, int j, int k, Shader& ourShader, Model& ourModel, glm::vec3 modelScale)
{
    glm::mat4 iteratedModel = glm::mat4(1.0f);
    iteratedModel = glm::translate(iteratedModel, modelPos);
    iteratedModel = glm::scale(iteratedModel, modelScale);	// scale
    iteratedModel = glm::rotate(iteratedModel, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));

    ourShader.setMat4("model", iteratedModel);
    ourModel.Draw(ourShader);
}

void DrawDynamicEnvironment(BoundingBoxObjectClass& ourBoundingBox, float currentFrame, const Frustum& camView, Shader& ourShader, unsigned int& display, unsigned int& total, glm::mat4& view, Model& ourModel)
{
    //matrix maths for rotations learned here, then refactored - https://learnopengl.com/Getting-started/Transformations
    //and here as well - https://catlikecoding.com/unity/tutorials/rendering/part-1/
    if (isFrustumCulling)
    {
        for (auto& boxChild : ourBoundingBox.children)
        {
            float xOffset = 10.0f * glm::sin(currentFrame / offsetTime + boxChild->x);
            float yOffset = 10.0f * glm::sin(currentFrame / offsetTime + boxChild->y);
            float zOffset = 10.0f * glm::sin(currentFrame / offsetTime + boxChild->z);

            glm::vec3 vectorOffset = glm::vec3(xOffset / offsetDiv, yOffset / offsetDiv, zOffset / offsetDiv);

            glm::vec3 finalModelPos = glm::vec3(boxChild->x * offsetModelPos, boxChild->y * offsetModelPos, boxChild->z * offsetModelPos) + vectorOffset;

            boxChild->transform.SetPos(finalModelPos);
        }
        ourBoundingBox.UpdateSelfAndChild();
        RunFrustumCulling(ourBoundingBox, camView, ourShader, display, total);
    }
    else
    {
        unsigned long int modelsCulled = 0;
        unsigned long long int modelsZCulled = 0;

        for (int i = 0; i < xAxisObjects; i++)
        {
            for (int j = 0; j < yAxisObjects; j++)
            {
                for (int k = 0; k < zAxisObjects; k++)
                {
                    total++;
                    glm::vec3 iteratedModelPos = glm::vec3(40.0f * i, 40.0f * j, 40.0f * k);

                    //offset the starting position for each model so they rotate starting in a different position
                    float xOffset = 40.0f * glm::sin(currentFrame + i);
                    float yOffset = 40.0f * glm::cos(currentFrame + j);
                    float zOffset = 40.0f * glm::sin(currentFrame + k);

                    glm::vec3 vectorOffset = glm::vec3(xOffset, yOffset, zOffset);  //make the offset a vector

                    glm::vec3 finalModelPos = iteratedModelPos + vectorOffset;  //create end pos with the start pos and vector

                    if (isZCulling)
                    {
                        glm::vec4 viewPos = view * glm::vec4(iteratedModelPos, 1.0f);
                        if (viewPos.z < -0.1f && viewPos.z > -farPlane)
                        {

                        }
                        else
                        {
                            modelsZCulled++;
                            globalModelsZCulled = modelsZCulled;
                            globalPolysZCulled = globalModelsZCulled * numPolygons;
                        }
                    }

                    DrawModels(finalModelPos, i, j, k, ourShader, ourModel, glm::vec3(10.0f, 10.0f, 10.0f));    //draw models
                    display++;
                }
            }
        }
    }

}

void DrawSparseEnvironment(BoundingBoxObjectClass& ourBoundingBox, const Frustum& camView, Shader& ourShader, unsigned int& display, unsigned int& total, glm::mat4& view, Model& ourModel)
{
    if (isFrustumCulling)
    {
        RunFrustumCulling(ourBoundingBox, camView, ourShader, display, total);
    }
    else
    {
        unsigned long int modelsCulled = 0;
        unsigned long long int modelsZCulled = 0;

        for (int i = 0; i < xAxisObjects; i++)
        {
            for (int j = 0; j < yAxisObjects; j++)
            {
                for (int k = 0; k < zAxisObjects; k++)
                {
                    total++;
                    glm::vec3 iteratedModelPos = glm::vec3(100.0f * i, 100.0f * j, 100.0f * k);

                    if (isZCulling)
                    {
                        glm::vec4 viewPos = view * glm::vec4(iteratedModelPos, 1.0f);
                        if (viewPos.z < -0.1f && viewPos.z > -farPlane)
                        {

                        }
                        else
                        {
                            modelsZCulled++;
                            globalModelsZCulled = modelsZCulled;
                            globalPolysZCulled = globalModelsZCulled * numPolygons;
                        }
                    }

                    DrawModels(iteratedModelPos, i, j, k, ourShader, ourModel, glm::vec3(10.0f, 10.0f, 10.0f));
                    display++;
                }
            }
        }
    }
}

void DrawDenseEnvironment(BoundingBoxObjectClass& ourBoundingBox, const Frustum& camView, Shader& ourShader, unsigned int& display, unsigned int& total, glm::mat4& view, Model& ourModel)
{
    if (isFrustumCulling)
    {
        RunFrustumCulling(ourBoundingBox, camView, ourShader, display, total);
    }
    else
    {
        unsigned long int modelsCulled = 0;
        unsigned long int modelsZCulled = 0;

        for (int i = 0; i < xAxisObjects; i++)
        {
            for (int j = 0; j < yAxisObjects; j++)
            {
                for (int k = 0; k < zAxisObjects; k++)
                {
                    total++;
                    glm::vec3 iteratedModelPos = glm::vec3(25.0f * i, 25.0f * j, 25.0f * k);

                    if (isZCulling)
                    {
                        glm::vec4 viewPos = view * glm::vec4(iteratedModelPos, 1.0f);
                        if (viewPos.z < -0.1f && viewPos.z > -farPlane)
                        {

                        }
                        else
                        {
                            modelsZCulled++;
                            globalModelsZCulled = modelsZCulled;
                            globalPolysZCulled = globalModelsZCulled * numPolygons;
                        }
                    }

                    DrawModels(iteratedModelPos, i, j, k, ourShader, ourModel, glm::vec3(10.0f, 10.0f, 10.0f));
                    display++;
                }
            }
        }
        //std::cout << "Z Culled models: " << modelsZCulled << " Rendered Models: " << display - globalModelsZCulled << " RenderedPolygons: " << (display * numPolygons) - (globalModelsZCulled * numPolygons) << std::endl;
    }
}
