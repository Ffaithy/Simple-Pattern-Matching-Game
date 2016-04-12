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
		renderer.drawObject(ObjRenderable{ getName(), 0, 0, OFFSET_X + x, OFFSET_Y + y, Cell::getWidth(), Cell::getHeight() }, true);

		//render selection
		if (isSelected())
		{
			renderer.drawObject(ObjRenderable{ Cell::SELECTOR, 0, 0, OFFSET_X + x, OFFSET_Y + y, Cell::getWidth(), Cell::getHeight() }, false);
		}
	}

}

Board::Board(const std::vector<int>& probes, int width, int height) :
		mProbes(probes),
		width(width),
		height(height),
		mSelected0(nullptr),
		mSelected1(nullptr),
		mBlockInput(false)
{
	initRenderer();
};

void Board::setProbes(const std::vector<int>& newProbes)
{
	mProbes.clear();
	mProbes = newProbes;
}

void Board::generate()
{
	int sz = mProbes.size();
	srand(time(0));

	mGenerator.resize(width);

	// set up sizes
	mCells.resize(height);
	for (int i = 0; i < height; ++i)
		mCells[i].resize(width);

	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
		{
			int prevX = -1;
			int prevprevX = -1;
			bool checkSimX = false;

			int prevY = -1;
			int prevprevY = -1;
			bool checkSimY = false;

			if (j >= NUM_MATCHES - 1)
			{
				prevprevX = mCells[i][j - 2].getType();
				prevX= mCells[i][j - 1].getType();
				if (prevprevX == prevX)
					checkSimX = true;
			}

			if (i >= NUM_MATCHES - 1)
			{
				prevprevY = mCells[i - 2][j].getType();
				prevY = mCells[i - 1][j].getType();
				if (prevprevY == prevY)
					checkSimY = true;
			}

			int number = -1;
			do
			{
				number = rand() % sz;
			} while ((checkSimX && (number == prevX)) || (checkSimY && (number == prevY)) );

			mCells[i][j] = { mProbes[number], j * Cell::getHeight(), i * Cell::getWidth() };
		}
}

void Board::initRenderer() const
{
	Renderer& renderer = Game::instance().getRenderer();
	for (unsigned int i = 0; i < mProbes.size(); ++i)
	{
		renderer.loadTexture(Cell::getName(mProbes[i]));
	}

	renderer.loadTexture(Cell::SELECTOR);
}

void Board::render()
{
	renderGenerator();

	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			mCells[i][j].render();
		}

}

void Board::renderGenerator()
{
	Renderer& renderer = Game::instance().getRenderer();

	//first line
	for (int j = 0; j < width; ++j)
		if (mGenerator[j].getType() != Cell::EMPTY)
		{
			int x, y;
			computeCoordinates(&mGenerator[j], x, y);
			renderer.drawObject(ObjRenderable{ mGenerator[j].getName(), 0, (NUM_SWAP_STEPS - STEP) * SWAP_DIFF_STEP, OFFSET_X + x, OFFSET_Y + 0, Cell::getWidth(), Cell::getHeight() - (NUM_SWAP_STEPS - STEP) * SWAP_DIFF_STEP });
		}

}

void Board::handleInput(int x, int y)
{
	if (mBlockInput)
		return;

	Renderer& renderer = Game::instance().getRenderer();

	//find the cell that the user clicked on
	int cCol = (x - OFFSET_X) / Cell::getWidth();
	int cRow = (y - OFFSET_Y) / Cell::getHeight();
	std::cerr << cCol << " " << cRow << std::endl;

	//new selection, no previous selection exists
	if (mSelected0 == nullptr)
	{
		mSelected0 = &mCells[cRow][cCol];
		mCells[cRow][cCol].setSelected(true);
	
		std::cerr << "SELECTION " << cCol << " " << cRow << std::endl;
	}
	else
	//handle the case when one cell is already selected
	{
		int oCol = mSelected0->getCol();
		int oRow = mSelected0->getRow();

		//move selection
		if (std::abs(oCol - cCol) + std::abs(oRow - cRow) > 1)
		{
			//clear old selection
			mCells[oRow][oCol].setSelected(false);
			
			//select the new cell
			mSelected0 = &mCells[cRow][cCol];
			mCells[cRow][cCol].setSelected(true);
		}
		else
			//the cell user clicked on is already selected, so unselect it
			if (std::abs(cRow - oRow) + std::abs(cCol - oCol) == 0)
			{
				//clear old selection
				mCells[cRow][cCol].setSelected(false);
				mSelected0 = nullptr;
			}
			else
			//user selected a valid cell, begin swap animation & block input
			{
				mBlockInput = true;
				mSelected1 = &mCells[cRow][cCol];

				//clear old selection
				mCells[oRow][oCol].setSelected(false);

				//swap
				beginSwapAnimation();
			}
	}
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
			mSelected0->setDirection(Direction::NONE);
			mSelected1->setDirection(Direction::NONE);
			mSelected0 = nullptr;
			mSelected1 = nullptr;

			mBlockInput = false;
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
					mBlockInput = false;
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

bool Board::isSwapValid() const
{
	return (isPieceInARowMatch(mSelected0->getRow(), mSelected0->getCol(), mSelected0->getType()) ||
		isPieceInARowMatch(mSelected1->getRow(), mSelected1->getCol(), mSelected1->getType()) ||
		isPieceInAColMatch(mSelected0->getRow(), mSelected0->getCol(), mSelected0->getType()) ||
		isPieceInAColMatch(mSelected1->getRow(), mSelected1->getCol(), mSelected1->getType()));

}

bool Board::isPieceInARowMatch(int row, int col, int type) const
{
	int x = row;
	int y = col;
	int count = 1;
	bool valid = false;

	//check col
	y--;
	while (y >= 0 && mCells[x][y].getType() == type)
	{
		count++;
		if (count >= NUM_MATCHES)
		{
			return true;
		}
		y--;
	}

	//reset y
	y = col;
	y++;
	while (y < width && mCells[x][y].getType() == type)
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

bool Board::isPieceInAColMatch(int row, int col, int type) const
{
	int x = row;
	int y = col;
	int count = 1;
	bool valid = false;

	//check line
	x--;
	while (x >= 0 && mCells[x][y].getType() == type)
	{
		count++;
		if (count >= NUM_MATCHES)
		{
			return true;
		}
		x--;
	}

	//reset y
	x = row;
	x++;
	while (x < height && mCells[x][y].getType() == type)
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

bool Board::rowHasMatches(int row)
{
	std::cerr << "rowHasMatches " << row << std::endl;

	bool hasMatch = false;
	int count = 1;
	int startIdx = 0; 
	int finalIdx = 1;
	int type = mCells[row][0].getType();
	while (finalIdx < width)
	{
		if (type == mCells[row][finalIdx].getType())
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
			for (int i = startIdx; i < ((type == mCells[row][finalIdx].getType() && (finalIdx == width - 1)) ? (finalIdx + 1) : finalIdx); ++i)
			{
				std::cerr << "EXPLODE [ " << row << " ][ " << i << " ]" << std::endl;
				mCells[row][i].setExplode(true);
			}
			mLinesToExplode.insert(row);
				
			std::cerr << "INSERTING LINE " << row << std::endl;

			hasMatch = true;
		}

		count = 1;
		type = mCells[row][finalIdx].getType();
		startIdx = finalIdx;

		finalIdx++;
	}

	return hasMatch;
}

bool Board::colHasMatches(int col)
{
	std::cerr << "colHasMatches " << col << std::endl;

	bool hasMatch = false;
	int count = 1;
	int startIdx = 0;
	int finalIdx = 1;
	int type = mCells[0][col].getType();
	while (finalIdx < height)
	{
		if (type == mCells[finalIdx][col].getType())
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
			for (int i = startIdx; i < ((type == mCells[finalIdx][col].getType() && (finalIdx == height - 1)) ? (finalIdx + 1) : finalIdx); ++i)
			{
				std::cerr << "EXPLODE [ " << i << " ][ " << col << " ]" << std::endl;
				mCells[i][col].setExplode(true);
			}
			mColsToExplode.insert(col);

			std::cerr << "INSERTING COL " << col << std::endl;

			hasMatch = true;
		}

		count = 1;
		type = mCells[finalIdx][col].getType();
		startIdx = finalIdx;

		finalIdx++;
	}

	return hasMatch;
}

bool Board::boardHasMatches()
{
	bool explode = false;
	for (int i = 0; i < height; ++i)
		if (rowHasMatches(i))
			explode = true;

	for (int i = 0; i < width; ++i)
		if (colHasMatches(i))
			explode = true;

	if (explode)
	{
		STATE = State::STATE_EXPLODE;
		Board::STEP = 0;
		computeScore();
	}

	return explode;
}

void Board::computeScore()
{
	int points = 0;

	std::cerr << "Compute Score" << std::endl;

	for (auto it : mLinesToExplode)
	{
		for (int i = 0; i < width; ++i)
			if (mCells[it][i].isExplode())
			{
				points++;
				std::cerr << "EXPLODE !!!! " << it << " " << i << std::endl;
				STATE = State::STATE_EXPLODE;
				Board::STEP = 0;
			}
	}
	mLinesToExplode.clear();

	for (auto it : mColsToExplode)
	{
		for (int i = 0; i < height; ++i)
			if (mCells[i][it].isExplode())
			{
				points++;
				std::cerr << "EXPLODE !!!! " << i << " " << it << std::endl;
				STATE = State::STATE_EXPLODE;
				Board::STEP = 0;
			}
	}
	mColsToExplode.clear();

	EventScore e(points);
	Game::instance().onNotify(&e);
}

//Animations
void Board::beginSwapAnimation()
{
	std::cerr << "BEGIN SWAP " << std::endl;

	if (mSelected0 == nullptr && mSelected1 == nullptr)
		return;

	STEP = 0;
	STATE = State::STATE_SWAP;

	if (mSelected0->getRow() == mSelected1->getRow()) ///line swap
	{
		Cell *min = (mSelected0->getCol() < mSelected1->getCol()) ? mSelected0 : mSelected1;
		Cell *max = (mSelected0->getCol() < mSelected1->getCol()) ? mSelected1 : mSelected0;

		min->setDirection(Direction::RIGHT);
		max->setDirection(Direction::LEFT);
	}
	else
		if (mSelected0->getCol() == mSelected1->getCol()) ///col swap
		{
			Cell *min = (mSelected0->getRow() < mSelected1->getRow()) ? mSelected0 : mSelected1;
			Cell *max = (mSelected0->getRow() < mSelected1->getRow()) ? mSelected1 : mSelected0;

			min->setDirection(Direction::DOWN);
			max->setDirection(Direction::UP);
		}
}

void Board::continueSwapAnimation() const
{
	std::cerr << "ANIMATE SWAP " << std::endl;

	Renderer& renderer = Game::instance().getRenderer();
	
	//compute new coordinates
	int x0, y0, x1, y1;
	computeCoordinates(mSelected0, x0, y0);
	computeCoordinates(mSelected1, x1, y1);

	mSelected0->setX(x0);
	mSelected0->setY(y0);
	mSelected1->setX(x1);
	mSelected1->setY(y1);
}

void Board::endSwap()
{
	std::cerr << "end SWAP " << std::endl;

	bool v = isSwapValid();

	std::string s = v ? "true" : "false";
	std::cerr << s << std::endl;

	int oRow = mSelected0->getRow();
	int oCol = mSelected0->getCol();
	int cRow = mSelected1->getRow();
	int cCol = mSelected1->getCol();

	if (v)
	{
		EventMove event;
		Game::instance().onNotify(&event);

		//swap
		std::swap(mCells[oRow][oCol], mCells[cRow][cCol]);

		mCells[oRow][oCol].setX(oCol * Cell::getHeight());
		mCells[oRow][oCol].setY(oRow * Cell::getWidth());
		mCells[cRow][cCol].setX(cCol * Cell::getHeight());
		mCells[cRow][cCol].setY(cRow * Cell::getWidth());

		rowHasMatches(oRow);
		if (oRow != cRow)
			rowHasMatches(cRow);
		colHasMatches(oCol);
		if (oCol != cCol)
			colHasMatches(cCol);

		STATE = State::STATE_EXPLODE;
		Board::STEP = 0;

		mSelected0 = nullptr;
		mSelected1 = nullptr;
	}
	else
	{
		Board::STEP = 0;
		STATE = State::STATE_INVALID_SWAP;

		switch (mSelected0->getDirection())
		{
		case Direction::UP:
			mSelected0->setDirection(Direction::DOWN);
			mSelected1->setDirection(Direction::UP);
			break;
		case Direction::DOWN:
			mSelected0->setDirection(Direction::UP);
			mSelected1->setDirection(Direction::DOWN);
			break;
		case Direction::LEFT:
			mSelected0->setDirection(Direction::RIGHT);
			mSelected1->setDirection(Direction::LEFT);
			break;
		case Direction::RIGHT:
			mSelected0->setDirection(Direction::LEFT);
			mSelected1->setDirection(Direction::RIGHT);
			break;
		default:
			;
		}

	}
}

void Board::startFall()
{
	std::cerr << "START FILL" << std::endl;
 	for (int i = height - 2; i >= 0; --i)
		for (int j = 0; j < width; ++j)
		{
			if (mCells[i][j].getType() != Cell::EMPTY)
			{
				if (mCells[i + 1][j].getType() == Cell::EMPTY || mCells[i + 1][j].getDirection() == Direction::DOWN)
				{
					mCells[i][j].setDirection(Direction::DOWN);
				}
			}
		}

	//check first line
	for (int j = 0; j < width; ++j)
	{
		if (mCells[0][j].getDirection() == Direction::DOWN || mCells[0][j].getType() == Cell::EMPTY)
		{
			mGenerator[j] = { mProbes[rand() % mProbes.size()], j * Cell::getWidth(), -1 * Cell::getHeight() };
			mGenerator[j].setDirection(Direction::DOWN);
		}
	}
}

void Board::animateFall()
{
	std::cerr << "ANIMATE FILL" << std::endl;
	Renderer& renderer = Game::instance().getRenderer();

	for (int i = height - 1; i >= 0; --i)
		for (int j = 0; j < width; ++j)
		{
		if (mCells[i][j].getDirection() == Direction::DOWN)
		{
				if (i != height - 1 && (mCells[i + 1][j].getDirection() == Direction::DOWN || (mCells[i + 1][j].getType() == Cell::EMPTY)))
				{
					int x, y;
					computeCoordinates(&mCells[i][j], x, y);

					mCells[i][j].setX(x);
					mCells[i][j].setY(y);

				}
				else
				{	
					mCells[i][j].setDirection(Direction::NONE);
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
		if (i == 0 && (mCells[i][j].getDirection() == Direction::DOWN || mCells[i][j].getType() == Cell::EMPTY))
		{
			mCells[i][j].setType(mGenerator[j].getType());
			mCells[i][j].setDirection(Direction::NONE);
			mCells[i][j].setX(j * Cell::getWidth());
			mCells[i][j].setY(i * Cell::getHeight());

			mGenerator[j].setType(Cell::EMPTY);
			mGenerator[j].setX(j * Cell::getWidth());
			mGenerator[j].setY(-1 * Cell::getHeight());
		}
		else
			if ((i > 0) && (mCells[i][j].getDirection() == Direction::DOWN || mCells[i][j].getType() == Cell::EMPTY))
			{
				mCells[i][j].setType(mCells[i - 1][j].getType());
				mCells[i][j].setDirection(Direction::NONE);
				mCells[i][j].setX(j * Cell::getWidth());
				mCells[i][j].setY(i * Cell::getHeight());
			}
		}

	for (int i = height - 1; i >= 0; --i)
		for (int j = 0; j < width; ++j)
		{
		if (i == 0)
		{
			if (mCells[i][j].getType() == Cell::EMPTY)
				return true;
		}
		else
			if (mCells[i][j].getType() == Cell::EMPTY && mCells[i - 1][j].getType() != Cell::EMPTY)
				return true;
		}

	return false;
}


void Board::computeCoordinates(const Cell* cell, int& x, int& y) const
{
	switch (cell->getDirection())
	{
	case Direction::UP:
		y = cell->getY() - SWAP_DIFF_STEP;
		x = cell->getX();
		break;
	case Direction::DOWN:
		y = cell->getY() + SWAP_DIFF_STEP;
		x = cell->getX();
		break;
	case Direction::LEFT:
		y = cell->getY();
		x = cell->getX() - SWAP_DIFF_STEP;
		break;
	case Direction::RIGHT:
		y = cell->getY();
		x = cell->getX() + SWAP_DIFF_STEP;
		break;
	default:
		;
	}
}