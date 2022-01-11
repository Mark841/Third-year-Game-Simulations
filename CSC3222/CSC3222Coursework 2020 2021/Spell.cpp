#include "Spell.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "Sphere.h"

#include "../../Common/Maths.h"

#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"

using namespace NCL;
using namespace CSC3222;

Vector4 activeFrames[] = {
	Vector4(512,384, 32, 32),
	Vector4(512,384, 32, 32),
	Vector4(512,384, 32, 32),
	Vector4(384,416, 32, 32),
	Vector4(384,416, 32, 32),
	Vector4(384,416, 32, 32),
};

Vector4 explodeFrames[] = {
	Vector4(512,384, 32, 32),
	Vector4(384,416, 32, 32),
	Vector4(416,416, 32, 32),
	Vector4(448,416, 32, 32),
	Vector4(480,416, 32, 32),
	Vector4(512,416, 32, 32),	
};

// Spell constructor
Spell::Spell(Vector2 position, Vector2 direction, float dt) : SimObject()	{
	// Sets spell position and adds a spell collider with offset to make it encompass the texture better
	SetPosition(position);
	collider->AddNewCollider(new Sphere(position - Vector2(0, 2), 7.5f, ColliderType::Spell, ableToMove::DYNAMIC));
	// Sets texture
	texture		= texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	// Initialises variables
	velocity	= direction;
	maxVelocity = Vector2(300, 50);
	animFrameCount = 6;
	lifeTime	= dt; 
	inverseMass = 2.0f;
	damping = 1.0f;
	affectedByGravity = false;
	// Add impulse in a randomised direction
	Vector2 xDirection = (direction * (float) (rand() % 150 + 25));
	Vector2 yDirection = Vector2(0, (float) (rand() % 50 - 25));
	AddImpulse(xDirection + yDirection);
}

// Spell deconstructor
Spell::~Spell()	{
}

// Method to draw the spell to screen
void Spell::DrawObject(GameSimsRenderer &r) {
	Vector4	texData = explodeFrames[currentanimFrame];
	Vector2 texPos	= Vector2(texData.x, texData.y);
	Vector2 texSize = Vector2(texData.z, texData.w);
	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position);
}

// MEthod to update the spell
bool Spell::UpdateObject(float dt) {
	animFrameData = explodeFrames[currentanimFrame];
	// Increase spell lifetime
	lifeTime += dt;
	// If the spells been alive for more than 4.5 seconds or has had more collisions than the set amount (3)
	if (lifeTime >= 4.5f || collisions > 3 || velocity == Vector2(0,0))
	{ // Destroy the spell
		return false;
	}
	// Update the colliders position
	UpdateCollider();
	return true;
}