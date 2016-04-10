#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include "Renderer.h"

Renderer::Renderer()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
	}
}

Renderer::~Renderer()
{
	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
	TTF_Quit();
}

bool Renderer::init()
{
	//Initialization flag
	bool success = true;

	//Create window
	gWindow = SDL_CreateWindow("My Game", 
								SDL_WINDOWPOS_UNDEFINED, 
								SDL_WINDOWPOS_UNDEFINED, 
								SCREEN_WIDTH, 
								SCREEN_HEIGHT, 
								SDL_WINDOW_SHOWN);
	if (gWindow == nullptr)
	{
		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		success = false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

	if (gRenderer == NULL)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		success = false;
	}

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		success = false;
	}

	//Initialize SDL_ttf
	if (TTF_Init() == -1)
	{
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}

	return success;
}

void Renderer::close()
{
	//Destroy window
	for (auto it : textures)
	{
		SDL_DestroyTexture(it.second);
	}
	textures.clear();

	TTF_CloseFont(gFont);
	gFont = nullptr;

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;
}

void Renderer::loadMedia(std::string name)
{
	if (textures.find(name) == textures.end())
	{
		SDL_Texture* texture = IMG_LoadTexture(gRenderer, name.c_str());
		textures.emplace(name, texture);
	}
}

void Renderer::update()
{
	//Update the surface
	//SDL_UpdateWindowSurface(gWindow);

	SDL_RenderPresent(gRenderer);
}

void Renderer::drawObject(const std::string name, const int xSrc, const int ySrc, const int xDst, const int yDst, const int height, const int width)
{
	//std::cerr << name << std::endl;

	loadMedia(name);

	SDL_Texture* texture = textures.find(name)->second;
	if (texture == nullptr)
	{
		std::cerr << "Unable to load image " << name.c_str() << " SDL Error: " << IMG_GetError() << std::endl;
	}

	SDL_Rect srcrect{ xSrc, ySrc, width, height };
	SDL_Rect dstrect{ xDst, yDst, width, height };

	if (width == 0 && height == 0)
	{
		SDL_RenderCopy(gRenderer, texture, nullptr, nullptr);
	}
	else
	{
		SDL_RenderCopy(gRenderer, texture, &srcrect, &dstrect);
	}
}

void Renderer::loadBackground(std::string name)
{
	SDL_RenderClear(gRenderer);
	loadMedia(name);
	drawObject(name, 0, 0, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH);
}

void Renderer::clear(int x, int y, int width, int height)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;

	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 0);
	SDL_RenderFillRect(gRenderer, &rect);
}

void Renderer::setTextureAlpha(std::string name, uint8_t alpha)
{
	auto it = textures.find(name);
	if (it != textures.end())
	{
		SDL_Texture* texture = it->second;
		if (texture != nullptr)
		{
			SDL_SetTextureAlphaMod(texture, alpha);
		}
	}
}

void Renderer::setTextureBlendModeAlpha(std::string name)
{
	auto it = textures.find(name);
	if (it != textures.end())
	{
		SDL_Texture* texture = it->second;
		if (texture != nullptr)
		{
			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		}
	}
}

void Renderer::setTextureBlendModeNone(std::string name)
{
	auto it = textures.find(name);
	if (it != textures.end())
	{
		SDL_Texture* texture = it->second;
		if (texture != nullptr)
		{
			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
		}
	}
}

void Renderer::setFontType(std::string name, int size)
{
	if (gFont != nullptr)
	{
		TTF_CloseFont(gFont);
		gFont = nullptr;
	}

	//Open the font
	gFont = TTF_OpenFont(name.c_str(), size);

	if (gFont == nullptr)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
	}
}

void Renderer::drawText(std::string message, int x, int y)
{
	SDL_Color color = { 0, 0, 0 };
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(gFont, message.c_str(), color); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

	SDL_Texture* Message = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage); //now you can convert it into a texture

	int w, h;
	TTF_SizeText(gFont, message.c_str(), &w, &h);

	SDL_Rect Message_rect; //create a rect
	Message_rect.x = x;  //controls the rect's x coordinate 
	Message_rect.y = y; // controls the rect's y coordinte
	Message_rect.w = w; // controls the width of the rect
	Message_rect.h = h; // controls the height of the rect

	//Mind you that (0,0) is on the top left of the window/screen, think a rect as the text's box, that way it would be very simple to understance

	//Now since it's a texture, you have to put RenderCopy in your game loop area, the area where the whole code executes

	clear(x, y, w, h);
	SDL_RenderCopy(gRenderer, Message, nullptr, &Message_rect);

	SDL_DestroyTexture(Message);
	SDL_FreeSurface(surfaceMessage);
}