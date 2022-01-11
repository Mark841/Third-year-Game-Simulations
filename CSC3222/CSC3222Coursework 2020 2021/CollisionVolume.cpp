#include "CollisionVolume.h"
#include "CollisionHandler.h"
#include "AABB.h"
#include "../../Common/Vector2.h"

using namespace NCL;
using namespace CSC3222;

// CollisionVolume constructor
CollisionVolume::CollisionVolume()
{
}

// CollisionVolume destructor
CollisionVolume::~CollisionVolume()
{
}

// This method groups nearby terrain colliders together to avoid so many collision checks
void CollisionVolume::GroupMapColliders()
{
	// Initialise vectors to store the WALLS and the LADDERS
	std::vector<Collider*> walls;
	std::vector<Collider*> ladders;

	// Go through all colliders and seperate into 2 vectors for LADDERS and WALLS
	for (auto c : colliders)
	{
		if (c->GetType() == ColliderType::Wall)
		{
			walls.emplace_back(c);
		}
		else if (c->GetType() == ColliderType::Ladder)
		{
			ladders.emplace_back(c);
		}
	}

	// Initialise vectors to store the grouped WALLS and the grouped LADDERS
	std::vector<Collider*> groupedWalls;
	std::vector<Collider*> groupedLadders;

	// Initialise variables to be used in loops below
	Vector2 centre;
	bool adding = true;
	float tileSize = 16;

	// GROUP similar colliders
	// For all the walls
	for (std::size_t i = 0; i < walls.size(); i++)
	{
		// Check if wall has already been added into a group
		for (auto gw : groupedWalls)
		{
			if (CollisionHandler::Collide(walls[i], gw) >= 0) 
			{
				// If a wall has already been added change adding to false
				adding = false;
			}
		}

		// If a wall hasn't already been grouped
		if (adding)
		{
			// Set this wall objects centre as the LEFT or BOTTOM tile
			Vector2 bottomLeftCorner = walls[i]->GetCentre();
			// Vector2 to store the centre of final tile in the grouping
			Vector2 sameGroup = walls[i]->GetCentre();

			// Booleans to keep track of if a wall is going vertical or horizontal, shouldn't go both
			bool horizontal = false;
			bool vertical = false;

			// For all remaining walls
			for (std::size_t j = i + 1; j < walls.size(); j++)
			{
				// Check if the end wall is colliding with an existing group of walls, dont want a constant overlap of walls
				bool carryOn = true;
				for (auto gw : groupedWalls)
				{
					if (CollisionHandler::Collide(walls[j], gw) >= 0)
					{
						carryOn = false;
					}
				}
				// If the walls are in same horizontal axis and are next to each other and walls aren't already being added in vertical direction
				if (carryOn && sameGroup + Vector2(tileSize, 0) == walls[j]->GetCentre() && !vertical)
				{
					// Replace the final tile in groups centre coordinates
					sameGroup = walls[j]->GetCentre();
					// Set that the walls are being grouped in a horizontal direction
					horizontal = true;
				}
				// If the walls are in same vertical axis and are next to each other and walls aren't already being added in horizontal direction
				if (carryOn && sameGroup + Vector2(0, tileSize) == walls[j]->GetCentre() && !horizontal)
				{
					// Replace the final tile in groups centre coordinates
					sameGroup = walls[j]->GetCentre();
					// Set that the walls are being grouped in a vertical direction
					vertical = true;
				}
			}

			// Set the RIGHT or TOP walls centre as the final one in the group
			Vector2 topRightCorner = sameGroup;
			// Get the half width of the group
			float halfWidth = ((tileSize / 2) + (topRightCorner.x - bottomLeftCorner.x) + (tileSize / 2)) / 2;
			// Get the half height of the group
			float halfHeight = ((tileSize / 2) + (topRightCorner.y - bottomLeftCorner.y) + (tileSize / 2)) / 2;
			// Get the centre of the grouped tiles
			Vector2 centre = ((topRightCorner - bottomLeftCorner) / 2) + bottomLeftCorner;

			// Add the new grouped collider to the groupedWalls vector
			groupedWalls.emplace_back(new AABB(centre, halfWidth, halfHeight, ColliderType::Wall, ableToMove::STATIC));
		}

		// Change adding back to true for next run through
		adding = true;
	}

	for (std::size_t i = 0; i < ladders.size(); i++)
	{
		// Check if ladder has already been added into a group
		for (auto gl : groupedLadders)
		{
			if (CollisionHandler::Collide(ladders[i], gl) >= 0) 
			{
				// If a ladder has already been added change adding to false
				adding = false;
			}
		}

		// If a ladder hasn't already been grouped
		if (adding) 
		{
			// Set this ladder objects centre as the BOTTOM LEFT tile
			Vector2 bottomLeftCorner = ladders[i]->GetCentre();
			// Vector2 to store the centre of final tile in the grouping
			Vector2 sameGroup = ladders[i]->GetCentre();

			// For all remaining ladders
			for (std::size_t j = i + 1; j < ladders.size(); j++)
			{
				// If the ladders are next to each other (above or to the right) (because of way the tiles are in the vectore the x or y only ever goes up so don't need to check if its left or below)
				if (sameGroup + Vector2(0, tileSize) == ladders[j]->GetCentre() ||
					sameGroup + Vector2(tileSize, 0) == ladders[j]->GetCentre())
				{
					// Replace the final tile in groups centre coordinates
					sameGroup = ladders[j]->GetCentre();
				}
			}

			// Set the TOP RIGHT ladders centre as the final one in the group
			Vector2 topRightCorner = sameGroup;
			// Get the half width of the group
			float halfWidth = ((tileSize / 2) + (topRightCorner.x - bottomLeftCorner.x) + (tileSize / 2)) / 2;
			// Get the half height of the group
			float halfHeight = ((tileSize / 2) + (topRightCorner.y - bottomLeftCorner.y) + (tileSize / 2)) / 2;
			// Get the centre of the grouped tiles
			Vector2 centre = ((topRightCorner - bottomLeftCorner) / 2) + bottomLeftCorner;

			// Add the new grouped collider to the groupedLadders vector
			groupedLadders.emplace_back(new AABB(centre, halfWidth, halfHeight, ColliderType::Ladder, ableToMove::STATIC));
		}

		// Change adding back to true for next run through
		adding = true;
	}

	// CLEAR original vector array
	colliders.clear();

	// ADD new grouped colliders into the colliders vector
	for (auto w : groupedWalls)
	{
		colliders.emplace_back(w);
	}
	for (auto l : groupedLadders)
	{
		colliders.emplace_back(l);
	}
}