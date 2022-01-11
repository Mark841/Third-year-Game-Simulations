#pragma once
#include "../../Common/Vector2.h"

namespace NCL {
	using namespace Maths;
	namespace CSC3222 {
		// RigidBody class - superclass of SimObject chain
		class RigidBody	{
			friend class GameSimsPhysics;
		public:
			// Constructor that takes no inputs
			RigidBody();
			~RigidBody();

			// Method to get the position of the RigidBody
			Vector2 GetPosition() const {
				return position;
			}

			// Method to set the position of the RigidBody
			void SetPosition(const Vector2& newPosition) {
				position = newPosition;
			}

			// Method to get the velocity of the RigidBody
			Vector2 GetVelocity() const {
				return velocity;
			}

			// Method to set the velocity of the RigidBody
			void SetVelocity(const Vector2& newVelocity) {
				velocity = newVelocity;
			}

			// Method to add a force to the RigidBody
			void AddForce(const Vector2& newForce) {
				force += newForce;
			}

			// Method to add an impulse to the RigidBody
			void AddImpulse(Vector2 impulse)
			{
				velocity += impulse * inverseMass;
			}

			// Method to decrement how many collisions an object has had from limit
			void HasHitSomething()
			{
				collisions++;
			}
			// Method to stun an object
			void Stun()
			{
				stunned = true;
			}
			// Method to check if stunned
			bool IsStunned()
			{
				return stunned;
			}

			// Method to set new start location of an object
			void SetNewStartPosition(Vector2 newPosition)
			{
				startPosition = newPosition;
			}
			// Method to reset the location of an object
			void ResetPosition()
			{
				position = startPosition;
			}

			// Virtual method to update an objects collider
			virtual void UpdateCollider() = 0;

			// Method to state if a RigidBody is on a ladder
			void SetOnLadder(bool onOrOff)
			{
				onLadder = onOrOff;
			}

		protected:
			// Variables to store information about RigidBody
			Vector2 position;
			Vector2 velocity;
			Vector2 maxVelocity;
			Vector2 force;

			Vector2 startPosition;

			float inverseMass;
			float elasticity;

			// Set a damping variable for each object and check this in physics class
			float damping;
			int collisions;
			bool onLadder;
			bool affectedByGravity;
			bool stunned;
		};
	}
}
