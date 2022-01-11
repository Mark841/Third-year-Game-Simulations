#pragma once
#include "SimObject.h"

namespace NCL::CSC3222 {
	// PixieDust class - extends the SimObject class
	class PixieDust :	public SimObject	{
	public:
		// Pixie dust constructor and deconstructor
		PixieDust();
		~PixieDust();

		// Update pixie dust method takes in the time between frames
		bool UpdateObject(float dt) override;

		// Override of the UpdateCollider method to take into account offset for the PixieDust
		void UpdateCollider()
		{
			collider->UpdateColliders(position - Vector2(0.5f, 0.5f));
		}

	protected:
	};
}
