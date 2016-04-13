#include "Cell.h"
#include "Game.h"

const std::string Cell::NAME_PREFIX = "./assets/items/ball_";
const std::string Cell::NAME_SUFIX = ".png";
const std::string Cell::SELECTOR = "./assets/items/selector.png";

const int Cell::DEFAULT_WIDTH = 60;
const int Cell::DEFAULT_HEIGHT = 60;
const int Cell::EMPTY = -1;

const int Cell::OFFSET_Y = 100;
const int Cell::OFFSET_X = 50;

const uint8_t Cell::MAX_ALPHA = 255;
const uint8_t Cell::INIT_ALPHA = 240;
const uint8_t Cell::ALPHA_DIFF_STEP = 8;

//Render the current object on the screen
void Cell::render(int step, int numSteps) const
{
	Renderer& renderer = Game::instance().getRenderer();

	//if it's an explosion, modify the alpha
	if (explode)
	{
		if (step == 0)
		{
			renderer.setTextureAlpha(getName(), INIT_ALPHA);
			renderer.setTextureBlendModeAlpha(getName());
		}
		else
			if (step == numSteps)
			{
				renderer.setTextureBlendModeNone(getName());
				explode = false;
				type = EMPTY;
			}
			else
			{
				renderer.setTextureBlendModeAlpha(getName());
				renderer.setTextureAlpha(getName(), INIT_ALPHA - step * ALPHA_DIFF_STEP);
			}
	}
	else
	{
		renderer.setTextureBlendModeNone(getName());
	}

	if (type != EMPTY)
	{
		//draw image
		renderer.drawObject(ObjRenderable{ getName(), 0, 0, OFFSET_X + x, OFFSET_Y + y, mWidth, mHeight }, true);

		//render selection
		if (isSelected())
		{
			renderer.drawObject(ObjRenderable{ Cell::SELECTOR, 0, 0, OFFSET_X + x, OFFSET_Y + y, mWidth, mHeight }, false);
		}
	}
}


void Cell::renderWithDynamicParams(int xSrc, int ySrc, int xDst, int yDst, int width, int height) const
{
	Renderer& renderer = Game::instance().getRenderer();
	renderer.drawObject(ObjRenderable{ getName(), xSrc, ySrc, OFFSET_X + xDst, OFFSET_Y + yDst, width, height });
}