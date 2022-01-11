#pragma once
#include "Collider.h"

namespace NCL {
	using namespace Maths;
	namespace CSC3222 {
		// AABB class that extends the Collider class
		class AABB : public Collider
		{
		protected:
			// Variables for the AABB
			float halfWidth;
			float halfHeight;

		public:
			// This is the constructor for a square, it needs the centre coordinates, a width, a height, a collider type (Player, Guard, etc) and whether the object is static or dynamic
			AABB(Vector2 centre, const float halfWidth, const float halfHeight, ColliderType type, ableToMove moveable);
			~AABB();

			// This is the abstract getHorizontalDistFromCentreToEdge method from the shape class, returns half the width
			float GetHorizontalDistFromCentreToEdge() const
			{
				return halfWidth;
			}
			// This is the abstract getVerticalDistFromCentreToEdge method from the shape class, returns half the height
			float GetVerticalDistFromCentreToEdge() const
			{
				return halfHeight;
			}
			// This method gets the width of the square
			float GetWidth() const
			{
				return halfWidth * 2;
			}
			// This method gets the height of the square
			float GetHeight() const
			{
				return halfHeight * 2;
			}

			// This method gets the top left corner x coordinate of the square
			float GetTopLeftX() const
			{
				return (centre.x - halfWidth);;
			}
			// This method gets the top left corner y coordinate of the square
			float GetTopLeftY() const
			{
				return (centre.y + halfHeight);
			}
			// This method gets the bottom right corner x coordinate of the square
			float GetBottomRightX() const
			{
				return (centre.x + halfWidth);
			}
			// This method gets the bottom right corner y coordinate of the square
			float GetBottomRightY() const
			{
				return (centre.y - halfHeight);
			}
		};
	}
}