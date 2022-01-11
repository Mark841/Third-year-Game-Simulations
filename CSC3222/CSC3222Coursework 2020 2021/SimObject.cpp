#include "SimObject.h"
#include "CollisionVolume.h"
#include "GameSimsRenderer.h"
#include "CollisionHandler.h"

using namespace NCL;
using namespace CSC3222;

TextureManager*  SimObject::texManager	= nullptr;
FruitWizardGame* SimObject::game		= nullptr;

// Constructor for SimObject
SimObject::SimObject()	{
	// Initialise variables
	texture		= nullptr;
	collider	= new CollisionVolume();

	animFrameCount		= 1;
	currentanimFrame	= 0;
	frameTime			= 0;
	frameRate			= 10;
	flipAnimFrame		= true; 
	
	attachedSpring = nullptr;
	// Set spring resting length between the 2 centres to be one tile width
	springLength = 32.0f;
	springSnappiness = 20.0f;
	springDampingConstant = 0.5f;
}

// Deconstructor for a SimObject
SimObject::~SimObject()	{
	delete collider;
}

// Method to initialise references to game and the texture manager
void SimObject::InitObjects(FruitWizardGame* game, TextureManager* texManager) {
	SimObject::game			= game;
	SimObject::texManager	= texManager;
}

// Method to update the objects animation
void SimObject::UpdateAnimFrame(float dt) {
	frameTime -= dt;

	if (frameTime < 0) {
		currentanimFrame = (currentanimFrame + 1) % animFrameCount;
		frameTime += 1.0f / frameRate;
	}
}

// Method to draw the object to screen
void SimObject::DrawObject(GameSimsRenderer& r) {
	Vector2 texPos		= Vector2(animFrameData.x, animFrameData.y);
	Vector2 texSize		= Vector2(animFrameData.z, animFrameData.w);
	Vector2 screenPos	= position;

	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position, flipAnimFrame);
}

// Method to do calculations for a spring - SHOULD CHANGE TO FOLLOWING OBJECT SO CAN THEN GO DOWN THE POINTERS TO FINAL OBJECT AND MAKE A CHAIN FROM THEM
void SimObject::SpringCalculations()
{
	// If there is an attached spring
	if (attachedSpring != nullptr)
	{
		// Get the distance from resting distance
		float x = (position - attachedSpring->position).Length() - springLength;

		// Make a new collision pair between the two objects to get information like collision normal
		springInfo = new CollisionPair();
		springInfo->a = attachedSpring;
		springInfo->b = this;
		springInfo->aCollider = attachedSpring->GetCollider()->GetCollider(0);
		springInfo->bCollider = collider->GetCollider(0);
		springInfo->penetrationDistance = CollisionHandler::Collide(springInfo->aCollider, springInfo->bCollider, springInfo);

		// Get the relative velocity between the spring normal and the object
		Vector2 relativeVelocity = attachedSpring->GetVelocity() - velocity;
		// Find out the force to be applied in that direction
		float force = (-(springSnappiness * x)) - (springDampingConstant * Vector2::Dot(relativeVelocity, springInfo->collisionNormal));
		// Apply the force to the object in the direction
		AddForce(springInfo->collisionNormal * force);
	}
}