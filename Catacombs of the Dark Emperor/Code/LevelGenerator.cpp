#include "Stdafx.h"
#include "Generator.h"
#include "Generator.h"
#include "Player.h"
#include "Tiles.h"
#include "Chest.h"

static Room* CreateRoom(Level* level, int x, int y, bool* dirs, int prevDir)
{
	Room* room = level->GetOrCreateRoom(x, y);

	// Add corner tiles.
	AddEntity<WallUpRight>(room, Room::Width - 2, 0);
	AddEntity<WallDownRight>(room, Room::Width - 2, Room::Height - 2);
	AddEntity<WallUpLeft>(room, 0, 0);
	AddEntity<WallDownLeft>(room, 0, Room::Height - 2);

	int doorX = Room::Width / 2;
	int doorY = Room::Height / 2;

	// Top row of walls.
	if (dirs[UP] || prevDir == DOWN)
	{
		AddRect<WallUp>(room, 2, 0, doorX - 2, 0);
		AddRect<WallUp>(room, doorX + 2, 0, Room::Width - 3, 0);

		AddEntity<DoorUp>(room, doorX - 1, 0);

		// Add barriers so that some parts of the room sprite 
		// are solid. Barriers are invisible colliders.
		AddEntity<Barrier>(room, doorX - 1, 0, 0.6f, 2.0f);
		AddEntity<Barrier>(room, doorX + 1, 0, 0.6f, 2.0f, 0.4f, 0.0f);
	}
	else AddRect<WallUp>(room, 2, 0, Room::Width - 3, 0);

	// Add bottom row of walls.
	if (dirs[DOWN] || prevDir == UP)
	{
		AddRect<WallDown>(room, 2, Room::Height - 2, doorX - 2, Room::Height - 2);
		AddRect<WallDown>(room, doorX + 2, Room::Height - 2, Room::Width - 3, Room::Height - 2);

		AddEntity<DoorDown>(room, doorX - 1, Room::Height - 2);

		AddEntity<Barrier>(room, doorX - 1, Room::Height - 2, 0.6f, 2.0f);
		AddEntity<Barrier>(room, doorX + 1, Room::Height - 2, 0.6f, 2.0f, 0.4f, 0.0f);
	}
	else AddRect<WallDown>(room, 2, Room::Height - 2, Room::Width - 3, Room::Height - 2);

	// Add left row of walls.
	if (dirs[LEFT] || prevDir == RIGHT)
	{
		AddRect<WallLeft>(room, 0, 2, 0, doorY - 2);
		AddRect<WallLeft>(room, 0, doorY + 2, 0, Room::Height - 3);

		AddEntity<DoorLeft>(room, 0, doorY - 1);

		// TODO: Barriers.
	}
	else AddRect<WallLeft>(room, 0, 2, 0, Room::Height - 3);

	// Add right row of walls.
	if (dirs[RIGHT] || prevDir == LEFT)
	{
		AddRect<WallRight>(room, Room::Width - 2, 2, Room::Width - 2, doorY - 2);
		AddRect<WallRight>(room, Room::Width - 2, doorY + 2, Room::Width - 2, Room::Height - 3);

		AddEntity<DoorRight>(room, Room::Width - 2, doorY - 1);

		// TODO: Barriers.
	}
	else AddRect<WallRight>(room, Room::Width - 2, 2, Room::Width - 2, Room::Height - 3);

	// Add floor tiles.
	AddRect<FloorTile>(room, 2, 2, Room::Width - 3, Room::Height - 3);

	return room;
}

void LevelGenerator::GeneratePath(Level* level, Vector2i start, Vector2i end, int prevDir, bool mainPath)
{
	// Store directions as vectors for convenience.
	Vector2i vecDirs[5];
	vecDirs[UP] = Vector2i(0, 1);
	vecDirs[DOWN] = Vector2i(0, -1);
	vecDirs[LEFT] = Vector2i(-1, 0);
	vecDirs[RIGHT] = Vector2i(1, 0);

	Vector2i cur = start;

	bool initial = true;
	Room* initialRoom = nullptr;

	// Loop until we reach the end room.
	// When we do, the loop will exit via a break.
	while (true)
	{
		int diffX = end.x - cur.x;
		int diffY = end.y - cur.y;

		bool dirs[4] = {};
		int choice = 0;
		bool noValidChoices = false;

		if (cur != end)
		{
			dirs[UP] = diffY > 0;
			dirs[DOWN] = diffY < 0;
			dirs[LEFT] = diffX < 0;
			dirs[RIGHT] = diffX > 0;

			int tries = 32;
			bool validDir = false;

			do
			{
				choice = randomInRange(0, 3);

				Vector2i nextP = cur + vecDirs[choice];
				auto it = roomsAdded.find(nextP);

				if (dirs[choice] && it == roomsAdded.end())
					validDir = true;

				// Stop if we tried too many times.
				// Assume there's no possible direction to go.
				if (--tries == 0)
				{
					noValidChoices = true;
					break;
				}
			} 
			while (!validDir);

			// Ensure only one direction is chosen.
			for (int i = 0; i < 4; ++i)
			{
				if (i != choice)
					dirs[i] = false;
			}

			if (mainPath && randomInRange(0, 2) == 0)
			{
				int dir = randomInRange(0, 3);

				// Don't generate a branching path in the direction
				// we're already going on the main path, or in the
				// direction we came from.
				if (!dirs[dir] && prevDir != GetOppositeDir(dir))
				{
					BranchStart branch = { cur, vecDirs[dir], dir };
					branches.push_back(branch);
					dirs[dir] = true;
				}
			}
		}

		Room* room = CreateRoom(level, cur.x, cur.y, dirs, prevDir);
		roomsAdded.insert(cur);

		// Copy current directions to the previous directions.
		prevDir = choice;

		if (initial)
		{
			if (mainPath)
			{
				player->SetPosition(8, 5);
				room->AddEntity(player);

				level->SetCurrentRoom(room);
			}

			initialRoom = room;
			initial = false;
		}
		else
		{
			// 50% (for now) chance of spawning a red chest in this room.
			if (randomUnit() <= 0.9f)
			{
				Chest* chest = new Chest();

				chest->spawn(player, 1);
				room->AddEntity(chest);
			}
			// 10% (for now) chance of spawning a blue chest in this room.
			if (randomUnit() <= 0.9f)
			{
				Chest* chest = new Chest();

				chest->spawn(player, 2);
				room->AddEntity(chest);
			}
		}

		if (cur == end || noValidChoices) break;
		else cur += vecDirs[choice];
	}
}

void LevelGenerator::Build(Level* level)
{
	player = new Player();

	// The number of rooms away from the origin our ending
	// room can be in either axis.
	int radius = 5;

	Vector2i start = Vector2i(0, 0);

	// Ensure the room is at the edge of the area to consider.
	int x = randomInRange(0, 1) == 0 ? radius : randomInRange(-radius, radius);
	int y = x == radius ? randomInRange(-radius, radius) : radius;

	Vector2i end = Vector2i(x, y);

	GeneratePath(level, start, end, -1, true);

	for (BranchStart branch : branches)
	{
		do
		{
			int endX = randomInRange(-radius, radius);
			int endY = randomInRange(-radius, radius);

			end = Vector2i(endX, endY);
		} 
		while (roomsAdded.find(end) != roomsAdded.end());

		// Make sure the previous direction is set for the
		// first room of the path, so it connects back
		// to the main path.
		int prevDir = branch.dirIndex;

		GeneratePath(level, branch.start + branch.dir, end, prevDir, false);
	}
}
