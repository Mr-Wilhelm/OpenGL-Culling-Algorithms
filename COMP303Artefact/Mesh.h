//LearnOpenGL's Mesh class - https://learnopengl.com/Model-Loading/Mesh

#include <glad/glad.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include "Shader.h"

#include<string>
#include<vector>

struct Vertex
{
	glm::vec3 pos;	//position

	glm::vec2 texCoord;	//texture coordinates

	glm::vec3 norm;	//normal - direction perpendicular to the tangeant (the y axis)
	glm::vec3 tang;	//tangeant - arbitrary direction (usually the x or z axis)
	glm::vec3 bitTang;	//bit tangeant (the other tangeant vector) (e.g. if the tangeant is the x axis, then this will be the z axis)
};

struct Texture
{
	unsigned int id;
	std::string texType;	//type of texture (e.g. diffuse, specular, albedo etc)
	std::string texPath;
};

class Mesh
{
public:
	//creating vectors (resizeable arrays) of vertices and textures
	//a vector is a resizeable array. More efficient than a list (O(n) complexity except for the last item) and thread safe (whatever that means)

	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	std::vector<unsigned int> indices;	//unsigned int - integer that can never be negative, good for ID numbers

	//function delcarations
	Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, std::vector<unsigned int> indices)
	{
		this->vertices = vertices;	//accessing the members of a struct using pointers. 
		this->textures = textures;	//getting the memory address rather than the variable itself is more efficient.
		this->indices = indices;

		CreateMesh();
	}
	void Draw(Shader& shader)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);	//activate the texture before binding it

			std::string number;
			std::string name = textures[i].texType;	//get the type of texture from the vector

			//possible optimization would be to change this to a switch case statement? But C++ doesn't allow switch on strings by default. 
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);

			shader.setInt(("material." + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);	//draw the mesh
		glBindVertexArray(0);
	}

private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	void CreateMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);	//bind the VAO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);	//bind the VBO

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);	//create a buffer object, with the length of the vertices vertex.

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);	//bind the EBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);	//create a buffer object with the length of the indices array.

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);	//setting vertex positions

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));	//setting vertex normals
	
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));	//setting vertex texture coordinates

		glBindVertexArray(0);
	}


};
