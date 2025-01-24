#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream> 
#include <iostream>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//mouse settings
Camera camera(glm::vec3(0.0, 0.0, 3.0f));
bool firstMouse = true;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;

//time settings
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//make a window object with those parameters
	GLFWwindow* window = glfwCreateWindow(800, 600, "Artefact", nullptr, nullptr);
	if (window == nullptr)	//check to see if the window loaded correctly
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))	//check to see if GLAD loaded correctly
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);	//setting the viewport dimenisons

	Shader ourShader("shader.vs", "shader.fs");

	//setting up vertex and index data and data buffers, and configure vertex attributes.
	float vertices[] =
	{
		0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f
	};
	unsigned int indices[] =
	{
		0, 1, 3,
		1, 2, 3
	};

	unsigned int VBO;	//vertex buffer object
	unsigned int VAO;	//vertex array objet
	unsigned int EBO;	//element buffer object

	glGenVertexArrays(1, &VAO);	//generate vertex array assigned to the address of the VAO
	glGenBuffers(1, &VBO);	//generate buffers assigned to the addresses of the VBO and EBO
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);	//bind the vertex array object

	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//bind vertex buffers
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //set vertex buffers

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);	//bind the index buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); //set the index buffer

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);	//configure vertex attributes
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);	//unbinding the buffer, because glVertexAttribPointer registers the VBO as the vertex attributes bound vertex buffer object.

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glm::mat4 projection = glm::perspective(glm::radians(camera.m_Zoom), (float)800 / (float)600, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	std::cout << "Hello World" << std::endl;
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACK, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn)
{
	float xPos = static_cast<float>(xPosIn);
	float yPos = static_cast<float>(yPosIn);

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yOffset));
}



