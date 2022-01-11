#include "GameSimsPhysics.h"
#include "RigidBody.h"
#include "CollisionVolume.h"
#include "CollisionHandler.h"
#include "../../Common/Vector2.h"

using namespace NCL;
using namespace CSC3222;

FruitWizardGame* GameSimsPhysics::game = nullptr;

// Physics constructor
GameSimsPhysics::GameSimsPhysics(GameSimsRenderer* renderer, FruitWizardGame* game)	{
	GameSimsPhysics::game = game;
	// Get a renderer so can draw colliders to the screen to view what's happening
	this->renderer = renderer;
	// Initialise the gravity variable
	gravity = Vector2(0, -9.8f);
}

// Physics destructor
GameSimsPhysics::~GameSimsPhysics()	{

}

// Method to update the physics on objects every frame
void GameSimsPhysics::Update(float dt, bool drawColliders) {
	IntegrateAcceleration(dt);
	// The broad phase calls handles the narrow phase and collision resolution aswell
	BroadPhaseSort();
	BroadPhaseSweep();
	IntegrateVelocity(dt);
	ResetForces();
	if (drawColliders)
	{
		DrawColliders();
	}
}

// Method to add a rigid body to the vector
void GameSimsPhysics::AddRigidBody(RigidBody* b) {
	allBodies.emplace_back(b);
}
// Method to remove a rigid body from the vector
void GameSimsPhysics::RemoveRigidBody(RigidBody* b) {
	auto at = std::find(allBodies.begin(), allBodies.end(), b);

	if (at != allBodies.end()) {
		//maybe delete too?
		allBodies.erase(at);
	}
}

// Method to add a collision volume to the vector
void GameSimsPhysics::AddCollider(CollisionVolume* c) {
	allColliders.emplace_back(c);
}
// Method to remove a collision volume from the vector
void GameSimsPhysics::RemoveCollider(CollisionVolume* c) {
	auto at = std::find(allColliders.begin(), allColliders.end(), c);

	if (at != allColliders.end()) {
		//maybe delete too?
		allColliders.erase(at);
	}
}

// Method to calculate the acceleration and resulting velocity of an object in this frame
void GameSimsPhysics::IntegrateAcceleration(float dt)
{
	// Integrate acceleration to get the velocity
	for (auto b : allBodies)
	{
		// Calculate acceleration
		Vector2 acceleration = b->force * b->inverseMass;
		b->velocity += acceleration * dt;

		// If the object should be affected by gravity, apply it
		if (b->affectedByGravity)
		{
			b->velocity += gravity;
		}

		// Apply an objects personal damping to its velocity
		b->velocity *= b->damping;
	}
}
// Method to apply the velocity to the position of an object in this frame
void GameSimsPhysics::IntegrateVelocity(float dt) 
{
	// Integrate velocity - to get position
	for (auto b : allBodies)
	{
		// Check if velocity is within the objects max horizontal velocity
		if (b->velocity.x > b->maxVelocity.x) 
		{ // in a positive direction
			b->velocity.x = b->maxVelocity.x;
		}
		else if (b->velocity.x < -(b->maxVelocity.x))
		{ // in a negative direction
			b->velocity.x = -(b->maxVelocity.x);
		}
		// Check if velocity is within the objects max vertical velocity
		if (b->velocity.y > b->maxVelocity.y) 
		{ // in a positive direction
			b->velocity.y = b->maxVelocity.y;
		}
		else if (b->velocity.y < -(b->maxVelocity.y))
		{ // in a negative direction
			b->velocity.y = -(b->maxVelocity.y);
		}

		// Set the position of the object
		b->position = b->position + b->velocity * dt;
	}
}

// Method to reset the forces of all objects stored in allBodies
void GameSimsPhysics::ResetForces()
{
	for (auto b : allBodies)
	{
		b->force = Vector2(0, 0);
	}
}
// Method to draw the colliders of all objects stored in allColliders
void GameSimsPhysics::DrawColliders()
{
	// For all Collision Volumes
	for (auto c : allColliders)
	{
		// For all Colliders in that Collision Volume
		for (int i = 0; i < c->GetAmountOfColliders(); i++)
		{
			if (c->GetCollider(i)->GetShape() == ColliderShape::Sphere)
			{ // If the collider is a Sphere
				renderer->DrawCircle(c->GetCollider(i)->GetCentre(), c->GetCollider(i)->GetHorizontalDistFromCentreToEdge(), Vector4(1, 0, 0, 1));
			}
			else
			{ // If the collider is a AABB
				renderer->DrawBox(c->GetCollider(i)->GetCentre(), Vector2(c->GetCollider(i)->GetHorizontalDistFromCentreToEdge(), c->GetCollider(i)->GetVerticalDistFromCentreToEdge()), Vector4(1, 0, 0, 1));
			}
		}
	}
}

// Fills the allRemainingColliders vector with the Colliders to check for broad phase checking
int GameSimsPhysics::FillAllRemainingColliders()
{
	// Clear vectors before filling them
	allRemainingColliders.clear();
	allRemainingBodies.clear();

	// Initialise a variable to keep track of how many colliders there are to check in broad phase
	int totalColliderAmount = 0;

	// For all Collision Volumes
	for (std::size_t x = 0; x < allColliders.size(); x++)
	{
		// For all Colliders in that Collision Volume
		for (int i = 0; i < allColliders[x]->GetAmountOfColliders(); i++)
		{
			allRemainingColliders.emplace_back(allColliders[x]->GetCollider(i));
			totalColliderAmount++;

			// If it is the first CollisionVolume then that means its the map colliders, the map doesn't have a rigidbody so set this to nullptr
			if (x == 0)
			{
				allRemainingBodies.emplace_back(nullptr);
			}
			else 
			{ // x-1 as the Collider volume index for an obect is one ahead of its matching body in the other vector because the first CollisionVolume is for the map Colliders
				allRemainingBodies.emplace_back(allBodies[x-1]);
			}
		}
	}
	// Return how many colliders there are to check
	return totalColliderAmount;
}
// Removes an item from the allRemainingColliders vector
void GameSimsPhysics::RemoveFromAllRemainingColliders(Collider* c)
{
	auto at = std::find(allRemainingColliders.begin(), allRemainingColliders.end(), c);

	if (at != allRemainingColliders.end()) {
		//maybe delete too?
		allRemainingColliders.erase(at);
	}
}
// Removes an item from the allRemainingBodies vector
void GameSimsPhysics::RemoveFromAllRemainingBodies(RigidBody* b)
{
	auto at = std::find(allRemainingBodies.begin(), allRemainingBodies.end(), b);

	if (at != allRemainingBodies.end()) {
		//maybe delete too?
		allRemainingBodies.erase(at);
	}
}

// Method to sort the colliders for broad phase sweeping - basically an insertion sort
void GameSimsPhysics::BroadPhaseSort()
{
	// Get the amount of colliders to loopp thorugh and also fill the vectors that are going to be used
	int totalAmountOfColliders = FillAllRemainingColliders();

	// For all the colliders to be checked
	for (int x = 0; x < totalAmountOfColliders; x++)
	{
		// set the furthest left collider to be the far right walls centre (the most right object) (allowed to do this as not going to use another map)
		Collider* furthestLeftCollider = allColliders[0]->GetCollider(2);
		RigidBody* furthestLeftBody = nullptr;

		// For all the remaining colliders to be checked
		for (std::size_t i = 0; i < allRemainingColliders.size(); i++)
		{ 
			// Checks if the objects furthest left part is more left than the current furthest left collider
			if ((allRemainingColliders[i]->GetCentre().x - allRemainingColliders[i]->GetHorizontalDistFromCentreToEdge()) < (furthestLeftCollider->GetCentre().x - furthestLeftCollider->GetHorizontalDistFromCentreToEdge()))
			{
				furthestLeftCollider = allRemainingColliders[i];
				furthestLeftBody = allRemainingBodies[i];
			}
		}
		// Add the now sorted by x axis collider and its body to the sort and sweep vector
		sortAndSweepColliders.emplace_back(furthestLeftCollider);
		sortAndSweepBodies.emplace_back(furthestLeftBody);
		// Remove the now sorted by x axis collider and its body from the remaining colliders and remaining bodies vectors so won't be found again as the most left
		RemoveFromAllRemainingColliders(furthestLeftCollider);
		RemoveFromAllRemainingBodies(furthestLeftBody);
	}
}
// Method of broad phase checking to go through all sorted colliders and see if there are potential collisions between them
void GameSimsPhysics::BroadPhaseSweep()
{
	// For all Colliders in the sort and sweep vector
	for (std::size_t i = 0; i < sortAndSweepColliders.size(); i++)
	{
		// Add this Collider to the potential collisions vector (index 0)
		potentialCollisionListColliders.emplace_back(sortAndSweepColliders[i]);
		potentialCollisionListBodies.emplace_back(sortAndSweepBodies[i]);

		// Go through all the next Colliders in the sort and sweep vector
		for (std::size_t j = i + 1; j < sortAndSweepColliders.size(); j++)
		{
			// Check if collisions are against 2 static objects, if they're not, continue
			if (!(sortAndSweepColliders[i]->GetMoveable() == ableToMove::STATIC && sortAndSweepColliders[j]->GetMoveable() == ableToMove::STATIC))
			{
				// If the 'j' Collider is further away than the 'i' Collider extends on x axis then break out the loop to iterate to next 'i' object as can't be a potential collision
				if ((sortAndSweepColliders[j]->GetCentre().x - sortAndSweepColliders[j]->GetHorizontalDistFromCentreToEdge()) > (sortAndSweepColliders[i]->GetCentre().x + sortAndSweepColliders[i]->GetHorizontalDistFromCentreToEdge()))
				{
					break;
				}
			
				// Check if objects collide with an AABB/AABB collision check
				if (CollisionHandler::Collide(new AABB(sortAndSweepColliders[i]->GetCentre(), sortAndSweepColliders[i]->GetHorizontalDistFromCentreToEdge(), sortAndSweepColliders[i]->GetVerticalDistFromCentreToEdge(), ColliderType::BroadPhaseCheck, ableToMove::DYNAMIC), new AABB(sortAndSweepColliders[j]->GetCentre(), sortAndSweepColliders[j]->GetHorizontalDistFromCentreToEdge(), sortAndSweepColliders[j]->GetVerticalDistFromCentreToEdge(), ColliderType::BroadPhaseCheck, ableToMove::DYNAMIC)))
				{
					// Add this Collider to the potential collisions list
					potentialCollisionListColliders.emplace_back(sortAndSweepColliders[j]);
					potentialCollisionListBodies.emplace_back(sortAndSweepBodies[j]);
				}
			}
		}
		// Perform narrow phase checks for this 'i' Collider before moving onto the next 'i' Collider
		NarrowPhaseChecks();
	}
	// Clear vector for next broad phase checks
	sortAndSweepColliders.clear();
	sortAndSweepBodies.clear();
}
// Method of narrow phase checking to see if there has been any collisions
void GameSimsPhysics::NarrowPhaseChecks()
{
	// For all potential collisions, starts at i = 1 as index 0 is the one that had a potential collision with all other Colliders in this vector
	for (std::size_t i = 1; i < potentialCollisionListColliders.size(); i++)
	{
		// Create a new CollisionPair to store information about a potential collision
		CollisionPair* collisionInfo = new CollisionPair();
		// Check if objects are colliding and store the penetration distance - always has A as collider with index 0 as that is the one that had a potential collision with all other Colliders in this vector
		collisionInfo->penetrationDistance = CollisionHandler::Collide(potentialCollisionListColliders[0], potentialCollisionListColliders[i], collisionInfo);

		// If the objects are colliding (penetration distance is over 0)
		if (collisionInfo->penetrationDistance >= 0)
		{
			// Set the bodies of the CollisionPair
			collisionInfo->a = potentialCollisionListBodies[0];
			collisionInfo->b = potentialCollisionListBodies[i];
			// Set the colliders of the CollisionPair
			collisionInfo->aCollider = potentialCollisionListColliders[0];
			collisionInfo->bCollider = potentialCollisionListColliders[i];

			// If b's RigidBody is nullptr then that means the object has no body (wall or ladder) or the Sphere is hitting the AABB, swap a and b round, only want those collisions one way
			if (collisionInfo->b == nullptr || (collisionInfo->aCollider->GetShape() == ColliderShape::Sphere && collisionInfo->bCollider->GetShape() == ColliderShape::AABB))
			{ 
				// Swap a and b round
				RigidBody* temp1 = collisionInfo->a;
				Collider* temp2 = collisionInfo->aCollider;

				collisionInfo->a = collisionInfo->b;
				collisionInfo->aCollider = collisionInfo->bCollider;
				collisionInfo->b = temp1;
				collisionInfo->bCollider = temp2;

				// Flip the collision normal
				collisionInfo->collisionNormal = -collisionInfo->collisionNormal;
			}
			// Perform the Collision Resolution for this pair
			CollisionResolution(collisionInfo);
		}
	}
	// Clear all potential collisions
	potentialCollisionListColliders.clear();
	potentialCollisionListBodies.clear();
}

// Method to resolve a collision
void GameSimsPhysics::CollisionResolution(CollisionPair* collisionInfo)
{
	// If the CheckCollisionObjects method returns true, apply projection and impulse
	if (game->CheckCollisionObjects(collisionInfo))
	{
		// Apply projection to the objects
		Projection(collisionInfo);
		// Apply impulse to the objects
		Impulse(collisionInfo);
	}
}
// Method to apply projection (collision seperation) in collision resolution
void GameSimsPhysics::Projection(CollisionPair* collisionInfo)
{
	// Get the data needed from the collision information
	RigidBody* aO = collisionInfo->a;
	RigidBody* bO = collisionInfo->b;

	Collider* aC = collisionInfo->aCollider;
	Collider* bC = collisionInfo->bCollider;

	// If As body is a nullptr then its against a wall or ladder
	if (aO == nullptr)
	{ // Don't want to move a static object so only apply for B
		bO->position += collisionInfo->collisionNormal * collisionInfo->penetrationDistance;
	}
	// Else two dynamic objects colliding
	else 
	{ // Calculate and apply projection (collision seperation) for both the objects
		float inverseMassTotal = aO->inverseMass + bO->inverseMass;
		aO->position -= collisionInfo->collisionNormal * collisionInfo->penetrationDistance * (aO->inverseMass / inverseMassTotal);
		bO->position += collisionInfo->collisionNormal * collisionInfo->penetrationDistance * (bO->inverseMass / inverseMassTotal);
	}
}
// Method to apply impulse in collision resolution
void GameSimsPhysics::Impulse(CollisionPair* collisionInfo)
{
	// Get the data needed from the collision information
	RigidBody* aO = collisionInfo->a;
	RigidBody* bO = collisionInfo->b;

	Collider* aC = collisionInfo->aCollider;
	Collider* bC = collisionInfo->bCollider;

	// Initialise variables to store information needed
	Vector2 relativeVelocity;
	float averageElasticity;
	float totalMass;

	// If As body is a nullptr then its against a wall or ladder
	if (aO == nullptr)
	{ // Don't want to move a static object so only apply for B
		relativeVelocity = bO->velocity;
		averageElasticity = bO->elasticity;
		totalMass = bO->inverseMass;
	}
	// Else two dynamic objects colliding
	else 
	{
		relativeVelocity = bO->velocity - aO->velocity;
		averageElasticity = (aO->elasticity + bO->elasticity) / 2.0f;
		totalMass = (aO->inverseMass + bO->inverseMass);
	}

	// Calculate the impulse force to be applied
	float J = ((-(1 + averageElasticity)) * (Vector2::Dot(relativeVelocity, collisionInfo->collisionNormal))) / totalMass;
	// Apply impulse for B
	bO->AddImpulse(collisionInfo->collisionNormal * J);

	// If A body is not a nullptr then apply impulse for it
	if (aO != nullptr)
	{
		aO->AddImpulse(-(collisionInfo->collisionNormal * J));
	}
}