#include "PixieDust.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "Sphere.h"

using namespace NCL;
using namespace CSC3222;

Vector4 animFrames[] = {
	Vector4(64 , 192, 32, 32),
	Vector4(96, 192, 32, 32),
	Vector4(128, 192, 32, 32),
	Vector4(160, 192, 32, 32)
};

// PixieDust constructor
PixieDust::PixieDust() : SimObject() {
	// Create a collider for the pixie dust
	collider->AddNewCollider(new Sphere(position - Vector2(0.5f, 0.5f), 9.5f, ColliderType::PixieDust, ableToMove::DYNAMIC));
	// Get texture and animation length for pixie dust
	texture = texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	animFrameCount = 4;
	// Set the dust to be affected by gravity
	affectedByGravity = true;
}

// PixieDust destructor
PixieDust::~PixieDust() {
}

// PixieDust update method
bool PixieDust::UpdateObject(float dt) {
	// Change to the current animation
	animFrameData = animFrames[currentanimFrame];

	// If the object has been collected, delete it
	if (collisions > 0)
	{
		return false;
	}

	// Update the colliders location
	UpdateCollider();
	return true;
}