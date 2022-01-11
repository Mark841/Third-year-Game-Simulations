#pragma once
#include "SimObject.h"
#include "FruitWizardGame.h"
#include "GameMap.h"

namespace NCL::CSC3222 {
	// Froggo class - extends the SimObject class
	class Froggo : public SimObject {
	public:
		// Froggo constructor and deconstructor
		Froggo();
		~Froggo();

		// Update froggo method takes in the time between frames
		bool UpdateObject(float dt) override;

		// Override of the UpdateCollider method to take into account offset for the Froggo
		void UpdateCollider()
		{
			collider->UpdateColliders(position + Vector2(2, 2));
		}

		// Method for the Froggo to wander around
		void Guard();
		// Method for the Froggo to chase the player
		void Chase();
		// Method for the Froggo to attack the player
		void Attack();
		// Method for when the Froggo gets stunned
		void Stunned(float dt);

		// Method for the Froggo to move to next tile
		void Move(MapNode* nextTile);

	protected:
		// Variables to keep track of how long the object has been moving in a direction and the force to apply
		float movementTime;
		Vector2 force;
		Vector2 horizontalMovementForce;
		Vector2 verticalMovementForce;

		Vector2 playerPos;
		Vector2 pixieDustPos;

		float stunTime;
		float maxStunTime;
		char state;
		int lastTurned;
		GameMap* map;
	};
}