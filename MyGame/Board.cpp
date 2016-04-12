#include "Board.h"
#include "Game.h"
#include "Event.h"
#include <ctime>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

const std::string Board::Cell::NAME_PREFIX = "./assets/items/ball_";
const std::string Board::Cell::NAME_SUFIX = ".png";
const std::string Board::Cell::SELECTOR = "./assets/items/selector.png";

const int Board::Cell::DEFAULT_WIDTH = 60;
const int Board::Cell::DEFAULT_HEIGHT = 60;
const int Board::Cell::EMPTY = -1;

int Board::Cell::width = Board::Cell::DEFAULT_WIDTH;
int Board::Cell::height = Board::Cell::DEFAULT_HEIGHT;

int Board::STEP = 0;
const int Board::OFFSET_Y = 100;
const int Board::OFFSET_X = 50;
Board::State Board::STATE = State::STATE_NONE;

void Board::Cell::render() const
{
	Renderer& renderer = Game::instance().getRenderer();

	//first clear
	//renderer.clear(OFFSET_X + y, OFFSET_Y + x, Cell::getWidth(), Cell::getHeight());

	if (explode)
	{
		if (Board::STEP == 0)
		{
			renderer.setTextureAlpha(getName(), Board::INIT_ALPHA);
			renderer.setTextureBlendModeAlpha(getName());
		}
		else
			if (Board::STEP == NUM_EXPLODE_STEPS)
			{
				renderer.setTextureBlendModeNone(getName());
				explode = false;
				type = EMPTY;
			}
			else
			{
				renderer.setTextureBlendModeAlpha(getName());
				renderer.setTextureAlpha(getName(), Board::INIT_ALPHA - Board::STEP * Board::EXPLODE_DIFF_STEP);
			}
	}
	else
	{
		renderer.setTextureBlendModeNone(getName());
	}

	if (type != EMPTY)
	{
		//draw image
		renderer.drawObject(ObjRenderable{ getName(), 0, 0, OFFSET_X + y, OFFSET_Y + x, Cell::getWidth(), Cell::getHeight() }, true);

		//render selection
		if (isSelected())
		{
			renderer.drawObject(ObjRenderable{ Cell::SELECTOR, 0, 0, OFFSET_X + y, OFFSET_Y + x, Cell::getWidth(), Cell::getHeight() }, false);
		}
	}

}

Board::Board(const std::vector<int>& probes, int width, int height) :
		probes(probes),
		width(width),
		height(height),
		selected0(nullptr),
		selected1(nullptr)
{
	initRenderer();
};

void Board::generate()
{
	int sz = probes.size();
	srand(time(0));

	generator.resize(width);

	// Set up sizes. (HEIGHT x WIDTH)
	cells.resize(height);
	for (int i = 0; i < height; ++i)
		cells[i].resize(width);

	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
		{
			int prevX = -1;
			int prevprevX = -1;
			bool checkSimX = false;

			int prevY = -1;
			int prevprevY = -1;
			bool checkSimY = false;

			if (j >= 2)
			{
				prevprevX = cells[i][j - 2].getType();
				prevX= cells[i][j - 1].getType();
				if (prevprevX == prevX)
					checkSimX = true;
			}

			if (i >= 2)
			{
				prevprevY = cells[i - 2][j].getType();
				prevY = cells[i - 1][j].getType();
				if (prevprevY == prevY)
					checkSimY = true;
			}

			int number = -1;
			do
			{
				number = rand() % sz;
			} while ((checkSimX && (number == prevX)) || (checkSimY && (number == prevY)) );

			cells[i][j] = { probes[number], i * Cell::getWidth(), j * Cell::getHeight() };
		}
}

void Board::initRenderer() const
{
	Renderer& renderer = Game::instance().getRenderer();
	for (unsigned int i = 0; i < probes.size(); ++i)
	{
		renderer.loadTexture(Cell::getName(probes[i]));
	}

	renderer.loadTexture(Cell::SELECTOR);
}

void Board::update()
{
	Renderer& renderer = Game::instance().getRenderer();
	if (STATE == State::STATE_SWAP)
	{
		if (STEP < NUM_SWAP_STEPS)
		{
			STEP++;
			continueSwapAnimation();
		}
		else
		{
			endSwap();
			STEP = 0;
			//STATE = State::STATE_NONE;

			EventMove event;
			Game::instance().onNotify(&event);
		}
		return;
	}

	if (STATE == State::STATE_INVALID_SWAP)
	{
		if (STEP < NUM_SWAP_STEPS)
		{
			STEP++;
			continueSwapAnimation();
		}
		else
		{
			//endSwap();
			STEP = 0;
			STATE = State::STATE_NONE;
			selected0->setDirection(Direction::NONE);
			selected1->setDirection(Direction::NONE);
			selected0 = nullptr;
			selected1 = nullptr;
		}
		return;
	}

	if (STATE == State::STATE_FILL)
	{
		if (STEP < NUM_SWAP_STEPS)
		{
			STEP++;
			animateFall();
		}
		else
		{
			if (!endFall())
			{
				STEP = 0;

				if (boardHasMatches())
				{
					STATE = State::STATE_EXPLODE;
				}
				else
				{
					STATE = State::STATE_NONE;
					EventCheckState ev;
					Game::instance().onNotify(&ev);
					
				}
			}
			else
			{
				STEP = 0;
				startFall();
			}
		}

		return;
	}

	/*for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			cells[i][j].render();
		}*/

	//update state
	if (STATE == State::STATE_EXPLODE)
	{
		if (STEP < NUM_EXPLODE_STEPS)
			Board::STEP++;
		else
		{
			//STATE = State::STATE_NONE;
			STATE = State::STATE_FILL;
			Board::STEP = 0;
			startFall();
		}
	}
}

void Board::render()
{
	renderGenerator();

	for (int i = 0; i < width; ++i)
	for (int j = 0; j < height; ++j)
	{
	cells[i][j].render();
	}

}

void Board::handleInput(int x, int y)
{
	Renderer& renderer = Game::instance().getRenderer();
	int cellY = (x - OFFSET_X) / Cell::getWidth();
	int cellX = (y - OFFSET_Y)/ Cell::getHeight();
	std::cerr << cellX << " " << cellY << std::endl;

	if (selected0 == nullptr)
	{
		selected0 = &cells[cellX][cellY];
		cells[cellX][cellY].setSelected(true);
		cells[cellX][cellY].render();
		std::cerr << "SELECTION " << cellX << " " << cellY << std::endl;
	}
	else
	{
		//int oldX = selected0->getX();
		//int oldY = selected0->getY();

		int oldX = selected0->getRow();
		int oldY = selected0->getCol();

		if (std::abs(cellX - oldX) + std::abs(cellY - oldY) > 1)
		{
			//move selection

			//clear old selection
			cells[oldX][oldY].setSelected(false);
			cells[oldX][oldY].render();

			//render new selection
			selected0 = &cells[cellX][cellY];
			cells[cellX][cellY].setSelected(true);
			cells[cellX][cellY].render();

		}
		else
			if (std::abs(cellX - oldX) + std::abs(cellY - oldY) == 0)
			{
				//clear old selection
				cells[cellX][cellY].setSelected(false);
				cells[cellX][cellY].render();
				selected0 = nullptr;
			}
			else
			//if (selected->getType() != cells[cellX][cellY].getType())
			{
				selected1 = &cells[cellX][cellY];

				//clear old selection
				cells[oldX][oldY].setSelected(false);
				
				//swap
				beginSwapAnimation();
				
				/*//swap
				std::swap(cells[oldX][oldY], cells[cellX][cellY]);
				
				cells[oldX][oldY].setX(oldX);
				cells[oldX][oldY].setY(oldY);
				cells[cellX][cellY].setX(cellX);
				cells[cellX][cellY].setY(cellY);

				//draw the changes
				//cells[oldX][oldY].render();
				//cells[cellX][cellY].render();
			
				bool v = checkSwap();
				std::string s = v ? "true" : "false";
				std::cerr << s << std::endl;

				if (v)
				{
					computeLine(oldX);
					if (oldX != cellX)
						computeLine(cellX);
					computeCol(oldY);
					if (oldY != cellY)
						computeCol(cellY);
					update();
				}

				selected0 = nullptr;
				selected1 = nullptr;

				render();*/
			}
	}
}

bool Board::isPieceInARowMatch(int initX, int initY, int type) const
{
	int x = initX;
	int y = initY;
	int count = 1;
	bool valid = false;

	//check line
	y--;
	while (y >= 0 && cells[x][y].getType() == type)
	{
		count++;
		if (count >= NUM_MATCHES)
		{
			return true;
		}
		y--;
	}

	//reset y
	y = initY;
	y++;
	while (y < width && cells[x][y].getType() == type)
	{
		count++;
		if (count >= NUM_MATCHES)
		{
			return true;
		}
		y++;
	}

	return valid;
}

bool Board::isPieceInAColMatch(int initX, int initY, int type) const
{
	int x = initX;
	int y = initY;
	int count = 1;
	bool valid = false;

	//check line
	x--;
	while (x >= 0 && cells[x][y].getType() == type)
	{
		count++;
		if (count >= NUM_MATCHES)
		{
			return true;
		}
		x--;
	}

	//reset y
	x = initX;
	x++;
	while (x < height && cells[x][y].getType() == type)
	{
		count++;
		if (count >= NUM_MATCHES)
		{
			return true;
		}
		x++;
	}

	return valid;
}

bool Board::isSwapValid() const
{
	//check selected0 and selected1
	if (selected0->getDirection() == Direction::LEFT || selected0->getDirection() == Direction::RIGHT)
	{
		return (isPieceInAColMatch(selected0->getRow(), selected0->getCol(), selected0->getType()) || isPieceInAColMatch(selected1->getRow(), selected1->getCol(), selected1->getType()));
	}

	return (isPieceInARowMatch(selected0->getRow(), selected0->getCol(), selected0->getType()) ||
		isPieceInARowMatch(selected1->getRow(), selected1->getCol(), selected1->getType()));
	
}

bool Board::rowHasMatches(int l)
{
	std::cerr << "rowHasMatches " << l << std::endl;

	bool hasMatch = false;
	int count = 1;
	int startIdx = 0; 
	int finalIdx = 1;
	int type = cells[l][0].getType();
	while (finalIdx < width)
	{
		if (type == cells[l][finalIdx].getType())
		{
			count++;

			if (finalIdx < width - 1)
			{
				finalIdx++;
				continue;
			}
		}

		if (count >= NUM_MATCHES && type != Cell::EMPTY)
		{
			for (int i = startIdx; i < ((type == cells[l][finalIdx].getType() && (finalIdx == width - 1)) ? (finalIdx + 1) : finalIdx); ++i)
			{
				std::cerr << "EXPLODE [ " << l << " ][ " << i << " ]" << std::endl;
				cells[l][i].setExplode(true);
			}
			linesToExplode.insert(l);
				
			std::cerr << "INSERTING LINE " << l << std::endl;

			hasMatch = true;
		}

		count = 1;
		type = cells[l][finalIdx].getType();
		startIdx = finalIdx;

		finalIdx++;
	}

	return hasMatch;
}

bool Board::colHasMatches(int c)
{
	std::cerr << "colHasMatches " << c << std::endl;

	bool hasMatch = false;
	int count = 1;
	int startIdx = 0;
	int finalIdx = 1;
	int type = cells[0][c].getType();
	while (finalIdx < height)
	{
		if (type == cells[finalIdx][c].getType())
		{
			count++;

			if (finalIdx < height - 1)
			{
				finalIdx++;
				continue;
			}
		}

		if (count >= NUM_MATCHES && type != Cell::EMPTY)
		{
			for (int i = startIdx; i < ((type == cells[finalIdx][c].getType() && (finalIdx == height - 1)) ? (finalIdx + 1) : finalIdx); ++i)
			{
				std::cerr << "EXPLODE [ " << i << " ][ " << c << " ]" << std::endl;
				cells[i][c].setExplode(true);
			}
			colsToExplode.insert(c);

			std::cerr << "INSERTING COL " << c << std::endl;

			hasMatch = true;
		}

		count = 1;
		type = cells[finalIdx][c].getType();
		startIdx = finalIdx;

		finalIdx++;
	}

	return hasMatch;
}

/*void Board::update()
{
	int points = 0;

	std::cerr << "UPDATE" << std::endl;
	for (auto it : linesToExplode)
	{
		for (int i = 0; i < width; ++i)
			if (cells[it][i].isExplode())
			{
				points++;
				std::cerr << "EXPLODE !!!! " << it << " " << i << std::endl;
				//cells[it][i].setType(Cell::EMPTY);
				STATE = State::STATE_EXPLODE;
				Board::STEP = 0;
			}
	}
	linesToExplode.clear();

	for (auto it : colsToExplode)
	{
		for (int i = 0; i < height; ++i)
			if (cells[i][it].isExplode())
			{
				points++;
				std::cerr << "EXPLODE !!!! " << i << " " << it << std::endl;
				//cells[i][it].setType(Cell::EMPTY);
				STATE = State::STATE_EXPLODE;
				Board::STEP = 0;
			}
	}
	colsToExplode.clear();

	EventScore e(points);
	Game::instance().onNotify(&e);
}*/

//Animations
void Board::beginSwapAnimation()
{
	std::cerr << "BEGIN SWAP " << std::endl;

	if (selected0 == nullptr && selected1 == nullptr)
		return;

	STEP = 0;
	STATE = State::STATE_SWAP;

	if (selected0->getRow() == selected1->getRow()) ///line swap
	{
		Cell *min = (selected0->getCol() < selected1->getCol()) ? selected0 : selected1;
		Cell *max = (selected0->getCol() < selected1->getCol()) ? selected1 : selected0;

		min->setDirection(Direction::RIGHT);
		max->setDirection(Direction::LEFT);
	}
	else
		if (selected0->getCol() == selected1->getCol()) ///col swap
		{
			Cell *min = (selected0->getRow() < selected1->getRow()) ? selected0 : selected1;
			Cell *max = (selected0->getRow() < selected1->getRow()) ? selected1 : selected0;

			min->setDirection(Direction::DOWN);
			max->setDirection(Direction::UP);
		}
}

void Board::continueSwapAnimation() const
{
	std::cerr << "ANIMATE SWAP " << std::endl;

	Renderer& renderer = Game::instance().getRenderer();

	//first clear
	//renderer.clear(selected0->getY() * Cell::getWidth(), selected0->getX() * Cell::getHeight(), Cell::getWidth(), Cell::getHeight());
	//renderer.clear(selected1->getY() * Cell::getWidth(), selected1->getX() * Cell::getHeight(), Cell::getWidth(), Cell::getHeight());
	
	//compute new coordinates
	int x0, y0, x1, y1;
	computeCoordinates(selected0, x0, y0);
	computeCoordinates(selected1, x1, y1);

	/*!!!!*/
	selected0->setX(x0);
	selected0->setY(y0);
	selected1->setX(x1);
	selected1->setY(y1);
	/*!!!!*/

	//draw image
	//renderer.drawObject(ObjRenderable{ selected0->getName(), 0, 0, OFFSET_X + y0, OFFSET_Y + x0, Cell::getWidth(), Cell::getHeight() });
	//renderer.drawObject(ObjRenderable{ selected1->getName(), 0, 0, OFFSET_X + y1, OFFSET_Y + x1, Cell::getWidth(), Cell::getHeight() });
}

void Board::computeCoordinates(const Cell* cell, int& x, int& y) const
{
	switch (cell->getDirection())
	{
	case Direction::UP:
		//x = cell->getX() - STEP * SWAP_DIFF_STEP;
		x = cell->getX() - SWAP_DIFF_STEP;
		y = cell->getY();
		break;
	case Direction::DOWN:
		//x = cell->getX() + STEP * SWAP_DIFF_STEP;
		x = cell->getX() + SWAP_DIFF_STEP;
		y = cell->getY();
		break;
	case Direction::LEFT:
		x = cell->getX();
		y = cell->getY() - SWAP_DIFF_STEP;
		//y = cell->getY() - STEP * SWAP_DIFF_STEP;
		break;
	case Direction::RIGHT:
		x = cell->getX();
		y = cell->getY() + SWAP_DIFF_STEP;
		//y = cell->getY() + STEP * SWAP_DIFF_STEP;
		break;
	default:
		;
	}
}

void Board::endSwap()
{
	std::cerr << "end SWAP " << std::endl;

	bool v = isSwapValid();

	std::string s = v ? "true" : "false";
	std::cerr << s << std::endl;

	int oldX = selected0->getRow();
	int oldY = selected0->getCol();
	int cellX = selected1->getRow();
	int cellY = selected1->getCol();

	if (v)
	{
		//swap
		std::swap(cells[oldX][oldY], cells[cellX][cellY]);

		cells[oldX][oldY].setX(oldX * Cell::getWidth());
		cells[oldX][oldY].setY(oldY * Cell::getHeight());
		cells[cellX][cellY].setX(cellX * Cell::getWidth());
		cells[cellX][cellY].setY(cellY * Cell::getHeight());

		rowHasMatches(oldX);
		if (oldX != cellX)
			rowHasMatches(cellX);
		colHasMatches(oldY);
		if (oldY != cellY)
			colHasMatches(cellY);

		STATE = State::STATE_EXPLODE;
		Board::STEP = 0;
		//update();

		selected0 = nullptr;
		selected1 = nullptr;
	}
	else
	{
		Board::STEP = 0;
		STATE = State::STATE_INVALID_SWAP;
		
		/*cells[oldX][oldY].setX(cellX * Cell::getWidth());
		cells[oldX][oldY].setY(cellY * Cell::getHeight());
		cells[cellX][cellY].setX(oldX * Cell::getWidth());
		cells[cellX][cellY].setY(oldY * Cell::getHeight());*/

		switch (selected0->getDirection())
		{
		case Direction::UP:
			selected0->setDirection(Direction::DOWN);
			selected1->setDirection(Direction::UP);
			break;
		case Direction::DOWN:
			selected0->setDirection(Direction::UP);
			selected1->setDirection(Direction::DOWN);
			break;
		case Direction::LEFT:
			selected0->setDirection(Direction::RIGHT);
			selected1->setDirection(Direction::LEFT);
			break;
		case Direction::RIGHT:
			selected0->setDirection(Direction::LEFT);
			selected1->setDirection(Direction::RIGHT);
			break;
		default:
			;
		}
	
	}
}

bool Board::boardHasMatches()
{
	bool noExplode = false;
	for (int i = 0; i < height; ++i)
		//noExplode = noExplode || computeLine(i);
		if (rowHasMatches(i))
			noExplode = true;

	for (int i = 0; i < width; ++i)
		//noExplode = noExplode || computeCol(i);
		if (colHasMatches(i))
			noExplode = true;

	if (noExplode)
	{
		STATE = State::STATE_EXPLODE;
		Board::STEP = 0;
		//update();
	}

	return noExplode;
}

void Board::startFall()
{
	std::cerr << "START FILL" << std::endl;
 	for (int i = height - 2; i >= 0; --i)
		for (int j = 0; j < width; ++j)
		{
			if (cells[i][j].getType() != Cell::EMPTY)
			{
				if (cells[i + 1][j].getType() == Cell::EMPTY || cells[i + 1][j].getDirection() == Direction::DOWN)
				{
					cells[i][j].setDirection(Direction::DOWN);
				}
			}
		}

	//check first line
	for (int j = 0; j < width; ++j)
	{
		if (cells[0][j].getDirection() == Direction::DOWN || cells[0][j].getType() == Cell::EMPTY)
		{
			generator[j] = { probes[rand() % probes.size()], -1 * Cell::getWidth(), j * Cell::getHeight() };
			generator[j].setDirection(Direction::DOWN);
		}
	}
}

void Board::renderGenerator()
{
	Renderer& renderer = Game::instance().getRenderer();

	//first line
	for (int j = 0; j < width; ++j)
		if (generator[j].getType() != Cell::EMPTY)
		{
		int x, y;
		computeCoordinates(&generator[j], x, y);

		//generator[j].setX(x);
		//	generator[j].setY(y);

		//std::cerr << "x = " << x << " y = " << y << std::endl;
		//std::cerr << "x = " << (NUM_SWAP_STEPS - STEP) * SWAP_DIFF_STEP << " y = " << Cell::getHeight() - (NUM_SWAP_STEPS - STEP) * SWAP_DIFF_STEP << std::endl;

		renderer.drawObject(ObjRenderable{ generator[j].getName(), 0, (NUM_SWAP_STEPS - STEP) * SWAP_DIFF_STEP, OFFSET_X + y, OFFSET_Y + 0, Cell::getWidth(), Cell::getHeight() - (NUM_SWAP_STEPS - STEP) * SWAP_DIFF_STEP });
		}

}

void Board::animateFall()
{
	std::cerr << "ANIMATE FILL" << std::endl;
	Renderer& renderer = Game::instance().getRenderer();

	//first line
	/*for (int j = 0; j < width; ++j)
		if (generator[j].getType() != Cell::EMPTY)
		{
			int x, y;
			computeCoordinates(&generator[j], x, y);
			
			//generator[j].setX(x);
		//	generator[j].setY(y);

			//std::cerr << "x = " << x << " y = " << y << std::endl;
			//std::cerr << "x = " << (NUM_SWAP_STEPS - STEP) * SWAP_DIFF_STEP << " y = " << Cell::getHeight() - (NUM_SWAP_STEPS - STEP) * SWAP_DIFF_STEP << std::endl;

			renderer.drawObject(ObjRenderable{ generator[j].getName(), 0, (NUM_SWAP_STEPS - STEP) * SWAP_DIFF_STEP, OFFSET_X + y, OFFSET_Y + 0, Cell::getWidth(), Cell::getHeight() - (NUM_SWAP_STEPS - STEP) * SWAP_DIFF_STEP });
		}*/
	//renderGenerator();

	for (int i = height - 1; i >= 0; --i)
		for (int j = 0; j < width; ++j)
		{
		if (cells[i][j].getDirection() == Direction::DOWN)
		{
				if (i != height - 1 && (cells[i + 1][j].getDirection() == Direction::DOWN || (cells[i + 1][j].getType() == Cell::EMPTY)))
				{
					int x, y;
					computeCoordinates(&cells[i][j], x, y);

					/*!!!*/
					cells[i][j].setX(x);
					cells[i][j].setY(y);

					//renderer.drawObject(ObjRenderable{ cells[i][j].getName(), 0, 0, OFFSET_X + y, OFFSET_Y + x, Cell::getWidth(), Cell::getHeight() });
				}
				else
				{	
					cells[i][j].setDirection(Direction::NONE);
				}
			}
		}
}

bool Board::endFall()
{
	std::cerr << "END FILL" << std::endl;

	for (int i = height - 1; i >= 0; --i)
		for (int j = 0; j < width; ++j)
		{
		if (i == 0 && (cells[i][j].getDirection() == Direction::DOWN || cells[i][j].getType() == Cell::EMPTY))
		{
			//cells[i][j].setType(Cell::EMPTY);
			cells[i][j].setType(generator[j].getType());
			generator[j].setType(Cell::EMPTY);
			generator[j].setX(j * 60);
			generator[j].setY(-60);
			cells[i][j].setDirection(Direction::NONE);
			cells[i][j].setX(i * Cell::getWidth());
			cells[i][j].setY(j * Cell::getHeight());
		}
		else
			if ((i > 0) && (cells[i][j].getDirection() == Direction::DOWN || cells[i][j].getType() == Cell::EMPTY))
			{
				cells[i][j].setType(cells[i - 1][j].getType());
				cells[i][j].setDirection(Direction::NONE);
				cells[i][j].setX(i * Cell::getWidth());
				cells[i][j].setY(j * Cell::getHeight());
			}
		}

	for (int i = height - 1; i >= 0; --i)
		for (int j = 0; j < width; ++j)
		{
		if (i == 0)
		{
			if (cells[i][j].getType() == Cell::EMPTY)
				return true;
		}
		else
			if (cells[i][j].getType() == Cell::EMPTY && cells[i - 1][j].getType() != Cell::EMPTY)
				return true;
		}

	return false;
}
