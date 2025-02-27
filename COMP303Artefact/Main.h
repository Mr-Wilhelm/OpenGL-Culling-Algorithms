#pragma once

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow* window);

void DrawModels(int i, int j, int k, Shader& ourShader, Model& ourModel);

void RunFrustumCulling(BoundingBoxObjectClass& ourBoundingBox, const Frustum& camView, Shader& ourShader, unsigned int& display, unsigned int& total);

void RunZCulling(glm::vec4& viewPos, int& retFlag);

void RunBackFaceCulling(GLFWwindow* window);

void DrawModels(Shader& ourShader, Model& ourModel);