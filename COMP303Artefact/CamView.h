#pragma once

#include <GLM/glm.hpp>
#include <list>
#include <array>
#include <memory>

#include "Camera.h"

class Transform
{
protected:

	glm::vec3 pos = { 0.0f, 0.0f, 0.0f };	//transform position
	glm::vec3 rot = { 0.0f, 0.0f, 0.0f };	//transform rotation (in degrees)
	glm::vec3 sca = { 1.0f, 1.0f, 1.0f };	//transform scale

	glm::mat4 globalSpaceMatrix = glm::mat4(1.0f);	//global space information

	bool badFlag = true;	//flag for data mismatch

protected:

	glm::mat4 GetModelMatrix()	//gets the position, rotation and scale of the model
	{
		//creating rotation matrices around each axis
		const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));	
		const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));	
		const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));	

		const glm::mat4 rotationMatrix = transformY * transformX * transformZ;	//combining all rotation axes to make a full rotation matrix

		return glm::translate(glm::mat4(1.0f), pos) * rotationMatrix * glm::scale(glm::mat4(1.0f), sca);	//return the position, rotation matrix, and scale
	}

public:


};

struct Plane
{
	//plane normal
	glm::vec3 normal = {0.0f, 1.0f, 0.0f};

	//distance from origin to nearest point on plane
	float distance = 0.0f;

	Plane() = default;	//just initialise the object and do nothing with it
	Plane(const glm::vec3& origin, const glm::vec3& norm) : normal(glm::normalize(norm)), distance(glm::dot(normal, origin))	//constructor with origin and norm
	{

	}

	float DistanceToPlane(const glm::vec3& point) const	//const means it will not modify the state of the object
	{
		return glm::dot(normal, point) - distance;
	}
};
struct Frustum
{
	//creating plane objects
	Plane topFace; Plane bottomFace;
	Plane rightFace; Plane leftFace;
	Plane farFace; Plane nearFace;
};
struct Volume
{
	virtual bool isOnFrustum(const Frustum& camVolume, const Transform& modelTransform)
};

Frustum CreateCameraBounds(const Camera& cam, float aspect, float fov, float near, float far)
{
#pragma region What this function does and why
	/*
	* ----- const float halfVert = far * tanf(fov * 0.5f); -----
	* The vertical size of the frustum can be derived from the FOV.
	* "fov * 0.5f" Calculates half of the field of view in the vertical direction, because total FOV is symmetric about the center.
	* tanf() gets the tangeant of an angle, which when multiplied by the far distance, gives half the height of the view frustum
	* 
	* ----- const float halfHori = halfVert * aspect; -----
	* calculates half of the horizontal side of the frustum, given the aspect ratio of the camera.
	* aspect ratio is width/height, the aspect ratio can be derived by multiplying the vertical side (halfVert) by the aspect ratio
	* This gives the half-width of the frustum at the far plane
	* 
	* ----- const glm::vec3 pointAtFarPlane = far * cam.Front; -----
	* This gets the point at the far plane, in the direction in front of the camera.
	*/
#pragma endregion

	Frustum frustum;
	const float halfVert = far * tanf(fov * 0.5f);	//half height
	const float halfHori = halfVert * aspect;	//half width
	const glm::vec3 pointAtFarPlane = far * cam.Front;	//distance from camera to far plane

	frustum.nearFace = { cam.Position + near * cam.Front, cam.Front };	//assigning the near face
	frustum.farFace = { cam.Position + pointAtFarPlane, -cam.Front };	//assigning the far face

	frustum.rightFace = { cam.Position, glm::cross(pointAtFarPlane - cam.Right * halfHori, cam.Up) };	//assigning the right face
	frustum.leftFace = { cam.Position, glm::cross(cam.Up, pointAtFarPlane + cam.Right * halfHori) };	//assigning the left face

	frustum.topFace = { cam.Position, glm::cross(cam.Right, pointAtFarPlane - cam.Up * halfVert) };	//assigning the top face
	frustum.bottomFace = { cam.Position, glm::cross(pointAtFarPlane + cam.Up * halfVert, cam.Right) };	//assigning the bottom face

	return frustum;
}