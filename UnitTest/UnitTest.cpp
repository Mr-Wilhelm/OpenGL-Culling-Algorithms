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

	TEST_CLASS(UnitTest)
	{
	public:	
		TEST_METHOD(UnitTest_ZCulling)
		{
			glm::vec4 camPos(0.0f, 0.0f, 1000.0f, 1.0f);
			int retFlag = 0;

			RunZCulling(camPos, retFlag);
		}
		TEST_METHOD(UnitTest_BackCulling)
		{
			RunBackFaceCulling(testWindow);
		}
		TEST_METHOD(UnitTest_FrustumCulling)
		{
			std::cout << "Running Unit Test" << std::endl;

			Model testModel("Sphere.fbx");

			std::cout << "Finished Unit Test" << std::endl;
		}
	};
}
