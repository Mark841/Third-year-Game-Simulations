#pragma once
#include <string>
#include <vector>
#include "GameSimsPhysics.h"
#include "CollisionVolume.h"
#include "SimObject.h"

#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/MeshGeometry.h"

//
struct MapNode
{
	MapNode* bestParent;
	int worldPosX;
	int worldPosY;
	int tilePosX;
	int tilePosY;
	int index;
	int cost;
	float g;
	float h;
	float f;
	bool closed;
};

namespace NCL {
	namespace Rendering {
		class TextureBase;
	}
	using namespace Maths;
	namespace CSC3222 {
		class GameSimsRenderer;
		class SimObject;
		class TextureManager;

		// GameMap class, handles the map
		class GameMap	{
		public:
			// Constructor for the GameMap, takes in the filename of the map, AN UNKNOWN AND UNUSED ARRAY OF OBJECTS, a texture manager reference to import and handle textures and a physics class reference so can add colliders
			GameMap(const std::string& filename, std::vector<SimObject*>& objects, TextureManager& texManager, GameSimsPhysics* physics);
			~GameMap();

			// Method to draw the map to screen
			void DrawMap(GameSimsRenderer & r);

			// Method to get the maps width
			int GetMapWidth() const {
				return mapWidth;
			}

			// Method to get the maps height
			int GetMapHeight() const {
				return mapHeight;
			}

			// Get the tile position from a world position
			Vector2 WorldPosToTilePos(Vector2 worldPos) const;
			// Get the world position from a tile position
			Vector2 TilePosToWorldPos(Vector2 tilePos) const;

			// Method to get a MapNode from worldPosition
			MapNode* GetTileFromWorldPos(Vector2 worldPos);

			// Get the manhattan distance heuristic from A to B
			float Heuristic(MapNode* a, MapNode* b);
			// Get the starting stage for the algorithm
			std::vector<MapNode*> AlgorithmInitialisation(Vector2 from, Vector2 targetObject);
			// Get the surrounding valid neighbouring nodes
			std::vector<MapNode*> GetNeighbouringNodes(MapNode* node);
			// The A* pathfinding algorithm
			std::vector<MapNode*> AStarAlgorithm(MapNode* A, MapNode* B);
			// Generate a path from the end point to the start point
			std::vector<MapNode*> GeneratePath(std::vector<MapNode*> path, MapNode* A, MapNode* B);

		protected:
			// Method to build the map
			void BuildMapMesh(GameSimsPhysics* physics);
			// Method to add the position of each tile and creates a collider for that tile
			void AddNewTilePos(int x, int y, GameSimsPhysics* physics, ColliderType type, std::vector<Vector3>& pos, Vector2 tileSize = Vector2(16, 16));
			// Method to apply texture to a tile
			void AddNewTileTex(int x, int y, std::vector<Vector2>& tex, Vector2 texSize = Vector2(16,16));

			// Variables to keep track of the map information
			int mapWidth;
			int mapHeight;
			int structureCount;

			Rendering::TextureBase*	tileTexture;
			char*			mapData;
			int*			mapCosts;

			std::vector<MapNode*> nodeData;
			std::vector<MapNode*> openList;

			Vector2*		mapTexCoords;

			MeshGeometry* mapMesh;

			// A CollisionVolume to contain the colliders for the map
			CollisionVolume* mapColliders = new CollisionVolume();

		};
	}
}

