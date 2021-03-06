#include "Stdafx.h"
#include "Player.h"
#include "Level.h"
#include "Generator.h"
#include "Utils.h"
#include "Assets.h"

int main(int, char**)
{
	uint32_t seed = (uint32_t)time(NULL);
	srand(seed);

	Print("Level seed: %u\n", seed);

	int width = 1024, height = 592;

	RenderWindow window(VideoMode(width, height), "Catacombs of the Dark Emperor");
	window.setVerticalSyncEnabled(true);

	Renderer rend(&window);

	sounds = new SoundEffects();

	Level* level = new Level();

	LevelGenerator* generator = new LevelGenerator();
	generator->Build(level, true);

	GameState& state = getGameState();

	// Tracks frame time so we can use it for updating entities
	// in a framerate independent manner.
	Clock clock;
	Time elapsed;

	state.gameStart = true;

	// Game loop.
	while (window.isOpen())
	{
		Event event;

		// Loop through window events and respond to them. 
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
				setTopScore();
			}
			else if (event.type == Event::KeyPressed)
			{
				switch (event.key.code)
				{
					case Keyboard::F1:
						state.showOutlines = !state.showOutlines;
						break;

					case Keyboard::F2:
						state.showPathGrid = !state.showPathGrid;
						break;

					case Keyboard::Escape:
						state.paused = !state.paused;
						state.manualPause = !state.manualPause;
						break;
				}
			}
		}

		// Clear the screen to a black color.
		window.clear(Color::Black);

		if (!state.paused)
		{
			level->Update(elapsed.asSeconds());

			if (state.newLevel || state.restart)
			{
				// Reset the floor count on game restart.
				if (state.restart)
					state.floor = 0;

				level->Destroy();
				delete level;
				level = new Level();

				// If restart is true, this is considered the
				// first level, otherwise it isn't.
				generator->Build(level, state.restart);

				state.newLevel = false;
				state.restart = false;
			}
		}
		
		level->Draw(rend);

		rend.WriteToWindow();
		window.display();

		elapsed = clock.restart();
	}

	return 0;
}
