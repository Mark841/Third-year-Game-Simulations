#include "GameMap.h"
#include "GameSimsRenderer.h"
#include "TextureManager.h"
#include "CollisionVolume.h"
#include "AABB.h"
#include "../../Common/Maths.h"
#include "../../Common/Assets.h"
#include "../../Common/TextureLoader.h"
#include <fstream>
#include <iostream>

using namespace NCL;
using namespace CSC3222;
using namespace Rendering;

// GameMap constructor
GameMap::GameMap(const std::string& filename, std::vector<SimObject*>& objects, TextureManager& texManager, GameSimsPhysics* physics)	{
	tileTexture = texManager.GetTexture("FruitWizard//arcade_platformerV2.png");

	std::ifstream mapFile(Assets::DATADIR + filename);

	// If there is an error with loading the map file
	if (!mapFile) {
		std::cout << "GameMap can't be loaded in!" << std::endl;
		return;
	}

	mapFile >> mapWidth;
	mapFile >> mapHeight;

	mapData		= new char[mapWidth * mapHeight];
	mapCosts	= new int[mapWidth * mapHeight];

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = ((mapHeight - 1 - y) * mapWidth) + x;
			mapFile >> mapData[tileIndex];
		}
	}
	
	// Loop through all tile indexes
	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = ((mapHeight - 1 - y) * mapWidth) + x;
			char c;
			mapFile >> c;
			mapCosts[tileIndex] = c - '0';

			// Create a node for this tile
			MapNode* node = new MapNode;
			node->worldPosX = (x * 16) + 8;
			node->worldPosY = (y * 16) + 8;
			node->tilePosX = x;
			node->tilePosY = y;
			node->cost = c - '0';
			node->index = tileIndex;
			node->bestParent = NULL;
			node->closed = false;
			// Add the node to the vector
			nodeData.emplace_back(node);
		}
	}

	// Build the map
	BuildMapMesh(physics);

	// Group map colliders so there isn't one for each block but instead groups similar blocks
	mapColliders->GroupMapColliders();
	// Apply the grouped collider vector to the physics class
	physics->AddCollider(mapColliders);
}

// GameMap destructor
GameMap::~GameMap()	{
	delete[] mapData;
	delete[] mapCosts;
	delete[] mapTexCoords;

	delete mapMesh;
}

// Method to draw the map to screen
void GameMap::DrawMap(GameSimsRenderer & r) {
	r.DrawMesh((OGLMesh*)mapMesh, (OGLTexture*)tileTexture);
}

// Method to build the map
void GameMap::BuildMapMesh(GameSimsPhysics* physics) {
	vector<Vector2> texCoords;
	vector<Vector3> positions;

	Vector2 primaryTile;

	// Information for the scene background
	AddNewTilePos(0, 0, physics, ColliderType::Background, positions, Vector2(mapWidth *16.0f, mapHeight * 16.0f));
	AddNewTileTex(0, 176, texCoords, Vector2(160, 128));

	// Create variable to store what type of collider to make
	ColliderType type;

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileType = mapData[(y * mapWidth) + x];
	
			switch (tileType) {
			case '0': {
				continue;
			}break; 
				//the normal tile types first

				// Top left of ladder
				case 'T':primaryTile = Vector2(320, 0);
					type = ColliderType::Ladder; 
					break; 
				// Mid left of ladder
				case 'L':primaryTile = Vector2(320, 16);
					type = ColliderType::Ladder; 
					break; 
				// Bottom left of ladder
				case 'B':primaryTile = Vector2(320, 32);
					type = ColliderType::Ladder; 
break;

// Top right of ladder
				case 't':primaryTile = Vector2(336, 0);
					type = ColliderType::Ladder;
					break;
					// Mid right of ladder
				case 'l':primaryTile = Vector2(336, 16);
					type = ColliderType::Ladder;
					break;
					// Bottom right of ladder
				case 'b':primaryTile = Vector2(336, 32);
					type = ColliderType::Ladder;
					break;

					// Random grass tiles
				case 'G': {
					primaryTile = Vector2(224, 48);
					type = ColliderType::Wall;
				}break;
					// Left end of random grass tiles
				case 'F': {
					primaryTile = Vector2(208, 48);
					type = ColliderType::Wall;
				}break;
					// Right end of random grass tiles
				case 'H': {
					primaryTile = Vector2(240, 48);
					type = ColliderType::Wall;
				}break;

					// Wall tile
				case 'W': {
					primaryTile = Vector2(256, 112);
					type = ColliderType::Wall;
				}break;
					// Top of wall
				case 'w': {
					primaryTile = Vector2(256, 96);
					type = ColliderType::Wall;
				}break;
			}
			// Add the tile 
			AddNewTilePos(x, y, physics, type, positions);
			AddNewTileTex((int)primaryTile.x, (int)primaryTile.y, texCoords);
		}
	}

	// Apply tile info to map
	mapMesh = new OGLMesh();
	mapMesh->SetVertexPositions(positions);
	mapMesh->SetVertexTextureCoords(texCoords);
	mapMesh->SetPrimitiveType(NCL::GeometryPrimitive::Triangles);
	mapMesh->UploadToGPU();
}

// Method to add the position of each tile and creates a collider for that tile
void GameMap::AddNewTilePos(int x, int y, GameSimsPhysics* physics, ColliderType type, std::vector<Vector3>& pos, Vector2 tileSize) {
	Vector3 topLeft = Vector3((x + 0) * tileSize.x, (y + 0) * tileSize.y, 0);
	Vector3 topRight = Vector3((x + 1) * tileSize.x, (y + 0) * tileSize.y, 0);
	Vector3 bottomLeft = Vector3((x + 0) * tileSize.x, (y + 1) * tileSize.y, 0);
	Vector3 bottomRight = Vector3((x + 1) * tileSize.x, (y + 1) * tileSize.y, 0);

	// If the object isn't the background, make a AABB collider for it
	if (!(type == ColliderType::Background)) {
		mapColliders->AddNewCollider(new AABB(Vector2((x * tileSize.x) + (tileSize.x / 2), (y * tileSize.y) + (tileSize.y / 2)), tileSize.x / 2, tileSize.y / 2, type, ableToMove::STATIC));
	}


	pos.emplace_back(topLeft);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(topRight);

	pos.emplace_back(topRight);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(bottomRight);
}

// Method to apply texture to a tile
void GameMap::AddNewTileTex(int x, int y, std::vector<Vector2>& tex, Vector2 texSize) {
	Vector2 topLeft = Vector2((x + 0.0f), (y + texSize.y));
	Vector2 topRight = Vector2((x + texSize.x), (y + texSize.y));
	Vector2 bottomLeft = Vector2((x + 0.0f), (y + 0.0f));
	Vector2 bottomRight = Vector2((x + texSize.x), (y + 0.0f));

	tex.emplace_back(topLeft);
	tex.emplace_back(bottomLeft);
	tex.emplace_back(topRight);

	tex.emplace_back(topRight);
	tex.emplace_back(bottomLeft);
	tex.emplace_back(bottomRight);
}

// Get the tile position from a world position
Vector2 GameMap::WorldPosToTilePos(Vector2 worldPos) const
{
	// Get the tile position
	int x = floor(worldPos.x / 16);
	int y = floor(worldPos.y / 16);
	return Vector2(x, y);
}
// Get the world position from a tile position
Vector2 GameMap::TilePosToWorldPos(Vector2 tilePos) const
{
	// Get the world position
	int x = (tilePos.x * 16) + 8;
	int y = (tilePos.y * 16) + 8;
	return Vector2(x, y);
}

// Method to get a MapNode from worldPosition
MapNode* GameMap::GetTileFromWorldPos(Vector2 worldPos)
{
	// Get tile position
	Vector2 tileLocation = WorldPosToTilePos(worldPos);
	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = ((mapHeight - 1 - y) * mapWidth) + x;
			// Check if the tile is corresponding to one in the map nodes vector
			if (tileLocation.x == nodeData[tileIndex]->tilePosX && tileLocation.y == nodeData[tileIndex]->tilePosY)
			{
				// Return the matching node
				return nodeData[tileIndex];
			}
		}
	}
	// Else return the first node in the vector
	return nodeData[0];
}

// Get the manhattan distance heuristic from A to B
float GameMap::Heuristic(MapNode* a, MapNode* b)
{
	// Return the manhattan distance between the two objects
	return abs(a->tilePosX - b->tilePosX) + abs(a->tilePosY - b->tilePosY);
}
// Get the starting stage for the algorithm
std::vector<MapNode*> GameMap::AlgorithmInitialisation(Vector2 from, Vector2 targetObject)
{
	// Clear any nodes from the open list
	openList.clear();
	// Reset any data needed by the algorithm (closde list and parent nodes)
	for (auto node : nodeData)
	{
		node->closed = false;
		node->bestParent = NULL;
	}

	// Set node A and B
	MapNode* A = GetTileFromWorldPos(from);
	MapNode* B = GetTileFromWorldPos(targetObject);

	// Set node A information
	A->bestParent = A;
	A->g = 0;
	A->h = Heuristic(A, B);
	A->f = A->g + A->h;

	// Add node A to the open list
	openList.emplace_back(A);
	 
	// Return the algorithm result
	return AStarAlgorithm(A, B);
}

// Get the surrounding valid neighbouring nodes
std::vector<MapNode*> GameMap::GetNeighbouringNodes(MapNode* node)
{
	// Make a vector to store all the neighbouring ndoes
	std::vector<MapNode*> activeNeighbours;

	// Get left tile
	if (node->tilePosX < mapWidth - 2)
	{
		int tileCost = mapCosts[((node->tilePosY)*mapWidth) + (node->tilePosX - 1)];
		// if tile is a valid move
		if (tileCost != 0)
		{
			// Get the node that is the left neighbour and set its cost
			MapNode* neighbour = nodeData[((node->tilePosY)*mapWidth) + (node->tilePosX - 1)];
			neighbour->cost = tileCost;

			// If no existing parent make this node the parent of the neighbour
			if (neighbour->bestParent == NULL)
			{
				neighbour->bestParent = node;
			}
			// Add neighbour to the neighbours vector
			activeNeighbours.emplace_back(neighbour);
		}
	}
	// Get right tile
	if (node->tilePosX > 0)
	{
		int tileCost = mapCosts[((node->tilePosY) * mapWidth) + (node->tilePosX + 1)];
		// if tile is a valid move
		if (tileCost != 0)
		{
			// Get the node that is the right neighbour and set its cost
			MapNode* neighbour = nodeData[((node->tilePosY) * mapWidth) + (node->tilePosX + 1)];
			neighbour->cost = tileCost;

			// If no existing parent make this node the parent of the neighbour
			if (neighbour->bestParent == NULL)
			{
				neighbour->bestParent = node;
			}
			// Add neighbour to the neighbours vector
			activeNeighbours.emplace_back(neighbour);
		}
	}
	// Get above tile
	if (node->tilePosY < mapHeight - 2)
	{
		int tileCost = mapCosts[((node->tilePosY + 1) * mapWidth) + (node->tilePosX)];
		// if tile is a valid move
		if (tileCost != 0)
		{
			// Get the node that is the above neighbour and set its cost
			MapNode* neighbour = nodeData[((node->tilePosY + 1) * mapWidth) + (node->tilePosX)];
			neighbour->cost = tileCost;

			// If no existing parent make this node the parent of the neighbour
			if (neighbour->bestParent == NULL)
			{
				neighbour->bestParent = node;
			}
			// Add neighbour to the neighbours vector
			activeNeighbours.emplace_back(neighbour);
		}
	}
	// Get below tile
	if (node->tilePosY > 0)
	{
		int tileCost = mapCosts[((node->tilePosY - 1) * mapWidth) + (node->tilePosX)];
		// if tile is a valid move
		if (tileCost != 0)
		{
			// Get the node that is the below neighbour and set its cost
			MapNode* neighbour = nodeData[((node->tilePosY - 1) * mapWidth) + (node->tilePosX)];
			neighbour->cost = tileCost;

			// If no existing parent make this node the parent of the neighbour
			if (neighbour->bestParent == NULL)
			{
				neighbour->bestParent = node;
			}
			// Add neighbour to the neighbours vector
			activeNeighbours.emplace_back(neighbour);
		}
	}
	// Return any neighbours
	return activeNeighbours;
}

// The A* pathfinding algorithm
std::vector<MapNode*> GameMap::AStarAlgorithm(MapNode* A, MapNode* B)
{
	std::vector<MapNode*> path;

	while (openList.size() > 0)
	{
		// P = best node in the open list (lowest f-value)
		MapNode* P = openList[0];
		// For each node in the ope nlist
		for (auto node : openList)
		{
			// If node doesn't have an f value yet ignore it
			if (node->f == NULL)
			{
				continue;
			}
			// Else if the f value is lower set P to that node
			else if (node->f < P->f && node->closed != true)
			{
				P = node;
			}
		}

		// If found a full path, as node to check is the destination node
		if (P == B)
		{
			// Return the found path
			return GeneratePath(path, A, B);
		}

		// Else not found a path yet
		else
		{
			// Get the nodes neighbours
			std::vector<MapNode*> neighbours = GetNeighbouringNodes(P);

			// For each node neighbour 'node' connected to P
			for (auto node : neighbours)
			{
				// Calculate the g,f,h values for the neighbouring 'node'
				float g = (node->cost) + (node->bestParent)->g;
				float h = Heuristic(node, B);
				float f = (node->g) + (node->h);

				// If 'node' is in closed list ignore it
				if (node->closed == true)
				{
					continue;
				}
				// If ndoe is in openlist and the calculated f value is greater than the nodes existing f value, ignore it
				if (std::find(openList.begin(), openList.end(), node) != openList.end() && f > node->f)
				{
					continue;
				}
				// Else the node could be a valid option
				else
				{
					// Set the nodes values and new best parent
					node->g = g;
					node->h = h;
					node->f = f;
					node->bestParent = P;

					// If 'node' not in openList already, add it to it
					if (std::find(openList.begin(), openList.end(), node) == openList.end())
					{
						openList.emplace_back(node);
					}
				}
			}

			// Remove P from openList
			openList.erase(std::find(openList.begin(), openList.end(), P));
			// Insert P to closedList
			P->closed = true;
		}
	}
	return path;
}
// Generate a path from the end point to the start point
std::vector<MapNode*> GameMap::GeneratePath(std::vector<MapNode*> path, MapNode* A, MapNode* B)
{
	// Clear any existing path
	path.clear();
	// Set the start node to the final one in the path
	MapNode* R = B;
	// While the path hasn't reached the start node
	while (R != A)
	{
		// Add the node to the path
		path.emplace_back(R);
		// Set the node to its parent node
		R = R->bestParent;
	}
	// Return the path
	return path;
}