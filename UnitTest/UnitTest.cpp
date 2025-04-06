#include "pch.h"
#include "CppUnitTest.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "CamView.h"
#include "CullingAlgorithms.h"

#include<list>
#include<memory>

#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	class BoundingBoxObject : public Model //declare a bounding box inheriting from model
	{
	public:
		std::list<std::unique_ptr<BoundingBoxObjectClass>> children;
		BoundingBoxObjectClass* parent;
	};

	class DummyModel : public Model
	{
	public:
		DummyModel()
		{
			// Create a window and OpenGL context using GLFW
			if (!glfwInit()) {
				std::cerr << "GLFW init failure" << std::endl;
				return;
			}

			GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Test", nullptr, nullptr);
			if (!window) {
				std::cerr << "Failed to create GLFW window!" << std::endl;
				glfwTerminate();
				return;
			}

			glfwMakeContextCurrent(window);
			//the other stuff is unnecessary
			//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
			//glfwSetCursorPosCallback(window, mouse_callback);
			//glfwSetScrollCallback(window, scroll_callback);

			// Initialize GLAD to load OpenGL functions
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
				std::cerr << "Failed to initialize GLAD!" << std::endl;
				return;
			}

			std::vector<Vertex> dummyVertexArray;

			Vertex dummyV1, dummyV2, dummyV3;	//make a dummy triangle
			dummyV1.pos = glm::vec3(1.0f, 0.0f, 0.0f);
			dummyV1.norm = glm::vec3(0.0f, 1.0f, 0.0f);
			dummyV1.texCoord = glm::vec2(1.0f, 1.0f);

			dummyV2.pos = glm::vec3(0.0f, 1.0f, 0.0f);
			dummyV2.norm = glm::vec3(0.0f, 1.0f, 0.0f);
			dummyV2.texCoord = glm::vec2(1.0f, 1.0f);

			dummyV3.pos = glm::vec3(0.0f, 0.0f, 1.0f);
			dummyV3.norm = glm::vec3(0.0f, 1.0f, 0.0f);
			dummyV3.texCoord = glm::vec2(1.0f, 1.0f);

			dummyVertexArray.push_back(dummyV1);
			dummyVertexArray.push_back(dummyV2);
			dummyVertexArray.push_back(dummyV3);

			std::vector<Texture> dummyTextureArray;
			Texture dummyTex;
			dummyTex.id = 1;
			dummyTex.texType = "texture_diffuse";
			dummyTex.texPath = "texture_diffuse.png";
			dummyTextureArray.push_back(dummyTex);

			std::vector<unsigned int> dummyIndexArray;
			dummyIndexArray.push_back(0);
			dummyIndexArray.push_back(1);
			dummyIndexArray.push_back(2);

			meshes.push_back(Mesh(dummyVertexArray, dummyTextureArray, dummyIndexArray));
		}
	};

	TEST_CLASS(UnitTest)
	{
	public:	
		TEST_METHOD(UnitTest_ZCulling)
		{
			try
			{
				glm::vec4 camPos(0.0f, 0.0f, 1000.0f, 1.0f);
				int retFlag = 0;
				unsigned long int dummyModelsCulled = 1;

				RunZCulling(camPos, retFlag, dummyModelsCulled);
			}
			catch (const std::exception&)
			{
				Assert::Fail();
			}

		}
		TEST_METHOD(UnitTest_BackCulling)
		{
			try
			{
				RunBackFaceCulling(testWindow);
			}
			catch (const std::exception&)
			{
				Assert::Fail();
			}
		}
		TEST_METHOD(UnitTest_FrustumCulling)
		{
			try
			{
				std::cout << "Running Unit Test" << std::endl;

				DummyModel testModel;	//the error is somewhere here - Credit: Joseph Walton-Rivers
				if (!testModel.isloaded())
					Assert::Fail();
				else
				{
					std::cout << "test model loaded" << testModel.isloaded() << std::endl;
				}

				BoundingBoxObjectClass testBoundingBox(testModel);

				Camera testCamera(glm::vec3(0.0f, 0.0f, 3.0f));
				Frustum testFrustum = CreateCameraBounds(testCamera, 1.0f, glm::radians(45.0f), 0.1f, 1000000.0f);

				Shader testShader("shader.vs", "shader.fs");

				unsigned int testDisplay = 0;
				unsigned int testTotal = 0;

				RunFrustumCulling(testBoundingBox, testFrustum, testShader, testDisplay, testTotal);

				std::cout << "Finished Unit Test" << std::endl;
			}
			catch (const std::exception& ex)
			{
				std::cerr << ex.what() << std::endl;
				Assert::Fail();
			}
		}
		TEST_METHOD(beep)
		{
			try
			{
				int total = TestFunction(10, 10);
				Assert::AreEqual(20, total);
			}
			catch (const std::exception&)
			{
				Assert::Fail();
			}

		}
	};
}
