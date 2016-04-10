#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <unordered_map>
#include <cstdint>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
typedef struct _TTF_Font TTF_Font;

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool init();
	void loadMedia(std::string name);
	void close();
	void update();
	void clear(int x, int y, int width, int height);
	void drawObject(const std::string name, const int xSrc = 0, const int ySrc = 0, const int xDst = 0, const int yDst = 0, const int height = 0, const int width = 0);
	void setTextureAlpha(std::string name, uint8_t alpha);
	void setTextureBlendModeAlpha(std::string name);
	void setTextureBlendModeNone(std::string name);
	void setFontType(std::string, int size);
	void drawText(std::string message, int x, int y);
	void loadBackground(std::string name);

public:
	static const int SCREEN_WIDTH = 640;
	static const int SCREEN_HEIGHT = 640;

private:
	SDL_Window* gWindow;
	SDL_Renderer* gRenderer;
	TTF_Font* gFont;
	std::unordered_map<std::string, SDL_Texture*> textures;
};

#endif