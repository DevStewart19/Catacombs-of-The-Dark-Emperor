
#include "Stdafx.h"
#include "Entity.h"
#include "Assets.h"
#include "Utils.h"
#include "Level.h"
#include "Collision.h"
#include "Globals.h"

std::vector<AABB> Entity::collides;

Entity::Entity(std::string name)
{
	Assets* assets = Assets::Instance();
	sf::Texture& tex = assets->GetTexture(name);

	sprite.setTexture(tex);
	Vector2u texSize = tex.getSize();

	// By default, use the size of the image to figure out
	// collision size.
	size = Vector2f((float)texSize.x, (float)texSize.y) / PIXELS_PER_UNIT;
}

void Entity::Draw(sf::RenderWindow& window)
{
	// Multiply the world position by PIXELS_PER_UNIT to
	// get the pixel location to draw at.
	Vector2f drawP = position * PIXELS_PER_UNIT;

	sprite.setPosition(drawP);
	window.draw(sprite);
}

void Entity::Move(Level* level, Vector2f accel, float elapsed)
{
	accel *= speed;
	accel += velocity * -16.0f;

	// Using the following equations of motion:

	// - p' = 1/2at^2 + vt + p.
	// - v' = at + v.
	// - a = specified by input.

	// Where a = acceleration, v = velocity, and p = position.
	// v' and p' denote new versions, while non-prime denotes old.

	// These are found by integrating up from acceleration to velocity. Use derivation
	// to go from position down to velocity and then down to acceleration to see how 
	// we can integrate back up.
	Vector2f delta = accel * 0.5f * Square(elapsed) + velocity * elapsed;
	velocity = accel * elapsed + velocity;

	colFlags = CF_NONE;

	Room* room = level->GetCurrentRoom();

	// Consider all other entities in this room except
	// this entity itself for collision.
	for (Entity* entity : room->GetEntities())
	{
		if (entity != this && !entity->IsPassable())
		{
			AABB bb = entity->BoundingBox();
			collides.push_back(bb);
		}
	}

	std::sort(collides.begin(), collides.end(), [this](auto a, auto b)
	{
		float distA = Distance2(a.center, position);
		float distB = Distance2(b.center, position);
		return distA < distB;
	});

	CollisionStep(this, delta, collides);
	collides.clear();
}
