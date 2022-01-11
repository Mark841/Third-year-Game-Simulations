#define _USE_MATH_DEFINES
#include "Sphere.h"
#include "../../Common/Vector2.h"

using namespace NCL;
using namespace CSC3222;

// Sphere constructor, calls the Collider superconstructor
Sphere::Sphere(Vector2 centre, const float radius, ColliderType type, ableToMove moveable) : Collider(centre, ColliderShape::Sphere, type, moveable)
{ // It sets the radius
	this->radius = radius;
}

// Sphere deconstructor
Sphere::~Sphere()
{

}