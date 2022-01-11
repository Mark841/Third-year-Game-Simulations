#include "Fruit.h"
#include "Sphere.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"

using namespace NCL;
using namespace CSC3222;

// Fruit constructor
Fruit::Fruit(Vector2 position) : SimObject() {
	// Set the position and make a collider for the fruit
	SetPosition(position);
	collider->AddNewCollider(new Sphere(position, 5.0f, ColliderType::Fruit, ableToMove::DYNAMIC));
	// Get the texture for the fruit
	texture = texManager->GetTexture("FruitWizard\\Fruit.png");
	// Get which fruit to spawn
	int fruitID = rand() % 16;
	// Set that the fruit should be affected by gravity
	affectedByGravity = true;
	//  Set the animation of the fruit
	animFrameData = Vector4((fruitID / 4) * 16.0f, (fruitID % 4) * 16.0f, 16.0f, 16.0f);
}

// Fruit deconstructor
Fruit::~Fruit() {
}

// Update method for a fruit
bool Fruit::UpdateObject(float dt) {
	// Update the collider position
	UpdateCollider();
	
	// If the fruit has been collected, delete it
	if (collisions > 0)
	{
		return false;
	}
	return true;
}