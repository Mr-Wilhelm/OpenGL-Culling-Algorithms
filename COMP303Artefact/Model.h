#include <glad/glad.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include <stb/stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>


class Model
{
public:

	Model(char* path)
	{
		SpawnModel(path);
	}
	void Draw(Shader& shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)	//iterate through all meshes
		{
			meshes[i].Draw(shader);	//Call the draw function of each mesh
		}
	}

private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> texturesToLoad(aiMaterial* mat, aiTextureType type, std::string typeName);

	void SpawnModel(std::string path)
	{
		Assimp::Importer import;
		const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "Assimp Error" << import.GetErrorString() << std::endl;
			return;
		}
		directory = path.substr(0, path.find_last_of("."));

		ProcessNode(scene->mRootNode, scene);

	}
	void ProcessNode(aiNode* node, const aiScene* scene);	//takes assimp nodes and scenes as parameters
	
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);



};
