#include "Pixie.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "Sphere.h"

using namespace NCL;
using namespace CSC3222;

Vector4 pixieFrames[] = {
	Vector4(32 * 1,32 * 4, 32, 32),
	Vector4(32 * 2,32 * 4, 32, 32),
	Vector4(32 * 3,32 * 4, 32, 32),
	Vector4(32 * 4,32 * 4, 32, 32),
};

// Pixie constructor
Pixie::Pixie() : SimObject() {
	// Create a collider for the pixie
	collider->AddNewCollider(new Sphere(position, 8.0f, ColliderType::Pixie, ableToMove::DYNAMIC));
	// Get texture and animation info for the Guard
	texture			= texManager->GetTexture("FruitWizard\\super_random_sprites.png");
	animFrameCount	= 4;
	// Initialise variables for the Pixie
	maxVelocity = Vector2(25, 25);
	// Set that the Pixie isn't affected by gravity
	affectedByGravity = false;

	// Set the threshold variables
	alignmentThreshold = 100.0f;
	seperationThreshold = 25.0f;
	cohesionThreshold = 50.0f;
	avoidanceThreshold = 40.0f;
}

// Pixie destructor
Pixie::~Pixie() {
}

// Pixie update method
bool Pixie::UpdateObject(float dt) {
	// Change to the current animation
	animFrameData = pixieFrames[currentanimFrame];

	// Get the enemies and pixies in the game
	enemies = game->GetEnemies();
	pixies = game->GetPixies();

	// Slowly head towards the player
	AddForce(game->GetPlayerPosition() - GetPosition());
	
	// Spring to player makes them travel to them too quickly so dont do that, instead do the above
	//SpringCalculations();

	// Perform flocking calculations
	Flocking(dt);

	// Update the collider for the Pixie
	UpdateCollider();

	// Clear the enemies and pixies vector ready for next update
	enemies.clear();
	pixies.clear();

	// If the pixie has collided with player, delete
	if (collisions > 0)
	{
		return false;
	}
	return true;
}

// Boid Alignment method to keep near boids in same direction
Vector2 Pixie::Alignment()
{
	// Initialise direction vector to its own direction
	Vector2 direction = GetVelocity();

	// For all pixies
	for (auto p : pixies)
	{
		// If pixie is itself, ignore
		if (p == this)
		{
			continue;
		}

		// Get the distance between the pixies
		float distance = Vector2(GetPosition() - p->GetPosition()).Length();

		// If the distance is more than the threshold, ignore
		if (distance > alignmentThreshold)
		{
			continue;
		}
		// Add the pixies direction to the total direction
		direction += p->GetVelocity();
	}

	// Return the normalised total direction of pixies
	return direction.Normalised();
}
// Boid Seperation method to keep boids at least a certain distance from each other
Vector2 Pixie::Seperation()
{
	// Initialise direction vector
	Vector2 direction = Vector2(0, 0);

	// For all pixies
	for (auto p : pixies)
	{
		// If pixie is itself, ignore
		if (p == this)
		{
			continue;
		}

		// Get the distance between the pixies
		float distance = Vector2(GetPosition() - p->GetPosition()).Length();

		// If the distance is more than the threshold, ignore
		if (distance > seperationThreshold)
		{
			continue;
		}

		// Set the strength of the seperation based on distance between objects
		float strength = 1.0f - (distance / seperationThreshold);
		// Apply the seperation to the total direction vector
		direction += (this->GetPosition() - p->GetPosition()).Normalised() * strength;
	}

	// Return the combined seperation direction vector
	return direction.Normalised();
}
// Boid Cohesion method to keep boids near each other
Vector2 Pixie::Cohesion()
{
	Vector2 avgPosition = GetPosition();
	float count = 1;

	// For all pixies
	for (auto p : pixies)
	{
		// If pixie is itself, ignore
		if (p == this)
		{
			continue;
		}

		// Get the distance between the pixies
		float distance = Vector2(GetPosition() - p->GetPosition()).Length();

		// If the distance is more than the threshold, ignore
		if (distance > cohesionThreshold)
		{
			continue;
		}

		// Get the average position between pixies
		avgPosition += p->GetPosition();
		count++;
	}

	// Get the average position between pixies and use them to make the direction for the pixie
	avgPosition /= count;
	Vector2 direction = avgPosition - GetPosition();

	// Return the combined cohesive direction vector
	return direction.Normalised();
}
// Boid Avoidance method to keep boids away from enemies
Vector2 Pixie::Avoidance()
{
	// Initialise direction vector
	Vector2 direction = Vector2(0, 0);

	// For all enemies
	for (auto e : enemies)
	{
		// Get the distance between the pixie and enemy
		float distance = Vector2(GetPosition() - e->GetPosition()).Length();

		// If the distance is more than the threshold, ignore
		if (distance > avoidanceThreshold)
		{
			continue;
		}
		// Else, move the pixie away from the enemy
		direction += (GetPosition() - e->GetPosition()).Normalised();
	}
	// Return the combined avoidance direction vector
	return direction.Normalised();
}
// Boid Flocking method to apply all other boid methods and get a position for the Pixie
void Pixie::Flocking(float dt)
{
	Vector2 direction = Vector2(0,0);
	// Apply a weighting to each different directional component
	direction += Alignment() * 10;
	direction += Seperation() * 150;
	direction += Cohesion() * 5;
	direction += Avoidance() * 20;
	// Add force to the pixie and increase the direction to have a more noticeable effect
	AddForce(direction * 40);
}