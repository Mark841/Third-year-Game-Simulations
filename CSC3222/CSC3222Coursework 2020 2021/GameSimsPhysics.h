#pragma once
#include <vector>
#include "GameSimsRenderer.h"
#include "../../Common/Vector2.h"
#include "CollisionPair.h"
#include "FruitWizardGame.h"

namespace NCL {
	namespace CSC3222 {
		class RigidBody;
		class CollisionVolume;

		// GameSimsPhysics class that handles all the physics elements
		class GameSimsPhysics	{
		public:
			// Physics constructor takes in a pointer to a renderer
			GameSimsPhysics(GameSimsRenderer* renderer, FruitWizardGame* game);
			~GameSimsPhysics();

			// Method to update the physics on objects every frame
			void Update(float dt, bool drawColliders);

			// Method to add a rigid body to the vector
			void AddRigidBody(RigidBody* b);
			// Method to remove a rigid body from the vector
			void RemoveRigidBody(RigidBody* b);

			// Method to add a collision volume to the vector
			void AddCollider(CollisionVolume* c);
			// Method to remove a collision volume from the vector
			void RemoveCollider(CollisionVolume* c);

			// Method to return all the colliders in the map
			CollisionVolume* AllMapColliders() 
			{
				return allColliders[0];
			}

		protected:
			// Method to calculate the acceleration and resulting velocity of an object in this frame
			void IntegrateAcceleration(float dt);
			// Method to apply the velocity to the position of an object in this frame
			void IntegrateVelocity(float dt);

			// Method to reset the forces of all objects stored in allBodies
			void ResetForces();
			// Method to draw the colliders of all objects stored in allColliders
			void DrawColliders();

			// Fills the allRemainingColliders vector with the Colliders to check for broad phase checking
			int FillAllRemainingColliders();
			// Removes an item from the allRemainingColliders vector
			void RemoveFromAllRemainingColliders(Collider* c);
			// Removes an item from the allRemainingBodies vector
			void RemoveFromAllRemainingBodies(RigidBody* b);

			// Method to sort the colliders for broad phase sweeping - basically an insertion sort
			void BroadPhaseSort();
			// Method of broad phase checking to go through all sorted colliders and see if there are potential collisions between them
			void BroadPhaseSweep();
			// Method of narrow phase checking to see if there has been any collisions
			void NarrowPhaseChecks();

			// Method to resolve a collision
			void CollisionResolution(CollisionPair* collisionInfo);

			// Method to apply projection (collision seperation) in collision resolution
			void Projection(CollisionPair* collisionInfo);
			// Method to apply impulse in collision resolution
			void Impulse(CollisionPair* collisionInfo);


			// Vectors to hold all rigidbodies and all collision volumes - a collision volume itself is a vector of colliders, therefore an object can have many colliders (like the map)
			std::vector<RigidBody*>			allBodies;
			std::vector<CollisionVolume*>	allColliders;

			// Vectors to hold data for the broad phase checking
			std::vector<Collider*>			sortAndSweepColliders;
			std::vector<RigidBody*>			sortAndSweepBodies;
			std::vector<Collider*>			allRemainingColliders;
			std::vector<RigidBody*>			allRemainingBodies;
			std::vector<Collider*>			potentialCollisionListColliders;
			std::vector<RigidBody*>			potentialCollisionListBodies;

			// Store the renderer pointer
			GameSimsRenderer* renderer;
			// Vector2 to store gravity
			Vector2 gravity;
			static FruitWizardGame*	game;
		};
	}
}

