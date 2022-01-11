#include "AABB.h"

using namespace NCL;
using namespace CSC3222;

// AABB constructor, calls the Collider superconstructor
AABB::AABB(Vector2 centre, const float halfWidth, const float halfHeight, ColliderType type, ableToMove moveable) : Collider(centre, ColliderShape::AABB, type, moveable)
{ 
	// Sets the width and the height
	this->halfWidth = halfWidth;
	this->halfHeight = halfHeight;
}

// AABB deconstructor
AABB::~AABB() 
{

}