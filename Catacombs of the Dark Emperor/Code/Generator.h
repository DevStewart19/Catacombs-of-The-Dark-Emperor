#pragma once

#include "Level.h"
#include "Player.h"

// Base class for a level generator.
class Generator
{
	virtual void Build(Level*, bool) {}
};

struct PathDirection
{
	DoorType doorType;
	int dir;
	int doorP;
};

struct BranchStart
{
	Vector2i start;
	Vector2i dir;
	PathDirection pd;
};

struct LevelGenerator : public Generator
{
	Player* player;

	Room* spawnRoom;
	PathDirection spawnPrevDir;

	Vector2iSet roomsAdded;

	// Stores the first room for each branching path.
	std::vector<BranchStart> branches;

	void Build(Level*, bool firstLevel) override;

private:
	void GenerateBossRoom(Level* level);
	void GeneratePath(Level* level, Vector2i start, Vector2i end, PathDirection prevDir, bool mainPath = false);
	void SpawnPlayer();
};
