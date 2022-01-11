#pragma once
#include "SimObject.h"
#include "FruitWizardGame.h"
#include "GameMap.h"

namespace NCL::CSC3222 {
	// Guard class - extends the SimObject class
	class Guard : public SimObject	{
	public:
		// Guard constructor and deconstructor
		Guard(Vector2 startLocation);
		~Guard();

		// Update guard method takes in the time between frames
		bool UpdateObject(float dt) override;

		// Override of the UpdateCollider method to take into account offset for the Guard
		void UpdateCollider()
		{
			collider->UpdateColliders(position - Vector2(0, 1));
		}

		// Method for the Guard to wander around
		void Wander();
		// Method for the Guard to chase the player
		void Chase();
		// Method for when the Guard gets stunned
		void Stunned(float dt);

		// Method to turn the guard round
		void Turn();

	protected:
		// Variables to keep track of guard decisions
		Vector2 force;
		Vector2 movementForce;
		float stunTime;
		float maxStunTime;
		char state;
		int lastTurned;
	};
}