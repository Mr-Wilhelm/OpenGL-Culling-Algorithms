#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream> 
#include <iostream>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//camera settings
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//mouse settings
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
float fov;

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
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_call(GLFWwindow* window, double xPosIn, double yPosIn)
{

}



