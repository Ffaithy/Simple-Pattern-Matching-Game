#include <iostream>
#include <string>
#include <ctime>
#include "SDL.h"
#include "Game.h"
#include "Timer.h"

const int SCREEN_FPS = 60;
const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;

using namespace std;

int main(int, char**)
{
	Game::instance().init();

	//Quit event
	bool quit = false;

	//Event handler
	SDL_Event e;

	//The frames per second timer
	Timer fpsTimer;

	//The frames per loop timer
	Timer loopTimer;

	//Start counting frames per second
	int countedFrames = 0;
	fpsTimer.start();

	while (!quit)
	{
		//Start loop timer
		loopTimer.start();

		while (SDL_PollEvent(&e) != 0)
		{
			//User quit the game
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

		//Calculate fps
		float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
		std::cerr << avgFPS << std::endl;

		//Update game state & render 
		Game::instance().update();

		//Update frame counter
		++countedFrames;

		//If frame finished early
		int frameTicks = loopTimer.getTicks();
		if (frameTicks < SCREEN_TICK_PER_FRAME)
		{
			//Wait remaining time
			SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
		}
	}

	Game::instance().close();

	return 0;
}