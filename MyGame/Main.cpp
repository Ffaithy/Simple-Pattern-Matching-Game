#include <iostream>
#include <string>
#include <ctime>
#include "SDL.h"
#include "Game.h"
#include "Timer.h"

const int MS_PER_UPDATE = 2000;

const int SCREEN_FPS = 60;
const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;

using namespace std;

int main(int, char**)
{
	Game::instance().init();

	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	//The frames per second timer
	Timer fpsTimer;

	//The frames per second cap timer
	Timer capTimer;

	Board& board = Game::instance().getBoard();
	board.generate();

	//Start counting frames per second
	int countedFrames = 0;
	fpsTimer.start();

	while (!quit)
	{
		//Start cap timer
		capTimer.start();

		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else
				if (e.type == SDL_MOUSEBUTTONUP)
				{
					//Get mouse position
					int x, y;
					SDL_GetMouseState(&x, &y);

					Game::instance().handleInput(x, y);
				}
		}

		//Calculate and correct fps
		float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
		if (avgFPS > 2000000)
		{
			avgFPS = 0;
		}

		//std::cerr << avgFPS << std::endl;

		Game::instance().update();

		++countedFrames;

		//If frame finished early
		int frameTicks = capTimer.getTicks();
		if (frameTicks < SCREEN_TICK_PER_FRAME)
		{
			//Wait remaining time
			SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
		}
	}

	Game::instance().close();

	return 0;
}