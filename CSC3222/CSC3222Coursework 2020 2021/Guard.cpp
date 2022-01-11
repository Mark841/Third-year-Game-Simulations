#include "Guard.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "AABB.h"

using namespace NCL;
using namespace CSC3222;

Vector4 guardIdleFrames[] = {
	Vector4(384,32, 64, 32),
	Vector4(448,32, 64, 32),	
	Vector4(512,32, 64, 32),	
	Vector4(576,32, 64, 32),	
	Vector4(448,64, 64, 32),	
	Vector4(512,64, 64, 32),
};

Vector4 guardWalkFrames[] = {
	Vector4(384,96, 64, 32),
	Vector4(448,96, 64, 32),
	Vector4(512,96, 64, 32),
	Vector4(576,96, 64, 32),
	Vector4(640,96, 64, 32),
	Vector4(704,96, 64, 32),
};

Vector4 guardAttackFrames[] = {
	Vector4(384,128, 64, 32),
	Vector4(448,128, 64, 32),
	Vector4(512,128, 64, 32),
	Vector4(576,128, 64, 32),
	Vector4(640,128, 64, 32),
	Vector4(704,128, 64, 32),
};

Vector4 guardStunFrames[] = {
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
};

// Guard constructor
Guard::Guard(Vector2 startLocation) : SimObject() {
	// Create a collider for the guard
	collider->AddNewCollider(new AABB((position - Vector2(0, 1)), 10.0f, 14.0f, ColliderType::Guard, ableToMove::DYNAMIC));
	// Get texture and animation info for the Guard
	texture = texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	animFrameCount	= 6;
	// Initialise variables for the Guard
	maxVelocity = Vector2(400, 400);
	inverseMass = 2.0f;
	movementForce = Vector2(50, 0);
	force = movementForce;
	affectedByGravity = true;
	startPosition = startLocation;

	// Initialise stun and states of the guard
	maxStunTime = 5.0f;
	state = 'W';
	lastTurned = 0;
}

// Guard destructor
Guard::~Guard() {
}

// Guard update method
bool Guard::UpdateObject(float dt) {

	// Check if the guard has just been stunned
	if (stunned && state != 'S') 
	{
		stunTime = 0.0f;
		state = 'S';
	}

	// Choose what to do based on the state of the guard
	switch (state)
	{
	case 'W':
		Wander();
		break;
	case 'C':
		Chase();
		break;
	case 'S':
		Stunned(dt);
		break;
	}

	// Increment the lastTurned counter which is used to check the time bewteen when the guard last turned around
	lastTurned++;

	// Apply force to the Guard
	AddForce(force);
	// Update the collider for the Guard
	UpdateCollider();
	return true;
}

// Method for the Guard to wander around
void Guard::Wander()
{
	// Change to the walking animation
	animFrameData = guardWalkFrames[currentanimFrame];

	// Get path to player
	std::vector<MapNode*> pathToPlayer = game->GetMap()->AlgorithmInitialisation(GetPosition(), game->GetPlayerPosition());

	// If player within 3 tiles AND player on same y axis patform, CHASE
	if (pathToPlayer.size() != 0 && pathToPlayer.size() <= 3 && abs(GetPosition().y - game->GetPlayerPosition().y) <= 8)
	{
		state = 'C';
	}

	// Move 10 tiles each side from start point then turn OR if at edge turn
	if (GetPosition().x > startPosition.x + 160 && force.x > 0)
	{
		Turn();
	}
	else if (GetPosition().x < startPosition.x - 160 && force.x < 0)
	{
		Turn();
	}
	else if (!game->CheckSafeMove(GetPosition()) && lastTurned > 50)
	{
		Turn();
	}
}
// Method for the Guard to chase the player
void Guard::Chase()
{
	// Get path to player and path to start position
	std::vector<MapNode*> pathToPlayer = game->GetMap()->AlgorithmInitialisation(GetPosition(), game->GetPlayerPosition());
	std::vector<MapNode*> pathToStart = game->GetMap()->AlgorithmInitialisation(GetPosition(), startPosition);

	// If player > 3 tiles away on same platform level OR if > 10 tiles away from start point, WANDER
	if ((pathToPlayer.size() != 0 && pathToPlayer.size() > 3 && abs(GetPosition().y - game->GetPlayerPosition().y) <= 8) || pathToStart.size() > 10)
	{
		state = 'W';
	}

	// Move towards player, if direction is one way and player is the other turn
	if (game->GetPlayerPosition().x < GetPosition().x && force.x > 0)
	{
		Turn();
	}
	else if (game->GetPlayerPosition().x > GetPosition().x && force.x < 0)
	{
		Turn();
	}

	// Check if gonna fall off a ledge, if so turn
	if (!game->CheckSafeMove(GetPosition()))
	{
		Turn();
	}
	// If player within 1 tile, attack (set to 2 because of calculation from centre of player and guard not from edge of them)
	if (pathToPlayer.size() != 0 && pathToPlayer.size() <= 2 && abs(GetPosition().y - game->GetPlayerPosition().y) <= 8)
	{
		// Change to the attacking animation
		animFrameData = guardAttackFrames[currentanimFrame];
	}
	else
	{
		// Change to the walking animation
		animFrameData = guardWalkFrames[currentanimFrame];
	}
}

// Method for when the Guard gets stunned
void Guard::Stunned(float dt)
{
	// Choose animation based on velocity
	if ((int) GetVelocity().x == 0 && (int) GetVelocity().y < 15)
	{
		// Change to the idle animation
		animFrameData = guardIdleFrames[currentanimFrame];
	}
	else
	{
		// Change to the stunned animation
		animFrameData = guardStunFrames[currentanimFrame];
	}

	// If time > 5s, WANDER
	if (stunTime >= maxStunTime)
	{
		force = flipAnimFrame ? movementForce : -movementForce;
		state = 'W';
		stunned = false;
		return;
	}
	// If knocked off edge, set new start point (IF y axis different)
	if (GetPosition().y > startPosition.y + 8 || GetPosition().y < startPosition.y - 8)
	{
		startPosition = GetPosition();
	}

	// Cant attack, cant collide (stunned variable is true) and dont move
	force = Vector2(0, 0);
	stunTime += dt;
}

// Method to turn the guard round
void Guard::Turn()
{
	lastTurned = 0;
	force = -force;
	flipAnimFrame = !flipAnimFrame;
}