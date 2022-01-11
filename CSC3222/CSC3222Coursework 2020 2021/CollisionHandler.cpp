#include "CollisionHandler.h"

using namespace NCL;
using namespace CSC3222;

// Method for colliding 2 shapes and passing on to the correct type methods
float CollisionHandler::Collide(Collider* s1, Collider* s2)
{
	// Check if the pointers are for the same Collider object
	if (s1 == s2)
	{ // Return -1 (negative) for a collision as the Colliders are the same object, can't collide with itself
		return -1.0f;
	}

	// If both colliders are Spheres
	if (s1->GetShape() == ColliderShape::Sphere && s2->GetShape() == ColliderShape::Sphere)
	{ // Cast the shapes to the correct class type
		Sphere* sphere1 = dynamic_cast<Sphere*>(s1);
		Sphere* sphere2 = dynamic_cast<Sphere*>(s2);
		return CollideCircles(sphere1, sphere2);
	}
	// If one collider is a AABB and the other is a Sphere
	else if (s1->GetShape() == ColliderShape::AABB && s2->GetShape() == ColliderShape::Sphere)
	{ // Cast the Colliders to the correct class type
		AABB* aabb = dynamic_cast<AABB*>(s1);
		Sphere* sphere = dynamic_cast<Sphere*>(s2);
		return CollideCircleAndSquare(aabb, sphere);
	}
	// If one collider is a Sphere and the other is a AABB
	else if (s1->GetShape() == ColliderShape::Sphere && s2->GetShape() == ColliderShape::AABB)
	{ // Cast the Colliders to the correct class type
		Sphere* sphere = dynamic_cast<Sphere*>(s1);
		AABB* aabb = dynamic_cast<AABB*>(s2);
		return CollideCircleAndSquare(aabb, sphere);
	}
	// If both colliders are AABBs
	else if (s1->GetShape() == ColliderShape::AABB && s2->GetShape() == ColliderShape::AABB)
	{ // Cast the AABBs to the correct class type
		AABB* aabb1 = dynamic_cast<AABB*>(s1);
		AABB* aabb2 = dynamic_cast<AABB*>(s2);
		return CollideSquares(aabb1, aabb2);
	}
	// Else if the shapes aren't any of the extended collider versions return -1 (negative) for a collision, this could happen
	// if a new shape has been extended from Collider but hasnt had collision methods made for it yet
	return -1.0f;
}

// Method for colliding 2 shapes and passing on to the correct type methods along with information about the collision
float CollisionHandler::Collide(Collider* s1, Collider* s2, CollisionPair* collisionInfo)
{
	// Check if the pointers are for the same Collider object
	if (s1 == s2)
	{ // Return -1 (negative) for a collision as the Colliders are the same object, can't collide with itself
		return -1.0f;
	}

	// If both colliders are Spheres
	if (s1->GetShape() == ColliderShape::Sphere && s2 -> GetShape() == ColliderShape::Sphere)
	{ // Cast the shapes to the correct class type
		Sphere* sphere1 = dynamic_cast<Sphere*>(s1);
		Sphere* sphere2 = dynamic_cast<Sphere*>(s2);
		float penetrationDistance = CollideCircles(sphere1, sphere2);
		// Only set the collision normal for this method here as this is the only one which isn't passed in collision info to avoid making 2 versions of it
		collisionInfo->collisionNormal = -Vector2(s1->GetCentre() - s2->GetCentre()).Normalised();
		return penetrationDistance;
	}
	// If one collider is a AABB and the other is a Sphere
	else if (s1->GetShape() == ColliderShape::AABB && s2->GetShape() == ColliderShape::Sphere)
	{ // Cast the Colliders to the correct class type
		AABB* aabb = dynamic_cast<AABB*>(s1);
		Sphere* sphere = dynamic_cast<Sphere*>(s2);
		float penetrationDistance = CollideCircleAndSquare(aabb, sphere, collisionInfo);
		return penetrationDistance;
	}
	// If one collider is a Sphere and the other is a AABB
	else if (s1->GetShape() == ColliderShape::Sphere && s2->GetShape() == ColliderShape::AABB)
	{ // Cast the Colliders to the correct class type
		Sphere* sphere = dynamic_cast<Sphere*>(s1);
		AABB* aabb = dynamic_cast<AABB*>(s2);
		float penetrationDistance = CollideCircleAndSquare(aabb, sphere, collisionInfo);
		// Method always considers AABB is object 1 and Sphere is object 2 but because they were passed into this method other way 
		// round and then swapped to go into the collision method, now need to flip the collision normal that was made
		collisionInfo->collisionNormal = -collisionInfo->collisionNormal;
		return penetrationDistance;
	}
	// If both colliders are AABBs
	else if (s1->GetShape() == ColliderShape::AABB && s2->GetShape() == ColliderShape::AABB)
	{ // Cast the AABBs to the correct class type
		AABB* aabb1 = dynamic_cast<AABB*>(s1);
		AABB* aabb2 = dynamic_cast<AABB*>(s2);
		float penetrationDistance = CollideSquares(aabb1, aabb2, collisionInfo);
		return penetrationDistance;
	}
	// Else if the shapes aren't any of the extended collider versions return -1 (negative) for a collision, this could happen
	// if a new shape has been extended from Collider but hasnt had collision methods made for it yet
	return -1.0f;
}

// Check for collision between 2 AABB's, this method is used for the walls grouping so doesnt have to have collision info passed in
float CollisionHandler::CollideSquares(AABB* s1, AABB* s2)
{
	// Get the distance between the shapes on x axis - if its negative then shapes are overlapping
	float xDistance = abs((float) s1->GetCentre().x - (float) s2->GetCentre().x) - s1->GetHorizontalDistFromCentreToEdge() - s2->GetHorizontalDistFromCentreToEdge();
	// Get the distance between the shapes on y axis - if its negative then shapes are overlapping
	float yDistance = abs((float) s1->GetCentre().y - (float) s2->GetCentre().y) - s1->GetVerticalDistFromCentreToEdge() - s2->GetVerticalDistFromCentreToEdge();

	if (xDistance >= 0)
	{ // If the squares dont overlap on the x axis
		return -1.0f;
	}
	if (yDistance >= 0)
	{ // If the squares dont overlap on the y axis
		return -1.0f;
	}
	// returns the penetration distance between the two overlapping corners or sides
	return sqrt((xDistance * xDistance) + (yDistance * yDistance));
}
// Check for collision between 2 AABB's
float CollisionHandler::CollideSquares(AABB* s1, AABB* s2, CollisionPair* collisionInfo)
{
	// Get the distance between the shapes on x axis - if its negative then shapes are overlapping
	float xDistance = abs((float)s1->GetCentre().x - (float)s2->GetCentre().x) - s1->GetHorizontalDistFromCentreToEdge() - s2->GetHorizontalDistFromCentreToEdge();
	// Get the distance between the shapes on y axis - if its negative then shapes are overlapping
	float yDistance = abs((float)s1->GetCentre().y - (float)s2->GetCentre().y) - s1->GetVerticalDistFromCentreToEdge() - s2->GetVerticalDistFromCentreToEdge();

	if (xDistance >= 0)
	{ // If the squares dont overlap on the x axis
		return -1.0f;
	}
	if (yDistance >= 0)
	{ // If the squares dont overlap on the y axis
		return -1.0f;
	}

	float penetrationDistance = 0.0f;
	// Figure out whats above or to side of what for which way round normal should be
	if (xDistance > yDistance && (float)s1->GetCentre().x > (float)s2->GetCentre().x)
	{ // If A is on RIGHT of B and the minimum overlap is on the x axis
		collisionInfo->collisionNormal = Vector2(-1, 0);
		penetrationDistance = abs(xDistance);
	}
	else if (xDistance > yDistance && (float)s1->GetCentre().x < (float)s2->GetCentre().x)
	{ // If A is on LEFT of B and the minimum overlap is on the x axis
		collisionInfo->collisionNormal = Vector2(1, 0);
		penetrationDistance = abs(xDistance);
	}
	else if (xDistance < yDistance && (float)s1->GetCentre().y < (float)s2->GetCentre().y)
	{ // If A is BELOW B and the minimum overlap is on the y axis
		collisionInfo->collisionNormal = Vector2(0, 1);
		penetrationDistance = abs(yDistance);
	}
	else
	{ // If A is ABOVE B and the minimum overlap is on the y axis
		collisionInfo->collisionNormal = Vector2(0, -1);
		penetrationDistance = abs(yDistance);
	}
	// returns the penetration distance between the two overlapping corners or sides
	return penetrationDistance;
}

// Check for collision between a AABB and a Sphere
float CollisionHandler::CollideCircleAndSquare(AABB* s, Sphere* c)
{
	// Distance from the centre of the Sphere to edge of the AABB on x axis, type converts to float to get more precise calculations and 
	// avoid rounding errors during the calculation
	float x = abs((float)c->GetCentre().x - (float)s->GetCentre().x) - (s->GetHorizontalDistFromCentreToEdge());
	// Distance from the centre of the Sphere to edge of the AABB on y axis, type converts to float to get more precise calculations and 
	// avoid rounding errors during the calculation
	float y = abs((float)c->GetCentre().y - (float)s->GetCentre().y) - (s->GetVerticalDistFromCentreToEdge());

	float penetrationDistance;
	// If the distance from the centre of the Sphere to the edge of the AABB is more than 0 on the x axis
	if (x > 0)
	{
		// If the distance from the centre of the Sphere to the edge of the AABB is more than 0 on the y axis
		if (y > 0)
		{ // If the hypotenuse length squared (traingle between the closest point of the AABB and the centre of the Sphere) is less
		  // than the radius squared (how far on the hypotenuse it'd have to be for overlap) then return true for an overlap
			penetrationDistance = sqrt(c->GetRadius() * c->GetRadius()) - sqrt((x * x) + (y * y));

		}
		// Else (if the y axis difference is 0) check the x axis overlap
		else
		{ // If the horizontal distance from the Sphere centre to the edge of the AABB is less than the radius then they must overlap
			penetrationDistance = c->GetRadius() - x;
		}
	}
	// Else (if the x axis difference is 0) the only thing to check is the y axis distance
	else
	{ // If the vertical distance from the Sphere centre to the edge of the AABB is less than the radius then they must overlap
		penetrationDistance = c->GetRadius() - y;
	}

	if (penetrationDistance >= 0)
	{ // If they're colliding
		// Return the penetration distance
		return penetrationDistance;
	}
	else
	{ // Else there wasn't a collision
		return -1.0f;
	}
}
// Check for collision between a AABB and a Sphere
float CollisionHandler::CollideCircleAndSquare(AABB* s, Sphere* c, CollisionPair* collisionInfo)
{
	// Distance from the centre of the Sphere to edge of the AABB on x axis, type converts to float to get more precise calculations and 
	// avoid rounding errors during the calculation
	float x = abs((float)c->GetCentre().x - (float)s->GetCentre().x) - (s->GetHorizontalDistFromCentreToEdge());
	// Distance from the centre of the Sphere to edge of the AABB on y axis, type converts to float to get more precise calculations and 
	// avoid rounding errors during the calculation
	float y = abs((float)c->GetCentre().y - (float)s->GetCentre().y) - (s->GetVerticalDistFromCentreToEdge());

	float penetrationDistance;
	// If the distance from the centre of the Sphere to the edge of the AABB is more than 0 on the x axis
	if (x > 0)
	{
		// If the distance from the centre of the Sphere to the edge of the AABB is more than 0 on the y axis
		if (y > 0)
		{ // If the hypotenuse length squared (traingle between the closest point of the AABB and the centre of the Sphere) is less
		  // than the radius squared (how far on the hypotenuse it'd have to be for overlap) then return true for an overlap
			penetrationDistance = sqrt(c->GetRadius() * c->GetRadius()) - sqrt((x * x) + (y * y));
			
		}
		// Else (if the y axis difference is 0) check the x axis overlap
		else
		{ // If the horizontal distance from the Sphere centre to the edge of the AABB is less than the radius then they must overlap
			penetrationDistance = c->GetRadius() - x;
		}
	}
	// Else (if the x axis difference is 0) the only thing to check is the y axis distance
	else
	{ // If the vertical distance from the Sphere centre to the edge of the AABB is less than the radius then they must overlap
		penetrationDistance = c->GetRadius() - y;
	}

	if (penetrationDistance >= 0)
	{ // If they're colliding
		if (x > 0 && y > 0)
		{ // If the circle is colliding with one of the AABBs corners
			Vector2 closestPoint;
			if (((float)s->GetCentre().x < (float)c->GetCentre().x) && ((float)s->GetCentre().y < (float)c->GetCentre().y))
			{ // if colliding on the top right corner of AABB
				closestPoint = Vector2(s->GetCentre().x + s->GetHorizontalDistFromCentreToEdge(), s->GetCentre().y + s->GetVerticalDistFromCentreToEdge());
			}
			else if (((float)s->GetCentre().x > (float)c->GetCentre().x) && ((float)s->GetCentre().y < (float)c->GetCentre().y))
			{ // if colliding on the top left corner of AABB
				closestPoint = Vector2(s->GetCentre().x - s->GetHorizontalDistFromCentreToEdge(), s->GetCentre().y + s->GetVerticalDistFromCentreToEdge());
			}
			else if (((float)s->GetCentre().x > (float)c->GetCentre().x) && ((float)s->GetCentre().y > (float)c->GetCentre().y))
			{ // if colliding on the bottom left corner of AABB
				closestPoint = Vector2(s->GetCentre().x - s->GetHorizontalDistFromCentreToEdge(), s->GetCentre().y - s->GetVerticalDistFromCentreToEdge());
			}
			else
			{ // if colliding on the bottom right corner of AABB
				closestPoint = Vector2(s->GetCentre().x + s->GetHorizontalDistFromCentreToEdge(), s->GetCentre().y - s->GetVerticalDistFromCentreToEdge());
			}
			collisionInfo->collisionNormal = Vector2(c->GetCentre() - closestPoint).Normalised();;
		}
		else if (x > 0 && (float)s->GetCentre().x > (float)c->GetCentre().x)
		{ // If the Sphere is colliding with AABB on the LEFT
			collisionInfo->collisionNormal = Vector2(-1, 0);
		}
		else if (x > 0 && (float)s->GetCentre().x < (float)c->GetCentre().x)
		{ // If the Sphere is colliding with AABB on the RIGHT
			collisionInfo->collisionNormal = Vector2(1, 0);
		}
		else if (x < 0 && (float)s->GetCentre().y < (float)c->GetCentre().y)
		{ // If the Sphere is colliding with AABB from ABOVE
			collisionInfo->collisionNormal = Vector2(0, 1);
		}
		else
		{ // If the Sphere is colliding with AABB from BELOW 
			collisionInfo->collisionNormal = Vector2(0, -1);
		}
		// returns the penetration distance
		return penetrationDistance;
	}
	else
	{ // Else there wasn't a collision
		return -1.0f;
	}
}

// Check for collision between 2 circles
float CollisionHandler::CollideCircles(Sphere* c1, Sphere* c2)
{ // Make a triangle between the two Sphere centres
	// Get the x distance between the edges of the Spheres, doesn't need to use absolute values like in AABB and Spheres collision as gonna be squared, 
	// type converts to float to get more precise calculations and avoid rounding errors during the calculation
	float xDistance = abs((float)c1->GetCentre().x) - ((float)c2->GetCentre().x);
	// Get the y distance between the edges of the Spheres, doesn't need to use absolute values like in AABB and Spheres collision as gonna be squared, 
	// type converts to float to get more precise calculations and avoid rounding errors during the calculation
	float yDistance = abs((float)c1->GetCentre().y) - ((float)c2->GetCentre().y);

	// Get the hypotenuse of the triangle between the two centres of the Spheres
	float distance = sqrt((xDistance * xDistance) + (yDistance * yDistance));

	// If the circles overlap (distance between centres is less than the total of both the radius') then they must be colliding
	if (distance <= (c1->GetRadius() + c2->GetRadius()))
	{
		float penetrationDistance = abs((c1->GetRadius() + c2->GetRadius()) - distance);
		// returns the penetration distance
		return penetrationDistance;
	}
	// If no overlap return false (negative)
	return -1.0f;
}
