#ifndef IRENDERABLE_H
#define IRENDERABLE_H

#include <string>

class ObjRenderable
{
	friend class Renderer;

public:
	ObjRenderable() : name(""), xSrc{ 0 }, ySrc{ 0 }, xDst{ 0 }, yDst{ 0 }, width{ 0 }, height{ 0 } {};

	ObjRenderable(const std::string& pname, int pxSrc, int pySrc, int pxDst, int pyDst, int pwidth, int pheight) :
				name(pname), xSrc{ pxSrc }, ySrc{ pySrc }, xDst{ pxDst }, yDst{ pyDst }, width{ pwidth }, height{ pheight } {};

	const std::string& getName() const { return name; };

private:
	std::string name;
	int xSrc;
	int ySrc;
	int xDst;
	int yDst;
	int width;
	int height;

};

class TextRenderable
{
	friend class Renderer;

public:
	TextRenderable() = delete;
	TextRenderable(const std::string& ptext, int px, int py, int pwidth, int pheight, int psize) : 
					text(ptext), x{ px }, y{ py }, width{ pwidth }, height{ pheight }, size{ psize } {};

private:
	std::string text;
	int x;
	int y;
	int width;
	int height;
	int size;
};

#endif