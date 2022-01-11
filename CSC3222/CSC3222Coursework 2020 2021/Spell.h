#pragma once
#include "SimObject.h"
#include "FruitWizardGame.h"

namespace NCL {
	namespace Maths {
		class Vector2;
	}
	namespace CSC3222 {
		// Spell class, extends the SimObject class
		class Spell : public SimObject {
		public:
			// Spell constructor takes in a starting position, direction and the time increment from last frame
			Spell(Vector2 position, Vector2 direction, float dt);
			~Spell();

			// Method to draw the spell
			void DrawObject(GameSimsRenderer &r) override;
			// Method to update the spell
			bool UpdateObject(float dt) override;
			// Overriden method to ste teh spell collider position with correct offset for the texture
			void UpdateCollider()
			{
				collider->UpdateColliders(position - Vector2(0,2));
			}

		protected: 
			// Variable to store the life duration of the spell
			float lifeTime;
		};
	}
}

