#ifndef MESH_H
#define MESH_H

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
	Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, std::vector<unsigned int> indices);


private:

};

#endif MESH_H
