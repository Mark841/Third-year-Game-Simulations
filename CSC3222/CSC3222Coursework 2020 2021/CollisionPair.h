#pragma once
#include <vector>
#include "../../Common/Vector2.h"
#include"RigidBody.h"
#include "Collider.h"

namespace NCL {
	namespace CSC3222 {
		// CollisionPair class, stores information about a collision
		class CollisionPair
		{
		public:
			// Information about the 2 objects
			RigidBody* a = nullptr;
			RigidBody* b = nullptr;

			// Information about the 2 objects colliders
			Collider* aCollider = nullptr;
			Collider* bCollider = nullptr;


			// Information about the collision
			float penetrationDistance = 0.0f;
			Vector2 collisionNormal = Vector2(0, 0);

			// CollisionPair constructor
			CollisionPair()
			{
				a = nullptr;
				b = nullptr;
				aCollider = nullptr;
				bCollider = nullptr;
				penetrationDistance = 0.0f;
				collisionNormal = Vector2(0, 0);
			}

			// CollisionPair destructor
			~CollisionPair()
			{

			}
		};
	}
}