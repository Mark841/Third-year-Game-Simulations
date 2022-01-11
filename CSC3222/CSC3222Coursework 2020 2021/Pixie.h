#pragma once
#include "SimObject.h"
#include "FruitWizardGame.h"

namespace NCL::CSC3222 {
	// Pixie class - extends the SimObject class
	class Pixie : public SimObject	{
	public:
		// Pixie constructor and deconstructor
		Pixie();
		~Pixie();

		// Update pixie method takes in the time between frames
		bool UpdateObject(float dt) override;

		// Boid Alignment method to keep near boids in same direction
		Vector2 Alignment();
		// Boid Seperation method to keep boids at least a certain distance from each other
		Vector2 Seperation();
		// Boid Cohesion method to keep boids near each other
		Vector2 Cohesion();
		// Boid Avoidance method to keep boids away from enemies
		Vector2 Avoidance();
		// Boid Flocking method to apply all other boid methods and get a position for the Pixie
		void Flocking(float dt);

	protected:
		// Threshold variables for flocking
		float alignmentThreshold;
		float seperationThreshold;
		float cohesionThreshold;
		float avoidanceThreshold;

		// Vector to store enemies and pixies
		std::vector<SimObject*> enemies;
		std::vector<SimObject*> pixies;
	};
}