#include "RigidBody.h"

using namespace NCL;
using namespace CSC3222;

// RigidBody constructor
RigidBody::RigidBody()	{
	// Initialise variables
	inverseMass = 1.0f;
	elasticity  = 0.5f; 
	maxVelocity = Vector2(1000, 1000);
	damping = 0.9f;
	collisions = 0;
	onLadder = false;
	affectedByGravity = false;
	stunned = false;
}

// RigidBody deconstructor
RigidBody::~RigidBody()	{

}