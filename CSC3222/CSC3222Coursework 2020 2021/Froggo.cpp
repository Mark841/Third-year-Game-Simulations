#include "Froggo.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "AABB.h"

using namespace NCL;
using namespace CSC3222;

Vector4 froggoIdleFrames[] = {
	Vector4(0  ,(96 * 5) + 32, 80, 64),
	Vector4(80 ,(96 * 5) + 32, 80, 64),
	Vector4(160,(96 * 5) + 32, 80, 64),
	Vector4(240,(96 * 5) + 32, 80, 64),
	Vector4(320,(96 * 5) + 32, 80, 64),
	Vector4(400,(96 * 5) + 32, 80, 64),
};

Vector4 froggoWalkFrames[] = {
	Vector4(0  ,(96 * 5) + 32, 80, 64),
	Vector4(80 ,(96 * 5) + 32, 80, 64),
	Vector4(160,(96 * 5) + 32, 80, 64),
	Vector4(240,(96 * 5) + 32, 80, 64),
	Vector4(320,(96 * 5) + 32, 80, 64),
	Vector4(400,(96 * 5) + 32, 80, 64),
};

Vector4 froggoLadderFrames[] = {
	Vector4(0  ,(96 * 4) + 32, 80, 64),
	Vector4(80 ,(96 * 4) + 32, 80, 64),
	Vector4(160,(96 * 4) + 32, 80, 64),
	Vector4(240,(96 * 4) + 32, 80, 64),
	Vector4(320,(96 * 4) + 32, 80, 64),
	Vector4(400,(96 * 4) + 32, 80, 64),
};

Vector4 froggoStunFrames[] = {
	Vector4(0  ,(96 * 8) + 32, 80, 64),
	Vector4(80, (96 * 8) + 32, 80, 64),
	Vector4(160,(96 * 8) + 32, 80, 64),
	Vector4(240,(96 * 8) + 32, 80, 64),
	Vector4(320,(96 * 8) + 32, 80, 64),
	Vector4(400,(96 * 8) + 32, 80, 64),
};

Vector4 froggoAttackFrames[] = {
	Vector4(0  ,(96 * 1) + 32, 80, 64),
	Vector4(80 ,(96 * 1) + 32, 80, 64),
	Vector4(80 ,(96 * 1) + 32, 80, 64),
	Vector4(160,(96 * 1) + 32, 80, 64),
	Vector4(160,(96 * 1) + 32, 80, 64),
	Vector4(240,(96 * 1) + 32, 80, 64),
};

Vector4 froggoJumpFrames[] = {
	Vector4(0  ,(96 * 7) + 32, 80, 64),
	Vector4(80 ,(96 * 7) + 32, 80, 64),
	Vector4(80 ,(96 * 7) + 32, 80, 64),
	Vector4(160,(96 * 7) + 32, 80, 64),
	Vector4(160,(96 * 7) + 32, 80, 64),
	Vector4(240,(96 * 7) + 32, 80, 64),
};

Froggo::Froggo() : SimObject() {
	// Create a collider for the froggo
	collider->AddNewCollider(new AABB((position + Vector2(2, 2)), 12.0f, 16.0f, ColliderType::Froggo, ableToMove::DYNAMIC));
	// Get texture and animation info for the Guard
	texture			 = texManager->GetTexture("FruitWizard\\frogman_crown.png");
	animFrameCount	 = 6;
	// Initialise variables for the Froggo
	maxVelocity = Vector2(500, 500);
	movementTime = 0;
	inverseMass = 10.0f;
	horizontalMovementForce = Vector2(40, 0);
	verticalMovementForce = Vector2(0, 200);
	force = horizontalMovementForce;

	// Set that the Froggo is affected by gravity
	affectedByGravity = true;

	// Initialise stun and states of the froggo
	maxStunTime = 3.0f;
	state = 'C';
	lastTurned = 0;

	// Get access to the map methods (pathfinding)
	map = game->GetMap();
}

// Froggo destructor
Froggo::~Froggo() {
}

// Froggo update method
bool Froggo::UpdateObject(float dt) {
	// Change to the current animation
	animFrameData = froggoWalkFrames[currentanimFrame];
	
	// Get the player and pixie dusts positions
	playerPos = game->GetPlayerPosition();
	pixieDustPos = game->GetPixieDustPosition();

	// Check if the froggo has just been stunned
	if (stunned && state != 'S')
	{
		stunTime = 0.0f;
		state = 'S';
	}

	// Choose what to do based on the state of the froggo
	switch (state)
	{
	case 'G':
		Guard();
		break;
	case 'C':
		Chase();
		break;
	case 'A':
		Attack();
		break;
	case 'S':
		Stunned(dt);
		break;
	}

	// Apply force to the Froggo
	AddForce(force);
	SetOnLadder(false);

	// Update the collider for the Froggo
	UpdateCollider();
	return true;
}

// Method for the Guard to wander around
void Froggo::Guard()
{
	// Get path to dust
	std::vector<MapNode*> path = map->AlgorithmInitialisation(GetPosition(), pixieDustPos);
	// Get path to dust
	std::vector<MapNode*> playerPath = map->AlgorithmInitialisation(GetPosition(), playerPos);

	// If player within 2 tiles, ATTACK
	if (playerPath.size() != 0 && playerPath.size() <= 2)
	{
		state = 'A';
	}
	// If player > 2 tiles away AND no dust exists, CHASE
	if (playerPath.size() > 2 && pixieDustPos == Vector2(-1,-1))
	{
		state = 'C';
	}

	// If Froggo is on top of dust, wait
	if (path.size() == 0)
	{
		// Change to the idle animation
		animFrameData = froggoIdleFrames[currentanimFrame];
		// Change force applied to 0
		force = Vector2(0, 0);
	}
	// Else move towards the dust
	else
	{
		// Move towards dust
		MapNode* nextTile = path[path.size() - 1];
		Move(nextTile);
	}

}
// Method for the Guard to chase the player
void Froggo::Chase()
{
	// Get path to player
	std::vector<MapNode*> path = map->AlgorithmInitialisation(GetPosition(), playerPos);

	// If player within 2 tiles AND no dust exists, ATTACK
	if (path.size() != 0 && path.size() <= 2 && pixieDustPos == Vector2(-1, -1))
	{
		state = 'A';
	}
	// If player > 2 tiles away AND dust exists, GUARD
	if (path.size() != 0 && path.size() > 2 && pixieDustPos != Vector2(-1, -1))
	{
		state = 'G';
	}

	// Move towards player
	if (path.size() != 0)
	{
		MapNode* nextTile = path[path.size() - 1];
		Move(nextTile);
	}
	else
	{
		// Change to the idle animation
		animFrameData = froggoIdleFrames[currentanimFrame];
		force = Vector2(0, 0);
	}
}
// Method for the Froggo to chase the player
void Froggo::Attack()
{
	// Get path to player
	std::vector<MapNode*> path = map->AlgorithmInitialisation(GetPosition(), playerPos);

	// If player more than 2 tiles and no pixie dust, CHASE
	if (path.size() > 2 && pixieDustPos == Vector2(-1, -1))
	{
		state = 'C';
	}
	// If player more than 2 tiles away AND dust exists, GUARD
	if (path.size() > 2 && pixieDustPos != Vector2(-1, -1))
	{
		state = 'G';
	}

	// Move towards player
	if (path.size() != 0)
	{
		MapNode* nextTile = path[path.size() - 1];
		Move(nextTile);
	}
	// Change to the attack animation
	animFrameData = froggoAttackFrames[currentanimFrame];
}
// Method for when the Froggo gets stunned
void Froggo::Stunned(float dt)
{
	// Choose animation based on velocity, if stationary
	if ((int)GetVelocity().x == 0 && (int)GetVelocity().y < 15)
	{
		// Change to the final stun animation so froggo appears to be sleeping
		animFrameData = froggoStunFrames[5];
	}
	// Else froggo is still being knocked back or falling
	else
	{
		// Change to the stunned animation
		animFrameData = froggoStunFrames[currentanimFrame];
	}

	// If time > 3s awaken, CHASE
	if (stunTime >= maxStunTime)
	{
		state = 'C';
		stunned = false;
		return;
	}

	// Cant attack, cant collide (stunned variable is true) and dont move
	force = Vector2(0, 0);
	stunTime += dt;
}

// Method for the Froggo to move to next tile
void Froggo::Move(MapNode* nextTile)
{
	// If the frog should move up a ladder
	if (nextTile->worldPosY + 5 > GetPosition().y && onLadder)
	{
		// If frog slightly out of centre alignment on ladder (and not at top of ladder)
		if (nextTile->worldPosX + 5 > (int)GetPosition().x)
		{
			// Change to the walking animation
			animFrameData = froggoWalkFrames[currentanimFrame];
			force = horizontalMovementForce;
		}
		else
		{
			// Change to the ladder animation
			animFrameData = froggoLadderFrames[currentanimFrame];
			force = verticalMovementForce;
		}
		// If next tile is ground, give a boost to get over top edge of ladder, otherwise colliders as its a AABB and not a sphere will permanently get caught on it
		if (nextTile->cost == 1 && nextTile->tilePosX < map->WorldPosToTilePos(GetPosition()).x)
		{
			force += -horizontalMovementForce;
			force += verticalMovementForce;
		}
		// If next tile is ground, give a boost to get over top edge of ladder, otherwise colliders as its a AABB and not a sphere will permanently get caught on it
		else if (nextTile->cost == 1 && nextTile->tilePosX > map->WorldPosToTilePos(GetPosition()).x)
		{
			force += horizontalMovementForce;
			force += verticalMovementForce;
		}
	}
	// If the frog should move down a ladder
	else if (nextTile->tilePosY < map->WorldPosToTilePos(GetPosition()).y && onLadder)
	{
		// If frog slightly out of centre alignment on ladder
		if (nextTile->worldPosX + 5 > (int) GetPosition().x)
		{
			// Change to the walking animation
			animFrameData = froggoWalkFrames[currentanimFrame];
			force = horizontalMovementForce;
		}
		else
		{
			// Change to the ladder animation
			animFrameData = froggoLadderFrames[currentanimFrame];
			// Apply vertical speed so just doesn't drop from gravity
			force = verticalMovementForce * 0.5;
		}
	}
	// If the frog should move left
	else if (nextTile->tilePosX < map->WorldPosToTilePos(GetPosition()).x && nextTile->tilePosY == map->WorldPosToTilePos(GetPosition()).y)
	{
		// Change to the walking animation
		animFrameData = froggoWalkFrames[currentanimFrame];
		flipAnimFrame = false;
		force = -horizontalMovementForce;

		// If next tile is one it should jump across or the next tile is above the current one (top corner of a ladder)
		if (nextTile->cost > 2 || nextTile->worldPosY + 6 > (int)GetPosition().y)
		{
			force += verticalMovementForce * 0.72;
		}
	}
	// If the frog should move right
	else if (nextTile->tilePosX > map->WorldPosToTilePos(GetPosition()).x && nextTile->tilePosY == map->WorldPosToTilePos(GetPosition()).y)
	{
		// Change to the walking animation
		animFrameData = froggoWalkFrames[currentanimFrame];
		flipAnimFrame = true;
		force = horizontalMovementForce;

		// If next tile is one it should jump across or the next tile is above the current one (top corner of a ladder)
		if (nextTile->cost > 2 || nextTile->worldPosY + 6 > (int)GetPosition().y)
		{
			force += verticalMovementForce * 0.72;
		}
	}
}