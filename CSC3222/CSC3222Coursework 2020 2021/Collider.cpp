#include "Collider.h"

using namespace NCL;
using namespace CSC3222;

// Colider constructor
Collider::Collider(Vector2 centre, ColliderShape shape, ColliderType type, ableToMove moveable)
{
	this->centre = centre;
	colliderShape = shape;
	colliderType = type;
	this->moveable = moveable;
}

// Collider destructor
Collider::~Collider()
{
}

// Method to get the object type of the collider in string format - used in collision output
std::string Collider::GetTypeString() const
{
	if (colliderType == ColliderType::Wall)
	{
		return "WALL";
	}
	if (colliderType == ColliderType::Ladder)
	{
		return "LADDER";
	}
	if (colliderType == ColliderType::Player)
	{
		return "PLAYER";
	}
	if (colliderType == ColliderType::Spell)
	{
		return "SPELL";
	}
	if (colliderType == ColliderType::Froggo)
	{
		return "FROGGO";
	}
	if (colliderType == ColliderType::Guard)
	{
		return "GUARD";
	}
	if (colliderType == ColliderType::Pixie)
	{
		return "PIXIE";
	}
	if (colliderType == ColliderType::Fruit)
	{
		return "FRUIT";
	}
	if (colliderType == ColliderType::PixieDust)
	{
		return "PIXIE DUST";
	}
}
