#pragma once
#include "Collider.h"

namespace NCL {
	using namespace Maths;
	namespace CSC3222 {
		// Sphere class that extends the Collider superclass
		class Sphere : public Collider
		{
		protected:
			// Each circle has a radius
			float radius;

		public:
			// This is the constructor for a circle, it needs the centre coordinates, a radius, a type (so whether its for a Player, Wall, etc) and whether the object is static or dynamic
			Sphere(Vector2 centre, const float radius, ColliderType type, ableToMove moveable);
			~Sphere();

			// This is the abstract getHorizontalDistFromCentreToEdge method from the shape class, returns the radius
			float GetHorizontalDistFromCentreToEdge() const
			{
				return radius;
			}
			// This is the abstract getVerticalDistFromCentreToEdge method from the shape class, returns the radius
			float GetVerticalDistFromCentreToEdge() const
			{
				return radius;
			}
			// This method gets the radius of the circle
			float GetRadius() const
			{
				return radius;
			}
		};
	}
}