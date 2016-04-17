#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <unordered_map>
#include <cstdint>
#include <SDL.h>
#include <SDL_ttf.h>
#include "IRenderable.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool init();
	void close();
	void update();
	void clear(int x, int y, int width, int height);

	//Texture manipulation 
	void loadTexture(const std::string& name);
	void setTextureAlpha(const std::string& name, uint8_t alpha);
	void setTextureBlendModeAlpha(const std::string& name);
	void setTextureBlendModeNone(const std::string& name);
	void cleanTextures();
	
	//Object and text rendering
	void drawBackground(const std::string& name);
	void drawObject(const ObjRenderable& obj, bool clear = false);
	void drawText(const TextRenderable& txt, bool clear = false);

	void setFontType(const std::string&, int size = DEFAULT_FONT_SIZE);

private:
	static const int SCREEN_WIDTH;
	static const int SCREEN_HEIGHT;
	static const int DEFAULT_FONT_SIZE;
	static const SDL_Color DEFAULT_TEXT_COLOR;
	static const SDL_Color DEFAULT_CEAR_COLOR;

	SDL_Window* sWindow;
	SDL_Renderer* sRenderer;
	TTF_Font* sFont;
	std::unordered_map<std::string, SDL_Texture*> textures;
};

#endif