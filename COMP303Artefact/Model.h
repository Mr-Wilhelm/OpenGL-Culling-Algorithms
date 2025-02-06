//LearnOpenGL's Model Class - https://learnopengl.com/Model-Loading/Model

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

unsigned int GetTextureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model
{
public:

	Model(std::string const &path)
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
	std::vector<Texture> loadedTextures;
	std::vector<Mesh> meshes;
	std::string directory;
	

	void SpawnModel(std::string path)
	{
		Assimp::Importer import;
		const aiScene *scene = import.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "Assimp Error In Model.h Line 53 " << import.GetErrorString() << std::endl;
			return;
		}
		directory = path.substr(0, path.find_last_of("/"));

		ProcessNode(scene->mRootNode, scene);

	}
	void ProcessNode(aiNode* node, const aiScene* scene)	//takes assimp nodes and scenes as parameters
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)	//iterate through all of the nodes meshes
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];	//assign them to a variable
			meshes.push_back(ProcessMesh(mesh, scene));	//run ProcessMesh function for each.
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}
	
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene)	//fills vertex, index and texture vectors and returns them as a mesh object
	{
		std::vector<Vertex> vertices;
		std::vector<Texture> textures;
		std::vector<unsigned int> indices;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			glm::vec3 vectorCoords;
			vectorCoords.x = mesh->mVertices[i].x;	//setting vertex positions to a reference of the mesh vertices
			vectorCoords.y = mesh->mVertices[i].y;
			vectorCoords.z = mesh->mVertices[i].z;
			vertex.pos = vectorCoords;	//pos from the vertex struct declared in Mesh.h

			if (mesh->HasNormals())
			{
				vectorCoords.x = mesh->mNormals[i].x;	//setting normal positions to a reference of the mesh normals
				vectorCoords.y = mesh->mNormals[i].y;
				vectorCoords.z = mesh->mNormals[i].z;
				vertex.norm = vectorCoords;	//norm value from the vertex struct in Mesh.h
			}
			else
			{
				std::cout << "Mesh has no normals" << std::endl;
			}

			if (mesh->mTextureCoords[0])
			{
				glm::vec2 texCoords;	//the UV/texture coordinates
				texCoords.x = mesh->mTextureCoords[0][i].x;	//x
				texCoords.y = mesh->mTextureCoords[0][i].y;	//y
				vertex.texCoord = texCoords;	//texCoord value from the vertex struct in Mesh.h
			}
			else
			{
				vertex.texCoord = glm::vec2(0.0f, 0.0f);	//set texture coordinates to 0.0, 0.0
				std::cout << "No Texture Coordinates" << std::endl;
			}


			vertices.push_back(vertex);
		}

		for (unsigned int i = 0;i < mesh->mNumFaces; i++)	//iterate through all faces
		{
			aiFace face = mesh->mFaces[i];

			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);	//store the face indicies in the indices vector
			}
		}

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		//diffuse textures
		std::vector<Texture> diffuseMaps = TexturesToLoad(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		//specular textures
		std::vector<Texture> specularMaps = TexturesToLoad(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		//normal maps
		std::vector<Texture> normalMaps = TexturesToLoad(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		//height maps
		std::vector<Texture> heightMaps = TexturesToLoad(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		return Mesh(vertices, textures, indices);
	}

	std::vector<Texture> TexturesToLoad(aiMaterial* mat, aiTextureType type, std::string typeName)	//this is loadMaterialTextures in the example
	{
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < loadedTextures.size(); j++)
			{
				if (std::strcmp(loadedTextures[j].texPath.data(), str.C_Str()) == 0)
				{
					textures.push_back(loadedTextures[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				Texture texture;
				texture.id = GetTextureFromFile(str.C_Str(), this->directory, false);
				texture.texType = typeName;
				texture.texPath = str.C_Str();
				textures.push_back(texture);
				loadedTextures.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
			}
		}
		return textures;
	}

	//this function is from learnopengl
	unsigned int GetTextureFromFile(const char* path, const std::string& directory, bool gamma)	//gets texture from file path, taking the path, directory and gamma
	{
		std::string fileName = std::string(path);	//get the file name
		fileName = directory + "/" + fileName;	//set it with the directory

		unsigned int texID;	//texture ID
		glGenTextures(1, &texID);

		int width, height, nrComponents;	//nrComponents to do with the number of colours being added (RGBA)
		unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)	//just red
			{
				format = GL_RED;
			}
			else if (nrComponents == 3)	//RGB
			{
				format = GL_RGB;
			}
			else if(nrComponents == 4)	//RGBA (use if the texture has alpha transparency)
			{
				format = GL_RGBA;
			}

			glBindTexture(GL_TEXTURE_2D, texID);
			glTexImage2D(GL_TEXTURE, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	//setting the texture settings to clamp to border
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to find texture at path" << path << std::endl;
			stbi_image_free(data);
		}
		return texID;
	}
		
};
