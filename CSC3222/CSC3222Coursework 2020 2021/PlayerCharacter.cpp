#include "PlayerCharacter.h"
#include "TextureManager.h"
#include "Sphere.h"
#include "../../Common/Window.h"
#include "GameSimsRenderer.h"

using namespace NCL;
using namespace CSC3222;

Vector4 runFrames[] = {
	Vector4(64,160, 32, 32),
	Vector4(96,160, 32, 32),
	Vector4(128,160, 32, 32),
	Vector4(160,160, 32, 32),
	Vector4(192,160, 32, 32),
	Vector4(224,160, 32, 32),
};

Vector4 attackFrames[] = {
	Vector4(128,288, 32, 30),
	Vector4(64,224, 32, 32),
	Vector4(160,288, 32, 30),
	Vector4(96,224, 32, 32),
	Vector4(192,288, 32, 29),
	Vector4(64,256, 32, 32)
};

Vector4 idleFrames[] = {
	Vector4(64,128, 32, 32),
	Vector4(96,128, 32, 32),
	Vector4(128,128, 32, 32),
	Vector4(160,128, 32, 32),
	Vector4(128,128, 32, 32),
	Vector4(224,128, 32, 32)
};

Vector4 fallFrames[] = {
	Vector4(64,320, 32, 32),
	Vector4(64,320, 32, 32),
	Vector4(64,320, 32, 32),
	Vector4(96,320, 32, 32),
	Vector4(96,320, 32, 32),
	Vector4(96,320, 32, 32)
};

Vector4 deathFrames[] = {
	Vector4(96,352, 32, 32),
	Vector4(128,352, 32, 32),
	Vector4(96,352, 32, 32),	
	Vector4(128,352, 32, 32),
	Vector4(96,352, 32, 32),
	Vector4(128,352, 32, 32),
};

Vector4 ladderFrames[] = {//Not an ideal spritesheet for ladders!
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
};

// Player constructor
PlayerCharacter::PlayerCharacter(Vector2 startLocation) : SimObject() {
	// Add a collider of the player
	collider->AddNewCollider(new Sphere((position - Vector2(0, 2)), 13.0f, ColliderType::Player, ableToMove::DYNAMIC));
	currentAnimState	= PlayerState::Left;
	texture				= texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	animFrameCount		= 6;
	// Sets mass of the player
	inverseMass = 10.0f;
	// Set max velocity of the player
	maxVelocity = Vector2(500, 250); 
	// Set whether the player should be affected by gravity
	affectedByGravity = true;
	startPosition = startLocation;
	pixieDustCollected = 0;
	spellAmount = 4;
}

// Player deconstructor
PlayerCharacter::~PlayerCharacter() {

}

// Player update method
bool PlayerCharacter::UpdateObject(float dt) {
	float testSpeed = 64;
	Vector4* animSource = idleFrames;

	// If the player is attacking, play the animation
	if (currentAnimState == PlayerState::Attack) {
		animSource = attackFrames;
		if (currentanimFrame >= 5) {
			currentAnimState = PlayerState::Idle;
		}
	}
	// If the player is not currently attacking
	else {
		currentAnimState = PlayerState::Idle;
		// If the player is on a ladder and the up key is pressed
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP) && onLadder) {
			// Set what the player is doing and direction they're facing
			animSource = ladderFrames;
			currentAnimState = PlayerState::Idle;
			flipAnimFrame = false;
			// Add force for key being pressed
			AddForce(Vector2(0, 250));
			// Set that player is no longer on ladder
			SetOnLadder(false);
		}
		// If the player is on a ladder and the down key is pressed
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN) && onLadder) {
			// Set what the player is doing and direction they're facing
			animSource = fallFrames;
			currentAnimState = PlayerState::Fall;
			flipAnimFrame = false;
			// Add force for key being pressed
			AddForce(Vector2(0, -250));
			// Set that player is no longer on ladder
			SetOnLadder(false);
		}
		// If the left key is pressed
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			// Set what the player is doing and direction they're facing
			animSource = runFrames;
			currentAnimState = PlayerState::Left;
			flipAnimFrame = true;
			// Add force for key being pressed
			AddForce(Vector2(-200,0));
		}
		// If the right key is pressed
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			// Set what the player is doing and direction they're facing
			animSource = runFrames;
			currentAnimState = PlayerState::Right;
			flipAnimFrame = false;
			// Add force for key being pressed
			AddForce(Vector2(200, 0));
		}
		// If the space key is pressed
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && spellAmount > 0) {
			// Set what the player is doing
			currentAnimState = PlayerState::Attack;
			currentanimFrame = 0;

			Vector2 direction;
			Vector2 spawnLoc;

			if (flipAnimFrame)
			{ // Player is facing left
				direction = Vector2(-1, 0);
				// Set the spells to spawn at edge of player so dont instantly collide with them
				spawnLoc = GetPosition() - Vector2(16, 0);
			}
			else
			{ // Player is facing right
				direction = Vector2(1, 0);
				// Set the spells to spawn at edge of player so dont instantly collide with them
				spawnLoc = GetPosition() + Vector2(16, 0);
			}

			// Create a new spell object
			Spell* spell = new Spell(spawnLoc, direction, dt);
			game->AddNewObject(spell);
			spellAmount--;
		}
	}

	// Update the colliders position
	UpdateCollider();

	if (collisions > 0)
	{
		pixieDustCollected++;
		collisions--;
	}

	// play the frame for this animation
	animFrameData = animSource[currentanimFrame];
	return true;
}
