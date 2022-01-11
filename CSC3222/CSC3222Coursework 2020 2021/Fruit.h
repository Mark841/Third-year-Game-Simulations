#pragma once
#include "SimObject.h"
#include "FruitWizardGame.h"

namespace NCL::CSC3222 {
	// Fruit class - extends the SimObject class
	class Fruit : public SimObject	{
	public:
		// Fruit constructor takes in a position
		Fruit(Vector2 position);
		~Fruit();
		// Update fruit method takes in the time between frames
		bool UpdateObject(float dt) override;

	};
}