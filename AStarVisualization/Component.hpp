#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
enum class ComponentType {
	RENDER,
	RAYCAST,
	FOLLOWCURSOR
};

class CRender
{
public:
	sf::VertexArray vertexArray;
	sf::RenderStates states = sf::RenderStates::Default;
	CRender(const sf::VertexArray& vertexArray_) 
		: vertexArray(vertexArray_){}
	CRender(const sf::VertexArray& vertexArray_, const sf::RenderStates& states_)
		: vertexArray(vertexArray_), states(states_) {}
};

class CRayCast
{
public:
	size_t numRays = 10;
	float rayLength = 4000;
	bool toMouse = false;
	bool drawTriangle = false;
	bool drawRay = false;
	bool sweep = false;
	CRayCast(size_t numRays_, bool drawTriangle_, bool drawRay_, bool sweep_)
		: numRays(numRays_), toMouse(false), drawTriangle(drawTriangle_), drawRay(drawRay_), sweep(sweep_){}
	CRayCast() : toMouse(true) {}
};

class CFlollowCursor
{
public:
	bool follow = true;
	CFlollowCursor() = default;
	CFlollowCursor(bool follow_) : follow(follow_) {}
};

