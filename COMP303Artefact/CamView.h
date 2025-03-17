#pragma once

#include <GLM/glm.hpp>
#include <list>
#include <array>
#include <memory>
#include <iostream>

#include "Camera.h"

class Transform
{
protected:

	glm::vec3 pos = { 0.0f, 0.0f, 0.0f };	//transform position
	glm::vec3 rot = { 0.0f, 0.0f, 0.0f };	//transform rotation (in degrees)
	glm::vec3 sca = { 1.0f, 1.0f, 1.0f };	//transform scale

	glm::mat4 globalSpaceMatrix = glm::mat4(1.0f);	//global space information

	bool dataChanged = true;	//flag for data mismatch

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

	void AssignModelMatrix()
	{
		globalSpaceMatrix = GetModelMatrix();
		dataChanged = false;
	}
	void AssignModelMatrix(const glm::mat4& parentModelMatrix)
	{
		globalSpaceMatrix = parentModelMatrix * GetModelMatrix();
		dataChanged = false;
	}

	void SetPos(const glm::vec3& newPos)
	{
		pos = newPos;
		dataChanged = true;
	}
	void SetRot(const glm::vec3& newRot)
	{
		rot = newRot;
		dataChanged = true;
	}
	void SetSca(const glm::vec3& newSca)
	{
		sca = newSca;
		dataChanged = true;
	}

	const glm::vec3& GetGlobalPos() const
	{
		return globalSpaceMatrix[3];
	}
	const glm::vec3& GetLocalPos() const
	{
		return pos;
	}
	const glm::vec3& GetLocalRot() const
	{
		return rot;
	}
	const glm::vec3& GetLocalSca() const
	{
		return sca;
	}
	const glm::vec3& GetGlobalSca() const
	{
		return { glm::length(GetRight()), glm::length(GetUp()), glm::length(GetBack()) };
	}
	const glm::mat4& ReturnModelMatrix() const
	{
		return globalSpaceMatrix;
	}

	glm::vec3 GetRight() const
	{
		return globalSpaceMatrix[0];
	}
	glm::vec3 GetUp() const
	{
		return globalSpaceMatrix[1];
	}
	glm::vec3 GetBack() const
	{
		return globalSpaceMatrix[2];
	}
	glm::vec3 GetFront() const
	{
		return -globalSpaceMatrix[2];
	}
	
	bool IsFlagged() const
	{
		return dataChanged;
	}
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

struct ObjectBound
{
	virtual bool IsInView(const Frustum& camView, const Transform& transform) const = 0;

	virtual bool IsInFrontOfPlane(const Plane& plane) const = 0;

	bool IsInView(const Frustum& camVolume) const	//return all faces of the view frustum
	{
		return (IsInFrontOfPlane(camVolume.leftFace) &&
			IsInFrontOfPlane(camVolume.rightFace) &&
			IsInFrontOfPlane(camVolume.topFace) &&
			IsInFrontOfPlane(camVolume.bottomFace) &&
			IsInFrontOfPlane(camVolume.nearFace) &&
			IsInFrontOfPlane(camVolume.farFace));
	};
};
struct Sphere : public ObjectBound	//inherits from the ObjectBound struct
{
	glm::vec3 center{ 0.0f, 0.0f, 0.0f };	//sphere origin
	float radius{ 0.0f };	//sphere radius

	//constructor, allows us to create sphere objects
	Sphere(const glm::vec3& sphereCenter, float sphereRadius) : ObjectBound{}, center{ sphereCenter }, radius{ sphereRadius }
	{

	}

	//const - function does not modify member variables of the struct, more efficient than changing the overall variable
	//final - function cannot be overridden by any derived classes
	
	//takes a plane as an input, and checks if anything is intersecting it
	bool IsInFrontOfPlane(const Plane& plane) const final	
	{
		return plane.DistanceToPlane(center) > -radius;	//return true if in front of the plane
	}

	//check if the sphere is within view
	bool IsInView(const Frustum& camView, const Transform& transform) const final
	{
		const glm::vec3 scale = transform.GetGlobalSca();	//global scale
		const glm::vec3 center{ transform.ReturnModelMatrix() * glm::vec4(center, 1.0f) };	//convert to a vec4 and apply model transformation
		const float scaleMax = std::max(std::max(scale.x, scale.y), scale.z);	//get maximum scaling factor

		Sphere sphere(center, radius * (scaleMax * 0.5f));	//create a new sphere object
		
		//return the result of the IsInFrontOfPlane function for each face
		return (sphere.IsInFrontOfPlane(camView.leftFace) &&
			sphere.IsInFrontOfPlane(camView.rightFace) &&
			sphere.IsInFrontOfPlane(camView.topFace) &&
			sphere.IsInFrontOfPlane(camView.bottomFace) &&
			sphere.IsInFrontOfPlane(camView.nearFace) &&
			sphere.IsInFrontOfPlane(camView.farFace));
	};
};
struct SquareBoundingBox : public ObjectBound
{
	glm::vec3 center{ 0.0f, 0.0f, 0.0f };	//center of the bounding box
	float extent{ 0.0f };	//half the size of the box (the extent)

	//constructor allows us to create these boxes
	SquareBoundingBox(const glm::vec3& boxCenter, float boxExtent) : ObjectBound{}, center{ boxCenter }, extent{ boxExtent }
	{

	}

	//check if the box is in front of a plane
	bool IsInFrontOfPlane(const Plane& plane) const final
	{
		//projection interval radius
		const float radius = extent * (std::abs(plane.normal.x) + std::abs(plane.normal.y) + std::abs(plane.normal.z));
		return -radius <= plane.DistanceToPlane(center);	//return a check of whether or not it is in front
	}

	bool IsInView(const Frustum& camView, const Transform& transform) const final
	{
		const glm::vec3 globalCenter{ transform.ReturnModelMatrix() * glm::vec4(center, 1.0f) };

		//getting the x y and z extents
		const glm::vec3 extentX = transform.GetRight() * extent;
		const glm::vec3 extentY = transform.GetUp() * extent;
		const glm::vec3 extentZ = transform.GetFront() * extent;

		//calculate projecting the dimensions of the view frustum
		const float projectionX =
			std::abs(glm::dot(glm::vec3{ 1.0f, 0.0f, 0.0f }, extentX)) +
			std::abs(glm::dot(glm::vec3{ 1.0f, 0.0f, 0.0f }, extentY)) +
			std::abs(glm::dot(glm::vec3{ 1.0f, 0.0f, 0.0f }, extentZ));

		const float projectionY =
			std::abs(glm::dot(glm::vec3{ 0.0f, 1.0f, 0.0f }, extentX)) +
			std::abs(glm::dot(glm::vec3{ 0.0f, 1.0f, 0.0f }, extentY)) +
			std::abs(glm::dot(glm::vec3{ 0.0f, 1.0f, 0.0f }, extentZ));

		const float projectionZ =
			std::abs(glm::dot(glm::vec3{ 0.0f, 0.0f, 1.0f }, extentX)) +
			std::abs(glm::dot(glm::vec3{ 0.0f, 0.0f, 1.0f }, extentY)) +
			std::abs(glm::dot(glm::vec3{ 0.0f, 0.0f, 1.0f }, extentZ));

		//create a new bounding box object
		const SquareBoundingBox boxObject(globalCenter, std::max(std::max(projectionX, projectionY), projectionZ));

		//return the value of the bounding boxes being in front of or behind things
		return (
			boxObject.IsInFrontOfPlane(camView.leftFace) &&
			boxObject.IsInFrontOfPlane(camView.rightFace) &&
			boxObject.IsInFrontOfPlane(camView.topFace) &&
			boxObject.IsInFrontOfPlane(camView.bottomFace) &&
			boxObject.IsInFrontOfPlane(camView.nearFace) &&
			boxObject.IsInFrontOfPlane(camView.farFace)
			);
	};
};
struct AABB : public ObjectBound	//AABB stands for Axis Aligned Bounding Box, info found here: https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
{
	glm::vec3 center{ 0.0f, 0.0f, 0.0f };	//center of the bounding box
	glm::vec3 extents{ 0.0f, 0.0f, 0.0f };	//extents of the bounding box

	//constructor
	AABB(const glm::vec3& minExtent, const glm::vec3& maxExtent) : ObjectBound{}, center{ (maxExtent + minExtent) * 0.5f }, extents{ maxExtent.x - center.x, maxExtent.y - center.y, maxExtent.z - center.z }
	{

	}

	//constructor using different variables
	AABB(const glm::vec3& boxCenter, float projectionX, float projectionY, float projectionZ) : ObjectBound{}, center{ boxCenter }, extents{ projectionX, projectionY, projectionZ }
	{

	}

	std::array<glm::vec3, 8> GetVertices() const
	{
		std::array<glm::vec3, 8> vertices;	//get an array for the vertices of the extents, populate and return
		vertices[0] = { center.x - extents.x, center.y - extents.y, center.z - extents.z };
		vertices[1] = { center.x + extents.x, center.y - extents.y, center.z - extents.z };
		vertices[2] = { center.x - extents.x, center.y + extents.y, center.z - extents.z };
		vertices[3] = { center.x + extents.x, center.y + extents.y, center.z - extents.z };
		vertices[4] = { center.x - extents.x, center.y - extents.y, center.z + extents.z };
		vertices[5] = { center.x + extents.x, center.y - extents.y, center.z + extents.z };
		vertices[6] = { center.x - extents.x, center.y + extents.y, center.z + extents.z };
		vertices[7] = { center.x + extents.x, center.y + extents.y, center.z + extents.z };
		return vertices;
	}

	//typical script to check if in front of a plane
	bool IsInFrontOfPlane(const Plane& plane) const final
	{
		const float radius = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) + extents.z * std::abs(plane.normal.z);
		return -radius <= plane.DistanceToPlane(center);
	}

	bool IsInView(const Frustum& camView, const Transform& transform) const final
	{
		const glm::vec3 globalCenter{ transform.ReturnModelMatrix() * glm::vec4(center, 1.0f) };	//global scale

		//getting the x y and z extents
		const glm::vec3 extentX = transform.GetRight() * extents.x;
		const glm::vec3 extentY = transform.GetUp() * extents.y;
		const glm::vec3 extentZ = transform.GetFront() * extents.z;

		//calculate projecting the dimensions of the view frustum
		const float projectionX =
			std::abs(glm::dot(glm::vec3{ 1.0f, 0.0f, 0.0f }, extentX)) +
			std::abs(glm::dot(glm::vec3{ 1.0f, 0.0f, 0.0f }, extentY)) +
			std::abs(glm::dot(glm::vec3{ 1.0f, 0.0f, 0.0f }, extentZ));

		const float projectionY =
			std::abs(glm::dot(glm::vec3{ 0.0f, 1.0f, 0.0f }, extentX)) +
			std::abs(glm::dot(glm::vec3{ 0.0f, 1.0f, 0.0f }, extentY)) +
			std::abs(glm::dot(glm::vec3{ 0.0f, 1.0f, 0.0f }, extentZ));

		const float projectionZ =
			std::abs(glm::dot(glm::vec3{ 0.0f, 0.0f, 1.0f }, extentX)) +
			std::abs(glm::dot(glm::vec3{ 0.0f, 0.0f, 1.0f }, extentY)) +
			std::abs(glm::dot(glm::vec3{ 0.0f, 0.0f, 1.0f }, extentZ));

		const AABB AABBObject(globalCenter, projectionX, projectionY, projectionZ);	//create an AABB object

		return (	//run the function on all the faces
			AABBObject.IsInFrontOfPlane(camView.leftFace) &&
			AABBObject.IsInFrontOfPlane(camView.rightFace) &&
			AABBObject.IsInFrontOfPlane(camView.topFace) &&
			AABBObject.IsInFrontOfPlane(camView.bottomFace) &&
			AABBObject.IsInFrontOfPlane(camView.nearFace) &&
			AABBObject.IsInFrontOfPlane(camView.farFace));
	};
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

AABB CreateAABB(const Model& model)	//take a model as in input, and create an AABB for it
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());	//ensure any vertex is smaller than this value
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());	//ensure any vertex is larger than this value

	for (auto&& mesh : model.meshes)	//iterate through all meshes
	{
		for (auto&& vertex : mesh.vertices)	//iterate through all vertices in the meshes
		{
			//update the minimum coordinates
			minAABB.x = std::min(minAABB.x, vertex.pos.x);
			minAABB.y = std::min(minAABB.y, vertex.pos.y);
			minAABB.z = std::min(minAABB.z, vertex.pos.z);

			//update the maximum coordinates
			maxAABB.x = std::max(maxAABB.x, vertex.pos.x);
			maxAABB.y = std::max(maxAABB.y, vertex.pos.y);
			maxAABB.z = std::max(maxAABB.z, vertex.pos.z);
		}
	}
	return AABB(minAABB, maxAABB);
}

Sphere CreateSphereBoundingVolume(const Model& model)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
	for (auto&& mesh : model.meshes)
	{
		for (auto&& vertex : mesh.vertices)
		{
			//update the minimum coordinates
			minAABB.x = std::min(minAABB.x, vertex.pos.x);
			minAABB.y = std::min(minAABB.y, vertex.pos.y);
			minAABB.z = std::min(minAABB.z, vertex.pos.z);

			//update the maximum coordinates
			maxAABB.x = std::max(maxAABB.x, vertex.pos.x);
			maxAABB.y = std::max(maxAABB.y, vertex.pos.y);
			maxAABB.z = std::max(maxAABB.z, vertex.pos.z);
		}
	}
	return Sphere((maxAABB + minAABB) * 0.5f, glm::length(minAABB - maxAABB));
}

class BoundingBoxObjectClass
{
public:

	//indices for the model - these correspond with the for loop used with the model initialisation
	int x;
	int y;
	int z;

	std::list<std::unique_ptr<BoundingBoxObjectClass>> children;
	BoundingBoxObjectClass* parent = nullptr;

	Transform transform;

	Model* pModel = nullptr;
	std::unique_ptr<AABB> boundingVolume;

	BoundingBoxObjectClass(Model& model) : pModel{ &model }
	{
		boundingVolume = std::make_unique<AABB>(CreateAABB(model));
	}

	AABB GetAABBGlobalPos()
	{
		//get global position
		const glm::vec3 globalCenter{ transform.ReturnModelMatrix() * glm::vec4(boundingVolume->center, 1.0f) };

		//getting the x y and z extents
		const glm::vec3 extentX = transform.GetRight() * boundingVolume->extents.x;
		const glm::vec3 extentY = transform.GetUp() * boundingVolume->extents.y;
		const glm::vec3 extentZ = transform.GetFront() * boundingVolume->extents.z;

		//getting the x y and z projection
		const float projectionX =
			std::abs(glm::dot(glm::vec3{ 1.0f, 0.0f, 0.0f }, extentX)) +
			std::abs(glm::dot(glm::vec3{ 1.0f, 0.0f, 0.0f }, extentY)) +
			std::abs(glm::dot(glm::vec3{ 1.0f, 0.0f, 0.0f }, extentZ));

		const float projectionY =
			std::abs(glm::dot(glm::vec3{ 0.0f, 1.0f, 0.0f }, extentX)) +
			std::abs(glm::dot(glm::vec3{ 0.0f, 1.0f, 0.0f }, extentY)) +
			std::abs(glm::dot(glm::vec3{ 0.0f, 1.0f, 0.0f }, extentZ));

		const float projectionZ =
			std::abs(glm::dot(glm::vec3{ 0.0f, 0.0f, 1.0f }, extentX)) +
			std::abs(glm::dot(glm::vec3{ 0.0f, 0.0f, 1.0f }, extentY)) +
			std::abs(glm::dot(glm::vec3{ 0.0f, 0.0f, 1.0f }, extentZ));

		//creating a AABB with those parameters
		return AABB(globalCenter, projectionX, projectionY, projectionZ);
	}

	//variadic templates, i dont fully understand them tbh - https://en.cppreference.com/w/cpp/language/pack
	//but this is adding a child, and argument input is the argument of any constructor that is created?
	template<typename... TArgs>
	void AddChild(TArgs&... args)
	{
		children.emplace_back(std::make_unique<BoundingBoxObjectClass>(args...));	//create a bounding box using argumnets passed
		children.back()->parent = this;	//parent the child object to this
	}
	
	//update self and child transform
	void UpdateSelfAndChild()
	{
		if (transform.IsFlagged())	//jcheck if flagged for an update
		{
			ForceUpdatePos();
			return;
		}

		for (auto&& child : children)
		{
			child->UpdateSelfAndChild();	//update all chldren position
		}
	}

	//force the update of the position
	void ForceUpdatePos()
	{
		if (parent)
		{
			transform.AssignModelMatrix(parent->transform.ReturnModelMatrix());	//set transform of the parents matrix
		}
		else
		{
			transform.AssignModelMatrix();	//directly assing matrix data
		}

		for (auto&& child : children)
		{
			child->ForceUpdatePos();
		}
	}

	void DrawSelfAndChild(const Frustum& frustum, Shader& ourShader, unsigned int& display, unsigned int& total)
	{
		if (boundingVolume->IsInView(frustum, transform))
		{
			ourShader.setMat4("model", transform.ReturnModelMatrix());
			pModel->Draw(ourShader);
			display++;
		}
		total++;

		for (auto& child : children)
		{
			child->DrawSelfAndChild(frustum, ourShader, display, total);
		}
	}

};
