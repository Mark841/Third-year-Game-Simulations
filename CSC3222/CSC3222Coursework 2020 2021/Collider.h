#pragma once
#include "../../Common/Vector2.h"

// Enum class to store what type of collider an object has
enum class ColliderShape {
	Sphere,
	AABB
};

// Enum class to store whether an object is static or dynamic
enum class ableToMove {
	STATIC,
	DYNAMIC
};

// Enum class to store what the colldier is for
enum class ColliderType {
	Background,
	Wall,
	Ladder,
	Player,
	Spell,
	Froggo,
	Guard,
	Pixie,
	Fruit,
	PixieDust,
	BroadPhaseCheck
};

namespace NCL {
	using namespace Maths;
	namespace CSC3222 {
		// Abstract Collider class, superclass for AABB and Sphere
		class Collider
		{
		public:

			// Colldier constructor, takes in centre of a collider, the type of collider it is and the object it is for
			Collider(Vector2 centre, ColliderShape shape, ColliderType type, ableToMove moveable);
			~Collider();

			// Virtual method to get the halfWidth of the collider
			virtual float GetHorizontalDistFromCentreToEdge() const = 0;

			// Virtual method to get the halfHeight of the collider
			virtual float GetVerticalDistFromCentreToEdge() const = 0;


			// Method to set the centre of the collider
			void SetCentre(Vector2 centre)
			{
				this->centre = centre;
			}

			// Method to get the centre of the collider
			Vector2 GetCentre() const
			{
				return centre;
			}
			// Method to get the shape type of the collider
			ColliderShape GetShape() const
			{
				return colliderShape;
			}
			// Method to get the object type of the collider
			ColliderType GetType() const
			{
				return colliderType;
			}
			// Method to get if the collider is static or dynamic in terms of movement
			ableToMove GetMoveable() const
			{
				return moveable;
			}

			// Method to get the object type of the collider in string format
			std::string GetTypeString() const;

		protected:
			// Variables to hold information about the collider
			Vector2 centre;

			ColliderShape colliderShape;
			ColliderType colliderType;
			ableToMove moveable;
		};
	}
}