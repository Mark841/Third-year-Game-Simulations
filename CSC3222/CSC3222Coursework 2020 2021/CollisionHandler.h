#pragma once
#include "AABB.h"
#include "Sphere.h"
#include "GameSimsPhysics.h"

namespace NCL {
	using namespace Maths;
	namespace CSC3222 {
		// CollisionHandler class, handles the calculations to see if two Colliders are overlapping
		class CollisionHandler
		{
		public:
			// Method for colliding 2 shapes and passing on to the correct type methods
			static float Collide(Collider* s1, Collider* s2);

			// Method for colliding 2 shapes and passing on to the correct type methods, edits collision information
			static float Collide(Collider* s1, Collider* s2, CollisionPair* collisionInfo);

			// Method for colliding 2 AABBs
			static float CollideSquares(AABB* s1, AABB* s2);

			// Method for colliding 2 AABBs and editing the collision information
			static float CollideSquares(AABB* s1, AABB* s2, CollisionPair* collisionInfo);

			// Method for colliding a AABB and a Sphere
			static float CollideCircleAndSquare(AABB* s, Sphere* c);

			// Method for colliding a AABB and a Sphere
			static float CollideCircleAndSquare(AABB* s, Sphere* c, CollisionPair* collisionInfo);

			// Method for colliding 2 Spheres
			static float CollideCircles(Sphere* c1, Sphere* c2);
		};
	}
}