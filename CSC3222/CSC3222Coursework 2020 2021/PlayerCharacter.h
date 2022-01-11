#pragma once
#include "SimObject.h"
#include "Spell.h"
#include "FruitWizardGame.h"

namespace NCL::CSC3222 {
	// PlayerCharacter class - extends SimObject
	class PlayerCharacter : public SimObject	{
	public:
		// Player constructor and deconstructor
		PlayerCharacter(Vector2 startLocation);
		~PlayerCharacter();

		// Update method takes in the time between frames
		bool UpdateObject(float dt) override;

		// Override of the UpdateCollider method to take into account offset for the player
		void UpdateCollider()
		{
			collider->UpdateColliders(position - Vector2(0, 2));
		}

		// Method to add to the players spell amount
		void GiveSpell()
		{
			spellAmount++;
		}
		// Method to get the players spell amount
		int GetSpellCount()
		{
			return spellAmount;
		}

	protected:
		// Enum class and variable to identify the current state of the player
		enum class PlayerState {
			Left,
			Right,
			Attack,
			Fall,
			Die,
			Idle
		};

		PlayerState		currentAnimState;
		int pixieDustCollected;
		int spellAmount;
	};
}