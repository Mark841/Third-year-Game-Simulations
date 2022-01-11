#pragma once
#include "../../Common/Vector2.h"
#include "../../Common/Vector4.h"
#include "RigidBody.h"
#include "CollisionVolume.h"
#include"CollisionPair.h"

namespace NCL {
	using namespace Maths;

	namespace Rendering {
		class TextureBase;
	}
	namespace CSC3222 {
		class GameSimsRenderer;
		class TextureManager;
		class FruitWizardGame;
		class RigidBody;
		class CollisionVolume;

		// SimObject class that extends the RigidBody superclass
		class SimObject	: public RigidBody {
		public:
			// SimObject constructor takes in nothing
			SimObject();
			~SimObject();

			// Generalised method to update collider, can be overriden in subclasses to incorporate an offset
			void UpdateCollider()
			{
				// If it has a set collider
				if (collider != nullptr)
				{
					// Update the colliders position
					collider->UpdateColliders(position);
				}
			}

			// Method to get the collider of an object
			CollisionVolume* GetCollider() const {
				return collider;
			}

			// Method to update the animation frame
			void	UpdateAnimFrame(float dt);

			// Virtual method to update an object
			virtual bool UpdateObject(float dt) = 0;

			// Virtual method to draw an object
			virtual void DrawObject(GameSimsRenderer &r);

			// Method to initialise the game and manager variables
			static void InitObjects(FruitWizardGame* game, TextureManager* manager);

			// Method to assign the leading object in a spring - SHOULD CHANGE TO FOLLOWING OBJECT SO CAN THEN GO DOWN THE POINTERS TO FINAL OBJECT AND MAKE A CHAIN FROM THEM
			void SetLeadingSpringObject(SimObject* leadingObject)
			{
				attachedSpring = leadingObject;
			}
			
			// Method to calulate whats needed for a spring
			void SpringCalculations();


		protected:
			// Store pointers to a texture and a collider
			CollisionVolume*		collider;
			Rendering::TextureBase* texture;

			// Store information for frames
			Vector4			animFrameData;
			int				currentanimFrame;
			bool			flipAnimFrame;
			int				animFrameCount;
			float			frameTime;
			float			frameRate;

			// Store references to game and texture objects
			static TextureManager*  texManager;
			static FruitWizardGame*	game;

			// Variables to store spring information
			SimObject*		attachedSpring;
			CollisionPair*	springInfo;
			// Set spring resting length between 2 centres
			float			springLength;
			float			springSnappiness;
			float			springDampingConstant;
		};
	}
}

