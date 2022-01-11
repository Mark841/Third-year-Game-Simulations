#include "FruitWizardGame.h"
#include "SimObject.h"
#include "GameMap.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "GameSimsPhysics.h"
#include "PlayerCharacter.h"
#include "Fruit.h"
#include "Guard.h"
#include "Froggo.h"
#include "Pixie.h"
#include "PixieDust.h"
#include "Spell.h"

#include "../../Common/Window.h"
#include "../../Common/TextureLoader.h"

using namespace NCL;
using namespace CSC3222;

// Game constructor
FruitWizardGame::FruitWizardGame()	{
	// Initialise variables
	renderer	= new GameSimsRenderer();
	texManager	= new TextureManager();
	physics		= new GameSimsPhysics(renderer, this);
	SimObject::InitObjects(this, texManager);
	// Initialise the game
	InitialiseGame();
}

// Game deconstructor
FruitWizardGame::~FruitWizardGame()	{
	delete currentMap;
	delete texManager;
	delete renderer;
	delete physics;
}

// Games update method
void FruitWizardGame::Update(float dt) {
	// If any new objects were added since last update call
	for (auto i : newObjects) {
		// Add the new objects to the game
		gameObjects.emplace_back(i);
	}
	// Clear the new objects vector
	newObjects.clear();

	// Increase the game duration
	gameTime += dt;

	// Update the renderer and physics methods
	renderer->Update(dt);
	physics->Update(dt, drawColliders);
	
	// Draw the map
	currentMap->DrawMap(*renderer);

	// Randomises a seed
	srand((int)(gameTime * 1000.0f));

	// Set the magicCount counter to the amount of spells the player has left
	magicCount = player->GetSpellCount();

	// For all objects in the game
	for (auto i = gameObjects.begin(); i != gameObjects.end(); ) {
		(*i)->UpdateAnimFrame(dt);
		// If object has said its finished with
		if (!(*i)->UpdateObject(dt)) { 
			// Remove the collider of the object from the physics checcker
			physics->RemoveCollider((*i)->GetCollider());
			// Remove the sim object from the physics checcker
			physics->RemoveRigidBody(*i);
			// Delete the object
			delete (*i);
			i = gameObjects.erase(i);
		}
		// Else the object should stay in the game
		else {
			// Draw the object
			(*i)->DrawObject(*renderer);
			++i;
		}
	}	

	// If there are no fruits left spawn more
	if (activeFruitCount <= 0)
	{
		SpawnFruit();
	}

	// Set game info trackers at top of screen
	renderer->DrawString("Score:" + std::to_string(currentScore), 
		Vector2(32, 12), Vector4(1, 1, 1, 1), 100.0f);
	renderer->DrawString("Lives:" + std::to_string(lives), 
		Vector2(144, 12), Vector4(1, 0, 0, 1), 100.0f);
	renderer->DrawString("Magic:" + std::to_string(magicCount), 
		Vector2(256, 12), Vector4(1, 0, 0.5f, 1), 100.0f);
	renderer->DrawString("Dust:"  + std::to_string(dustCount), 
		Vector2(366, 12), Vector4(0.5f, 0.3f, 0.8f, 1), 100.0f);

	// Set additional information at bottom of the screen
	renderer->DrawString("GameTime:" + std::to_string((int) gameTime),
		Vector2(32, 317.5), Vector4(1, 1, 1, 1), 100.0f);
	renderer->DrawString("Draw Colliders: K",
		Vector2(320, 317.5), Vector4(1, 1, 1, 1), 100.0f);

	// Draw to screen
	renderer->Render();

	// If game time is an increment of 30 and no active dust already, then have chance to spawn pixie dust. The attempted variable is so it only goes in this if once and not for how many frames are in the 1 second interval
	if ((int)gameTime % 30 == 0 && activePixieDust == 0 && dustCount < 4 && attempted == 0.0f)
	{
		// Chance for spawning dust is 1 in 3
		if (rand() % 2 == 1)
		{
			PixieDust* pixieDust = new PixieDust();
			pixieDust->SetPosition(GenerateRandomSafePosition(2));
			pixieDust->UpdateCollider();
			AddNewObject(pixieDust);
			activePixieDust++;

			// Set the active dust position variable
			pixieDustPosition = pixieDust->GetPosition();
		}
	}
	// Increment attempt counter for spawning dust as don't wan't the above if to be triggered more than once in a time window
	attempted += dt;
	// If the attempted variable is greater than 1 (for 1 second)
	if (attempted > 1.0f)
	{
		// Reset attempted counter and just toggled variables
		attempted = 0.0f;
		justToggled = false;
	}

	// Toggle pixie mode when players dust count is 4 and ctrl is pressed
	if (dustCount == 4 && Window::GetKeyboard()->KeyDown(KeyboardKeys::CONTROL))
	{
		SpawnPixies();
		dustCount = 0;
	}
	// Toggle whether the colliders should be drawn or not when K key is pressed
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::K) && !justToggled)
	{
		// Toggle drawing colliders
		drawColliders = !drawColliders;
		justToggled = true;
	}
}

// Method to initialise the game
void FruitWizardGame::InitialiseGame() {
	delete currentMap;
	for (auto o : gameObjects) {
		delete o;
	}
	gameObjects.clear();

	currentMap = new GameMap("FruitWizardMap.txt", gameObjects, *texManager, physics);

	renderer->SetScreenProperties(16, currentMap->GetMapWidth(), currentMap->GetMapHeight());

	// Create a player object
	Vector2 startLocation = Vector2(50, 32);
	player = new PlayerCharacter(startLocation);
	player->SetPosition(startLocation);
	player->UpdateCollider();
	AddNewObject(player);

	// Create the initial 4 guards
	startLocation = Vector2(0, 0);
	Guard* guard1 = new Guard(startLocation);
	guard1->SetPosition(startLocation);
	guard1->UpdateCollider();
	guards.emplace_back(guard1);
	AddNewObject(guard1);

	Guard* guard2 = new Guard(startLocation);
	guard2->SetPosition(startLocation);
	guard2->UpdateCollider();
	guards.emplace_back(guard2);
	AddNewObject(guard2);

	Guard* guard3 = new Guard(startLocation);
	guard3->SetPosition(startLocation);
	guard3->UpdateCollider();
	guards.emplace_back(guard3);
	AddNewObject(guard3);

	Guard* guard4 = new Guard(startLocation);
	guard4->SetPosition(startLocation);
	guard4->UpdateCollider();
	guards.emplace_back(guard4);
	AddNewObject(guard4);
	
	// Distribute the guards randomly and spawn the starting fruit
	DistributeGuards();
	SpawnFruit();

	// Initialise game variables
	gameTime		= 0;
	currentScore	= 0;
	magicCount		= 0;
	dustCount		= 0;
	lives			= 3;

	drawColliders	= false;
	justToggled		= false;
	attempted		= 0.0f; 
	
	pixieDustPosition	= Vector2(-1, -1);
}

// Method to add a new SimObject into the game
void FruitWizardGame::AddNewObject(SimObject* object) {
	// Adds that new sim object to the game
	newObjects.emplace_back(object);
	physics->AddRigidBody(object);
	// If that new object has a collider, add it aswell
	if (object->GetCollider()) {
		physics->AddCollider(object->GetCollider());
	}
}

// Method to generate a random safe position for objects on top of platforms
Vector2 FruitWizardGame::GenerateRandomSafePosition(int objectType)
{
	// Get the map colliders to be able to get the standable platforms where stuff can spawn
	CollisionVolume* mapColliders = physics->AllMapColliders();
	std::vector<Collider*> mapPlatforms;

	// For all the colliders that make up the map
	for (int i = 0; i < mapColliders->GetAmountOfColliders(); i++)
	{
		// Get all the map tile colliders with width greater than 16 (1 tile wide), exclude ladders as they arent safe to spawn on
		if (mapColliders->GetCollider(i)->GetType() == ColliderType::Wall && mapColliders->GetCollider(i)->GetHorizontalDistFromCentreToEdge() > 16)
		{
			mapPlatforms.emplace_back(mapColliders->GetCollider(i));
		}
	}
	// Choose a random one out of them 
	Collider* chosenPlatform = mapPlatforms[rand() % mapPlatforms.size()];

	// Choose a random number between 0 and width of collider (X position), subtract 10 as the longest platform on the bottom can lead into a wall creating an unsafe spawn
	int xOffset = rand() % (int) chosenPlatform->GetHorizontalDistFromCentreToEdge() - 10;

	// Choose whether its left or right on that collider (X position)
	int xSpawn = rand() % 2 == 1 ? ((int) chosenPlatform->GetCentre().x - xOffset) : ((int) chosenPlatform->GetCentre().x + xOffset);

	// Get the centre of collider (Y position)
	int ySpawn = (int) chosenPlatform->GetCentre().y;


	// Need object type to take into account vertical offset
	// If object is fruit
	if (objectType == 0)
	{
		return Vector2((float) xSpawn, (float) ySpawn + 8 + 5);
	}

	// If object is a guard
	else if (objectType == 1)
	{
		return Vector2((float) xSpawn, (float) ySpawn + 8 + 12);
	}

	// If object is pixie dust
	else if (objectType == 2)
	{
		return Vector2((float) xSpawn, (float) ySpawn + 8 + 8);
	}

	// If object is pixie
	else if (objectType == 3)
	{
		return Vector2((float) xSpawn, (float) ySpawn + 8 + 24);
	}
	// Else spawn object in set position
	else
	{
		return Vector2(100, 100);
	}
}
// Method to check if the move would be a safe one to do (for the guard check if they'd walk off an edge)
bool FruitWizardGame::CheckSafeMove(Vector2 pos) 
{
	// Get all the colliders in the map
	CollisionVolume* mapColliders = physics->AllMapColliders();

	// For all the colliders
	for (int i = 0; i < mapColliders->GetAmountOfColliders(); i++)
	{
		// Get the closest map collider to the object (the one it is stood on)
		if (mapColliders->GetCollider(i)->GetType() == ColliderType::Wall && 
			mapColliders->GetCollider(i)->GetVerticalDistFromCentreToEdge() < 9  && 
			abs(pos.y - mapColliders->GetCollider(i)->GetCentre().y) < 25 && 
			abs(pos.x - mapColliders->GetCollider(i)->GetCentre().x) < mapColliders->GetCollider(i)->GetHorizontalDistFromCentreToEdge())
		{
			// If going to fall off that platform return false
			if (abs(pos.x - mapColliders->GetCollider(i)->GetCentre().x) > mapColliders->GetCollider(i)->GetHorizontalDistFromCentreToEdge() - 4)
			{
				return false;
			}
		}

		// If next to a wall return false
		if (mapColliders->GetCollider(i)->GetType() == ColliderType::Wall &&
			mapColliders->GetCollider(i)->GetVerticalDistFromCentreToEdge() > 8 &&
			abs(pos.x - mapColliders->GetCollider(i)->GetCentre().x) < 18)
		{
			return false;
		}
	}
	// Else it's a safe move so return true
	return true;
}

// Method to spawn 16 fruit on the map
void FruitWizardGame::SpawnFruit()
{
	// For loop to go round 16 times for 16 fruit
	for (int i = 0; i < 16; i++)
	{
		// Make a new fruit
		Fruit* fruit = new Fruit(GenerateRandomSafePosition(0));
		fruit->UpdateCollider();
		AddNewObject(fruit);
		fruits.emplace_back(fruit);
		activeFruitCount++;
	}
}
// Method to spawn the pixies when pixie mode is activated
void FruitWizardGame::SpawnPixies()
{
	for (int i = 0; i < 8; i++)
	{
		// Make a new pixie
		Pixie* pixie = new Pixie();
		pixie->SetPosition(GenerateRandomSafePosition(3));
		pixie->UpdateCollider();

		// Add a spring object to the pixie, this isn't used but could be as an alternative for moving towards the player
		pixie->SetLeadingSpringObject(player);

		AddNewObject(pixie);
		pixies.emplace_back(pixie);
	}
}

// Method to redistribute the fruit when the player loses a life
void FruitWizardGame::DistributeFruit()
{
	// For all remaining fruit
	for (auto f : fruits)
	{
		// Set a new safe position
		f->SetPosition(GenerateRandomSafePosition(0));
	}
}
// Method to redistribute guards when the player loses a life
void FruitWizardGame::DistributeGuards()
{
	// For all guards
	for (auto g : guards)
	{
		// Set new start position and then reset position
		g->SetNewStartPosition(GenerateRandomSafePosition(1));
		g->ResetPosition();
	}

}

// Method to get the players current position (for pixies, guards and froggos)
Vector2 FruitWizardGame::GetPlayerPosition()
{
	return player->GetPosition();
}

// Method to check what is colliding with what
bool FruitWizardGame::CheckCollisionObjects(CollisionPair* collisionInfo)
{
	//std::cout << collisionInfo->aCollider->GetTypeString() << " HITTING " << collisionInfo->bCollider->GetTypeString() << std::endl;

	/*
		If it is two objects that should react to each other:
			- if SPELL is hitting ANYTHING
			- if PLAYER is hitting GUARD
			- if PLAYER is hitting FRUIT

	*/
	
	// WALL and ANYTHING BUT PIXIE
	if (collisionInfo->aCollider->GetType() == ColliderType::Wall && !(collisionInfo->bCollider->GetType() == ColliderType::Pixie))
	{
		// If one of the objects is a spell
		if (collisionInfo->bCollider->GetType() == ColliderType::Spell)
		{
			// Call the has hit something method for the spell
			collisionInfo->b->HasHitSomething();
		}
		// Apply projection and impulse to the objects
		return true;
	}
	// ANYTHING BUT PIXIE and WALL
	if (!(collisionInfo->aCollider->GetType() == ColliderType::Pixie) && collisionInfo->bCollider->GetType() == ColliderType::Wall)
	{
		// If one of the objects is a spell
		if (collisionInfo->aCollider->GetType() == ColliderType::Spell)
		{
			// Call the has hit something method for the spell
			collisionInfo->a->HasHitSomething();
		}
		// Apply projection and impulse to the objects
		return true;
	}

	// LADDER and ANYTHING
	if (collisionInfo->aCollider->GetType() == ColliderType::Ladder)
	{
		// Set that the object is on a ladder
		collisionInfo->b->SetOnLadder(true);
	}
	// ANYTHING and LADDER
	if (collisionInfo->bCollider->GetType() == ColliderType::Ladder)
	{
		// Set that the object is on a ladder
		collisionInfo->a->SetOnLadder(true);
	}

	// SPELL and GUARD (UNSTUNNED)
	if (collisionInfo->aCollider->GetType() == ColliderType::Spell && collisionInfo->bCollider->GetType() == ColliderType::Guard && collisionInfo->b->IsStunned() == false)
	{
		// Call the has hit something method for the spell enough times for it to be destroyed
		collisionInfo->a->HasHitSomething();
		collisionInfo->a->HasHitSomething();
		collisionInfo->a->HasHitSomething();

		// Increase score and apply stun to the guard
		currentScore += 200;
		collisionInfo->b->Stun();

		// Knock back in correct direction
		if (collisionInfo->b->GetPosition().x > collisionInfo->a->GetPosition().x)
		{
			collisionInfo->b->AddImpulse(Vector2(5000, 0));
		}
		else
		{
			collisionInfo->b->AddImpulse(Vector2(-5000,0));
		}
		// Apply projection and impulse to the objects
		return true;
	}
	// GUARD (UNSTUNNED) and SPELL
	if (collisionInfo->aCollider->GetType() == ColliderType::Guard && collisionInfo->a->IsStunned() == false && collisionInfo->bCollider->GetType() == ColliderType::Spell)
	{
		// Call the has hit something method enough times for the spell enough times for it to be destroyed
		collisionInfo->b->HasHitSomething();
		collisionInfo->b->HasHitSomething();
		collisionInfo->b->HasHitSomething();

		// Increase score and apply stun to the guard
		currentScore += 200;
		collisionInfo->a->Stun();

		// Knock back in correct direction
		if (collisionInfo->a->GetPosition().x > collisionInfo->b->GetPosition().x)
		{
			collisionInfo->a->AddImpulse(Vector2(5000, 0));
		}
		else
		{
			collisionInfo->a->AddImpulse(Vector2(-5000, 0));
		}
		// Apply projection and impulse to the objects
		return true;
	}
	// SPELL and FROGGO (UNSTUNNED)
	if (collisionInfo->aCollider->GetType() == ColliderType::Spell && collisionInfo->bCollider->GetType() == ColliderType::Froggo && collisionInfo->b->IsStunned() == false)
	{
		// Call the has hit something method for the spell enough times for it to be destroyed
		collisionInfo->a->HasHitSomething();
		collisionInfo->a->HasHitSomething();
		collisionInfo->a->HasHitSomething();

		// Apply stun to the froggo
		collisionInfo->b->Stun();

		// Knock back in correct direction
		if (collisionInfo->b->GetPosition().x > collisionInfo->a->GetPosition().x)
		{
			collisionInfo->b->AddImpulse(Vector2(-5000, 0));
		}
		else
		{
			collisionInfo->b->AddImpulse(Vector2(5000, 0));
		}
		// Apply projection and impulse to the objects
		return true;
	}
	// FROGGO (UNSTUNNED) and SPELL
	if (collisionInfo->aCollider->GetType() == ColliderType::Froggo && collisionInfo->a->IsStunned() == false && collisionInfo->bCollider->GetType() == ColliderType::Spell)
	{
		// Call the has hit something method for the spell enough times for it to be destroyed
		collisionInfo->b->HasHitSomething();
		collisionInfo->b->HasHitSomething();
		collisionInfo->b->HasHitSomething();

		// Apply stun to the froggo
		collisionInfo->a->Stun();

		// Knock back in correct direction
		if (collisionInfo->a->GetPosition().x > collisionInfo->b->GetPosition().x)
		{
			collisionInfo->a->AddImpulse(Vector2(-5000, 0));
		}
		else
		{
			collisionInfo->a->AddImpulse(Vector2(5000, 0));
		}
		// Apply projection and impulse to the objects
		return true;
	}
	
	// FRUIT and FRUIT
	if (collisionInfo->aCollider->GetType() == ColliderType::Fruit && collisionInfo->bCollider->GetType() == ColliderType::Fruit)
	{
		// Apply projection and impulse to the objects to seperate them as dont want a big pile of fruit
		return true;
	}
	// PLAYER and FINAL FRUIT
	if (collisionInfo->aCollider->GetType() == ColliderType::Player && collisionInfo->bCollider->GetType() == ColliderType::Fruit && activeFruitCount == 1)
	{
		// Increase score with an EXTRA 5000 points
		currentScore += 5000;

		// Create a new guard and froggo
		Vector2 startLocation = GenerateRandomSafePosition(1);
		Guard* guard = new Guard(startLocation);
		guard->SetPosition(startLocation);
		guard->UpdateCollider();
		guards.emplace_back(guard);
		AddNewObject(guard);

		Froggo* froggo = new Froggo();
		froggo->SetPosition(Vector2(250, 290));
		froggo->UpdateCollider();
		froggos.emplace_back(froggo);
		AddNewObject(froggo);
	}
	// FINAL FRUIT and PLAYER
	if (collisionInfo->aCollider->GetType() == ColliderType::Fruit && collisionInfo->bCollider->GetType() == ColliderType::Player && activeFruitCount == 1)
	{
		// Increase score with an EXTRA 5000 points
		currentScore += 5000;

		// Create a new guard and froggo
		Vector2 startLocation = GenerateRandomSafePosition(1);
		Guard* guard = new Guard(startLocation);
		guard->SetPosition(startLocation);
		guard->UpdateCollider();
		guards.emplace_back(guard);
		AddNewObject(guard);

		Froggo* froggo = new Froggo();
		froggo->SetPosition(Vector2(250, 290));
		froggo->UpdateCollider();
		froggos.emplace_back(froggo);
		AddNewObject(froggo);
	}
	// PLAYER and ANY FRUIT (Will always go through this when player hits a fruit so only delete the fruit object here)
	if (collisionInfo->aCollider->GetType() == ColliderType::Player && collisionInfo->bCollider->GetType() == ColliderType::Fruit)
	{
		// Increment score
		currentScore += 1000;

		// Delete the fruit
		collisionInfo->b->HasHitSomething();
		fruits.erase(std::find(fruits.begin(), fruits.end(), collisionInfo->b));
		activeFruitCount--;
	}
	// ANY FRUIT and PLAYER (Will always go through this when player hits a fruit so only delete the
	if (collisionInfo->aCollider->GetType() == ColliderType::Fruit && collisionInfo->bCollider->GetType() == ColliderType::Player)
	{
		// Increment score
		currentScore += 1000;

		// Delete the fruit
		collisionInfo->a->HasHitSomething();
		fruits.erase(std::find(fruits.begin(), fruits.end(), collisionInfo->a));
		activeFruitCount--;
	}

	// PLAYER and GUARD (UNSTUNNED)
	if (collisionInfo->aCollider->GetType() == ColliderType::Player && collisionInfo->bCollider->GetType() == ColliderType::Guard && collisionInfo->b->IsStunned() == false)
	{
		// Reset the players position and take 1 damage
		collisionInfo->b->ResetPosition();
		lives--;

		// Redistribute guards and fruit
		DistributeGuards();
		DistributeFruit();
	}
	// GUARD (UNSTUNNED) and PLAYER
	if (collisionInfo->aCollider->GetType() == ColliderType::Guard && collisionInfo->bCollider->GetType() == ColliderType::Player && collisionInfo->a->IsStunned() == false)
	{
		// Reset the players position and take 1 damage
		collisionInfo->b->ResetPosition();
		lives--;

		// Redistribute guards and fruit
		DistributeGuards();
		DistributeFruit();
	}

	// PLAYER and FROGGO (UNSTUNNED)
	if (collisionInfo->aCollider->GetType() == ColliderType::Player && collisionInfo->bCollider->GetType() == ColliderType::Froggo && collisionInfo->b->IsStunned() == false)
	{
		// Reset the players position and take 1 damage
		collisionInfo->b->ResetPosition();
		lives--;

		// Redistribute guards and fruit
		DistributeGuards();
		DistributeFruit();
	}
	// FROGGO (UNSTUNNED) and PLAYER
	if (collisionInfo->aCollider->GetType() == ColliderType::Froggo && collisionInfo->bCollider->GetType() == ColliderType::Player && collisionInfo->a->IsStunned() == false)
	{
		// Reset the players position and take 1 damage
		collisionInfo->b->ResetPosition();
		lives--;

		// Redistribute guards and fruit
		DistributeGuards();
		DistributeFruit();
	}

	// PIXIE DUST and PLAYER
	if (collisionInfo->aCollider->GetType() == ColliderType::PixieDust && collisionInfo->bCollider->GetType() == ColliderType::Player)
	{
		// Add score to player
		currentScore += 500;

		// Set that the dust has hit something
		collisionInfo->a->HasHitSomething();
		activePixieDust--;
		dustCount++;

		// Set the dust position to not existing
		pixieDustPosition = Vector2(-1, -1);
	}
	// PLAYER and PIXIE DUST
	if (collisionInfo->aCollider->GetType() == ColliderType::Player && collisionInfo->bCollider->GetType() == ColliderType::PixieDust)
	{
		// Add score to player
		currentScore += 500;

		// Set that the dust has hit something
		collisionInfo->b->HasHitSomething();
		activePixieDust--;
		dustCount++;

		// Set the dust position to not existing
		pixieDustPosition = Vector2(-1, -1);
	}

	// PIXIE and PLAYER
	if (collisionInfo->aCollider->GetType() == ColliderType::Pixie && collisionInfo->bCollider->GetType() == ColliderType::Player)
	{
		// Add score to player
		currentScore += 1000;

		// Set that the pixie has hit something and destroy it
		collisionInfo->a->HasHitSomething();
		pixies.erase(std::find(pixies.begin(), pixies.end(), collisionInfo->a));

		// Give player another spell
		player->GiveSpell();
	}
	// PLAYER and PIXIE
	if (collisionInfo->aCollider->GetType() == ColliderType::Player && collisionInfo->bCollider->GetType() == ColliderType::Pixie)
	{
		// Add score to player
		currentScore += 1000;

		// Set that the pixie has hit something and destroy it
		collisionInfo->b->HasHitSomething();
		pixies.erase(std::find(pixies.begin(), pixies.end(), collisionInfo->b));

		// Give player another spell
		player->GiveSpell();
	}

	// If projection and impulse shouldn't be applied, return false
	return false;
}