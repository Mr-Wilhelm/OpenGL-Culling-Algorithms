#pragma once

#include <GLM/glm.hpp>
#include <list>
#include <array>
#include <memory>

#include "Camera.h"

struct Plane
{
	//plane normal
	glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);

	//distance from origin to nearest point on plane
	float distance = 0.0f;
};
struct Frustum
{
	//creating plane objects
	Plane topFace; Plane bottomFace;
	Plane rightFace; Plane leftFace;
	Plane farFace; Plane nearFace;
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
	const float halfVert = far * tanf(fov * 0.5f);
	const float halfHori = halfVert * aspect;
	const glm::vec3 pointAtFarPlane = far * cam.Front;

	//TODO:: the rest of thsi function
}