#pragma once

#include "CamView.h"
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

float farPlane = 1000.0f;
bool isBackCulling;

GLFWwindow* testWindow = glfwCreateWindow(1200, 800, "testWindow", nullptr, nullptr);

void RunZCulling(glm::vec4& viewPos, int& retFlag)
{
    retFlag = 1;
    if (viewPos.z > farPlane)
    {
        { retFlag = 3; return; };   //skip the rendering process of the model. This prevents it from being drawn outright
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

