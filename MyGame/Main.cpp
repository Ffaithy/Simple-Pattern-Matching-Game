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

	//Game loop
	//time_t previous;
	//time(&previous);
	//double lag = 0;

	//The frames per second timer
	LTimer fpsTimer;

	//The frames per second cap timer
	LTimer capTimer;

	Renderer& renderer = Game::instance().getRenderer();
	
	renderer.setFontType("./fonts/ANUDI.ttf", 24);

	Board& board = Game::instance().getBoard();
	board.generate();
	//board.render();
	//renderer.update();

	//Start counting frames per second
	int countedFrames = 0;
	fpsTimer.start();

	while (!quit)
	{
		//time_t current;
		//time(&current);
		//double elapsed = difftime(current, previous);
		//previous = current;
		//lag += elapsed;

		//TODO
		//processInput();
		//Handle events on queue

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
					board.handleInput(x, y);
					//renderer.update();
				}
		}

		//Calculate and correct fps
		float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
		if (avgFPS > 2000000)
		{
			avgFPS = 0;
		}

		//std::cerr << avgFPS << std::endl;

		if (Game::instance().getStop() == false)
		{
			board.render();
		}
		SDL_Color color = { 0, 0, 0 };
		renderer.update();
		++countedFrames;

		//If frame finished early
		int frameTicks = capTimer.getTicks();
		if (frameTicks < SCREEN_TICK_PER_FRAME)
		{
			//Wait remaining time
			SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
		}

		//while (lag > MS_PER_UPDATE)
		//{
			//TODO UPDATE GAME WORLD
			//update();
			//lag -= MS_PER_UPDATE;
		//}

		//TODO
		//render(lag/MS_PER_UPDATE);
		//Apply the image
		

		//Update the surface
		
	}

	Game::instance().close();

	return 0;
}