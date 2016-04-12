#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include "Renderer.h"

const int Renderer::SCREEN_WIDTH = 640;
const int Renderer::SCREEN_HEIGHT = 640;
const int Renderer::DEFAULT_FONT_SIZE = 24;
const SDL_Color Renderer::DEFAULT_TEXT_COLOR{ 0, 0, 0 };
const SDL_Color Renderer::DEFAULT_CEAR_COLOR{ 255, 255, 255 };

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
	sWindow = SDL_CreateWindow("My Game", 
								SDL_WINDOWPOS_UNDEFINED, 
								SDL_WINDOWPOS_UNDEFINED, 
								SCREEN_WIDTH, 
								SCREEN_HEIGHT, 
								SDL_WINDOW_SHOWN);
	if (sWindow == nullptr)
	{
		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		success = false;
	}

	//Create renderer
	sRenderer = SDL_CreateRenderer(sWindow, -1, SDL_RENDERER_ACCELERATED);

	if (sRenderer == nullptr)
	{
		std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
		success = false;
	}

	//Initialize SDL_img
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
		success = false;
	}

	//Initialize SDL_ttf
	if (TTF_Init() == -1)
	{
		std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
		success = false;
	}

	return success;
}

void Renderer::close()
{
	//Destroy textures
	for (auto it : textures)
	{
		SDL_DestroyTexture(it.second);
	}
	textures.clear();

	//Destroy font
	TTF_CloseFont(sFont);
	sFont = nullptr;

	//Destroy window
	SDL_DestroyWindow(sWindow);
	sWindow = nullptr;

	//Destroy renderer
	SDL_DestroyRenderer(sRenderer);
	sRenderer = nullptr;
}

void Renderer::update()
{
	//Update the surface
	SDL_RenderPresent(sRenderer);
}

void Renderer::clear(int x, int y, int width, int height)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;

	SDL_SetRenderDrawColor(sRenderer, DEFAULT_CEAR_COLOR.r, DEFAULT_CEAR_COLOR.g, DEFAULT_CEAR_COLOR.b, 0);
	SDL_RenderFillRect(sRenderer, &rect);
}

void Renderer::loadTexture(const std::string& name)
{
	if (textures.find(name) == textures.end())
	{
		SDL_Texture* texture = IMG_LoadTexture(sRenderer, name.c_str());
		textures.emplace(name, texture);
	}
}

void Renderer::setTextureAlpha(const std::string& name, uint8_t alpha)
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

void Renderer::setTextureBlendModeAlpha(const std::string& name)
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

void Renderer::setTextureBlendModeNone(const std::string& name)
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

void Renderer::drawBackground(const std::string& name)
{
	SDL_RenderClear(sRenderer);
	loadTexture(name);

	const ObjRenderable background {name, 0, 0, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH};
	drawObject(background);
}

void Renderer::drawObject(const ObjRenderable& obj, bool clear)
{
	if (clear)
	{
		this->clear(obj.xDst, obj.yDst, obj.width, obj.height);
	}

	loadTexture(obj.name);

	auto it = textures.find(obj.name);
	if (it != textures.end())
	{
		SDL_Texture* texture = it->second;
		if (texture == nullptr)
		{
			std::cerr << "Unable to load image " << obj.name.c_str() << " SDL Error: " << IMG_GetError() << std::endl;
			return;
		}

		SDL_Rect srcrect{ obj.xSrc, obj.ySrc, obj.width, obj.height };
		SDL_Rect dstrect{ obj.xDst, obj.yDst, obj.width, obj.height };

		SDL_RenderCopy(sRenderer, texture, &srcrect, &dstrect);
	}
}

void Renderer::drawText(const TextRenderable& txt, bool clear)
{

	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(sFont, (txt.text + txt.addText).c_str(), DEFAULT_TEXT_COLOR);
	SDL_Texture* message = SDL_CreateTextureFromSurface(sRenderer, surfaceMessage); 

	int w, h;
	TTF_SizeText(sFont, txt.text.c_str(), &w, &h);

	//Clear the area
	this->clear(txt.x, txt.y, w, h);
	SDL_Rect rectMessage {txt.x, txt.y, w, h}; 
	SDL_RenderCopy(sRenderer, message, nullptr, &rectMessage);

	SDL_DestroyTexture(message);
	SDL_FreeSurface(surfaceMessage);
}

void Renderer::setFontType(const std::string& name, int size)
{
	if (sFont != nullptr)
	{
		TTF_CloseFont(sFont);
		sFont = nullptr;
	}

	//Open the font
	sFont = TTF_OpenFont(name.c_str(), size);

	if (sFont == nullptr)
	{
		std::cerr << "Failed to load lazy font! SDL_ttf Error: " << TTF_GetError() << std::endl;
	}
}
