
#include "Stdafx.h"
#include "Room.h"

bool Room::TrySetObstacle(int x, int y)
{
	Vector2i p = Vector2i(x, y);

	auto it = obstacles.find(p);

	if (it == obstacles.end())
	{
		obstacles.insert(p);
		return true;
	}

	return false;
}

void Room::Update(Level* level, float elapsed)
{
	for (Entity* entity : entities)
		entity->Update(level, elapsed);
}

void Room::Draw(Renderer& rend)
{
	for (Entity* entity : entities)
	{
		if (entity->Visible())
			entity->Draw(rend);
	}
}
