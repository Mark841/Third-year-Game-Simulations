#pragma once
#include "Collider.h"
#include "../../Common/Vector2.h"
#include <string>
#include <vector>
#include <list>

namespace NCL {
	using namespace Maths;
	namespace CSC3222 {
		// CollisionVolume class, holds information about the Colliders for an object
		class CollisionVolume {
		public:
			// CollisionVolume constructor and destructor
			CollisionVolume();
			~CollisionVolume();

			// Method to add a new Collider
			void AddNewCollider(Collider* c)
			{
				colliders.emplace_back(c);
			}

			// Method to update the Colliders positions to the new position
			void UpdateColliders(Vector2 position)
			{
				for (auto c : colliders)
				{
					c->SetCentre(position);
				}
			}

			// Method to get a certain Collider
			Collider* GetCollider(int index)
			{
				return colliders[index];
			}

			// This method groups nearby terrain colliders together to avoid so many collision checks
			void GroupMapColliders();

			// Method to get all the Colliders in a CollisionVolume
			std::vector<Collider*> GetColliders()
			{
				return colliders;
			}

			// Method to get the amount of Colliders in a CollisionVolume
			int GetAmountOfColliders()
			{
				return colliders.size();
			}

		protected:
			// Variable to store all the Colliders for a CollisionVolume
			std::vector<Collider*> colliders;
		};
	}
}