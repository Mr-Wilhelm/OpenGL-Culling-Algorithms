#pragma once

#include "CamView.h"
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

void RunFrustumCulling(BoundingBoxObjectClass& ourBoundingBox, const Frustum& camView, Shader& ourShader, unsigned int& display, unsigned int& total);

void RunZCulling(glm::vec4& viewPos, int& retFlag);

void RunBackFaceCulling(GLFWwindow * window);
