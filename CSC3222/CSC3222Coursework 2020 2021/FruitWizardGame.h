#pragma once
#include <vector>
#include "CollisionPair.h"

namespace NCL::CSC3222 {
	class GameMap;
	class SimObject;
	class GameSimsRenderer;
	class GameSimsPhysics;
	class TextureBase;
	class TextureManager;
	class PlayerCharacter;

	// FruitWizardGame class, contains information for the main running of the game
	class FruitWizardGame {
	public:
		GameSimsRenderer* renderer;

		// FruitWizardGame constructor and destructor
		FruitWizardGame();
		~FruitWizardGame();

		// Update method to happen every frame
		void Update(float dt);

		// Method to add a new object to the game space
		void AddNewObject(SimObject* object);

		// Method to generate a random safe position for objects on top of platforms
		Vector2 GenerateRandomSafePosition(int objectType);
		// Method to check if the move would be a safe one to do (for the guard check if they'd walk off an edge)
		bool CheckSafeMove(Vector2 pos);

		// Method to spawn 16 fruit on the map
		void SpawnFruit();
		// Method to spawn the pixies when pixie mode is activated
		void SpawnPixies();
		// Method to redistribute the fruit when the player loses a life
		void DistributeFruit();
		// Method to redistribute guards when the player loses a life
		void DistributeGuards();

		// Method to get the players current position (for pixies, guards and froggos)
		Vector2 GetPlayerPosition();
		// Method to get the pixie dust position (for the froggos)
		Vector2 GetPixieDustPosition()
		{
			return pixieDustPosition;
		}
		// Method to get the lives of the player to restart the game
		int GetLives()
		{
			return lives;
		}
		// Method to get all the guards and frogs
		std::vector<SimObject*> GetEnemies()
		{
			enemies.clear();
			for (auto g : guards)
			{
				enemies.emplace_back(g);
			}
			for (auto f : froggos)
			{
				enemies.emplace_back(f);
			}
			return enemies;
		}
		// Method to get all the pixies
		std::vector<SimObject*> GetPixies()
		{
			return pixies;
		}
		// Method to get the map for the frog
		GameMap* GetMap()
		{
			return currentMap;
		}

		// Method to check what is colliding with what
		bool CheckCollisionObjects(CollisionPair* collisionInfo);

	protected:
		// Method to initialise the start of the game
		void InitialiseGame();

		// Initialise variables and objects for the game
		TextureManager*		texManager;
		GameSimsPhysics*	physics;
		GameMap* currentMap;

		PlayerCharacter* player;

		float gameTime;

		int currentScore;
		int magicCount;
		int dustCount;
		int lives;
		int activeFruitCount;

		int activePixieDust;
		float attempted;

		int pixieMode;

		bool drawColliders;
		bool justToggled;

		Vector2 pixieDustPosition;

		std::vector<SimObject*> gameObjects;
		std::vector<SimObject*> newObjects;
		std::vector<SimObject*> guards;
		std::vector<SimObject*> froggos;
		std::vector<SimObject*> enemies;
		std::vector<SimObject*> fruits;
		std::vector<SimObject*> pixies;
	};
}