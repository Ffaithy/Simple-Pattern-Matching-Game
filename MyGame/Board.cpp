#include "Board.h"
#include "Game.h"
#include "Event.h"
#include <ctime>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

int Board::STEP = 0;
const int Board::OFFSET_Y = 100;
const int Board::OFFSET_X = 50;
Board::State Board::STATE = State::STATE_NONE;

Board::Board(const std::vector<int>& probes, int width, int height) :
		mProbes(probes),
		width(width),
		height(height),
		mSelected0(nullptr),
		mSelected1(nullptr),
		mBlockInput(false)
{
	//Initialize the components
	initRenderer();
	mAnimationManager.setMoveStep(SWAP_DIFF_STEP);
};

void Board::setProbes(const std::vector<int>& newProbes)
{
	mProbes.clear();
	mProbes = newProbes;
}

void Board::generate()
{
	//Initialize random seed for generating values
	int sz = mProbes.size();
	srand(time(0));

	mGenerator.resize(width);

	mCells.resize(height);

	for (int i = 0; i < height; ++i)
		mCells[i].resize(width);

	//Populate the cells with random values
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

			//check for macthes until a valid value is obtained
			int number = -1;
			do
			{
				number = rand() % sz;
			} while ((checkSimX && (mProbes[number] == prevX)) || (checkSimY && (mProbes[number] == prevY)));

			//store the new value
			mCells[i][j] = { mProbes[number], j * mCells[i][j].getDefaultHeight(), i * mCells[i][j].getDefaultWidth() };
		}
}

void Board::initRenderer() const
{
	//load the necessary art
	Renderer& renderer = Game::instance().getRenderer();
	for (unsigned int i = 0; i < mProbes.size(); ++i)
	{
		renderer.loadTexture(Cell::getName(mProbes[i]));
	}

	renderer.loadTexture(Cell::SELECTOR);
}

void Board::render()
{
	//render the first line
	renderGenerator();

	//render the board
	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			mCells[i][j].render(STEP, NUM_EXPLODE_STEPS);
		}

}

void Board::renderGenerator()
{
	//first line
	for (int j = 0; j < width; ++j)
		if (mGenerator[j].getType() != Cell::EMPTY)
		{
			int x, y;
			mAnimationManager.computeNextCoordinates(&mGenerator[j], x, y);
			mGenerator[j].renderWithDynamicParams(0, (NUM_SWAP_STEPS - STEP) * SWAP_DIFF_STEP, x, 0, mGenerator[j].getDefaultWidth(), mGenerator[j].getDefaultHeight() - (NUM_SWAP_STEPS - STEP) * SWAP_DIFF_STEP);
		}

}

//The only possible action is Mouse click
//TODO extend the user actions
void Board::handleInput(int x, int y)
{
	if (mBlockInput)
		return;

	if (STATE == State::STATE_NONE)
	{
		Renderer& renderer = Game::instance().getRenderer();

		//find the cell that the user clicked on
		int cCol = (x - OFFSET_X) / Cell::getDefaultWidth();
		int cRow = (y - OFFSET_Y) / Cell::getDefaultHeight();
		std::cerr << cCol << " " << cRow << std::endl;

		if (cCol < 0 || cCol >= DEFAULT_WIDTH || cRow < 0 || cRow >= DEFAULT_HEIGHT)
			return;

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

					//begin swap animation
					STEP = 0;
					STATE = State::STATE_SWAP;

					mAnimationManager.beginSwapAnimation(mSelected0, mSelected1);
				}
		}
	}
}

void Board::update()
{
	//update the Board state, based on Step 
	switch (STATE)
	{
	//process a valid swap 
	case State::STATE_SWAP:
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
			}
		break;
		}
	//process an invalid swap
	case State::STATE_INVALID_SWAP:
	{
		if (STEP < NUM_SWAP_STEPS)
		{
			STEP++;
			continueSwapAnimation();
		}
		else
		{
			STEP = 0;
			STATE = State::STATE_NONE;
			mSelected0->setDirection(Direction::NONE);
			mSelected1->setDirection(Direction::NONE);
			mSelected0 = nullptr;
			mSelected1 = nullptr;

			mBlockInput = false;
		}
		break;
	}
	//process a fall animation
	case State::STATE_FALL:
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

				//after the fall finises, check for new explosions
				if (boardHasMatches())
				{
					STATE = State::STATE_EXPLODE;
				}
				else
				{
					STATE = State::STATE_NONE;
					//notify the Game that animation is completed, to check the state
					EventCheckState ev;
					Game::instance().onNotify(&ev);
					mBlockInput = false;
				}
			}
			else
			{
				//continue falling if necessary
				//the falling takes place line by line
				STEP = 0;
				startFall();
			}
		}

		break;
	}

	//process explosions
	case State::STATE_EXPLODE:
	{
		if (STEP < NUM_EXPLODE_STEPS)
			Board::STEP++;
		else
		{
			//after an explosion, the board must be filled with new values
			STATE = State::STATE_FALL;
			Board::STEP = 0;
			startFall();
		}

		break;
	}
	default:
		break;
	}
}

//check if the swap generates an explosion
bool Board::isSwapValid()
{
	int count = 0;

	int oRow = mSelected0->getRow();
	int oCol = mSelected0->getCol();
	int cRow = mSelected1->getRow();
	int cCol = mSelected1->getCol();

	bool valid = false;

	//the cells will be marked for explosion
	//it is sufficient to check the selected cells' rows and columns
	if (rowHasMatches(oRow))
		valid = true;
	if (rowHasMatches(cRow))
		valid = true;
	if (colHasMatches(oCol))
		valid = true;
	if (colHasMatches(cCol))
		valid = true;

	return valid;
}

//check if a row has matches
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

		//mark the matched cells
		if (count >= NUM_MATCHES && type != Cell::EMPTY)
		{
			//handle special case when finalIdx reaches the end
			for (int i = startIdx; i < ((type == mCells[row][finalIdx].getType() && (finalIdx == width - 1)) ? (finalIdx + 1) : finalIdx); ++i)
			{
				std::cerr << "EXPLODE [ " << row << " ][ " << i << " ]" << std::endl;
				mCells[row][i].setExplode(true);
			}
			//store the line for exploding
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

//check if a column has matches
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

		//mark the matched cells
		if (count >= NUM_MATCHES && type != Cell::EMPTY)
		{
			//handle special case when finalIdx reaches the end
			for (int i = startIdx; i < ((type == mCells[finalIdx][col].getType() && (finalIdx == height - 1)) ? (finalIdx + 1) : finalIdx); ++i)
			{
				std::cerr << "EXPLODE [ " << i << " ][ " << col << " ]" << std::endl;
				mCells[i][col].setExplode(true);
			}
			//store the column
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

//this function gets called after the falling animation ends and checks for a new explosion through the entire board
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

//When computing the score, only the stored lines and columns are checked
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
				STATE = State::STATE_EXPLODE;
				Board::STEP = 0;
			}
	}
	mColsToExplode.clear();

	//Notifies the game to update the user score
	EventScore e(points);
	Game::instance().onNotify(&e);
}

//Animations
void Board::continueSwapAnimation() const
{
	std::cerr << "ANIMATE SWAP " << std::endl;
	
	//compute new coordinates
	mAnimationManager.continueMovement(mSelected0);
	mAnimationManager.continueMovement(mSelected1);
}

void Board::endSwap()
{
	std::cerr << "END SWAP " << std::endl;

	//do the actual swap
	int oRow = mSelected0->getRow();
	int oCol = mSelected0->getCol();
	int cRow = mSelected1->getRow();
	int cCol = mSelected1->getCol();

	std::swap(mCells[oRow][oCol], mCells[cRow][cCol]);

	mCells[oRow][oCol].setX(oCol * Cell::getDefaultHeight());
	mCells[oRow][oCol].setY(oRow * Cell::getDefaultWidth());
	mCells[cRow][cCol].setX(cCol * Cell::getDefaultHeight());
	mCells[cRow][cCol].setY(cRow * Cell::getDefaultWidth());

	//check is swap generates some matches
	bool valid = isSwapValid();

	std::string s = valid ? "true" : "false";
	std::cerr << s << std::endl;

	if (valid)
	{
		//is swap is valid, decrease the number of moves
		EventMove event;
		Game::instance().onNotify(&event);

		STATE = State::STATE_EXPLODE;
		Board::STEP = 0;

		//clear selections
		mSelected0 = nullptr;
		mSelected1 = nullptr;
	}
	else
	{
		//swap is invalid
		Board::STEP = 0;
		STATE = State::STATE_INVALID_SWAP;

		//swap back the cells
		std::swap(mCells[oRow][oCol], mCells[cRow][cCol]);

		mCells[oRow][oCol].setX(cCol * Cell::getDefaultHeight());
		mCells[oRow][oCol].setY(cRow * Cell::getDefaultWidth());
		mCells[cRow][cCol].setX(oCol * Cell::getDefaultHeight());
		mCells[cRow][cCol].setY(oRow * Cell::getDefaultWidth());

		mAnimationManager.invertDirection(mSelected0);
		mAnimationManager.invertDirection(mSelected1);
	}
}

void Board::startFall()
{
	std::cerr << "START FALL" << std::endl;

	//Initialize the falling animation
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
			mGenerator[j] = { mProbes[rand() % mProbes.size()], j * Cell::getDefaultWidth(), -1 * Cell::getDefaultHeight() };
			mGenerator[j].setDirection(Direction::DOWN);
		}
	}
}

void Board::animateFall()
{
	std::cerr << "ANIMATE FALL" << std::endl;

	for (int i = height - 1; i >= 0; --i)
		for (int j = 0; j < width; ++j)
		{
		if (mCells[i][j].getDirection() == Direction::DOWN)
		{
				if (i != height - 1 && (mCells[i + 1][j].getDirection() == Direction::DOWN || (mCells[i + 1][j].getType() == Cell::EMPTY)))
				{
					//use the Animation component to update the positions of the cells on screen
					mAnimationManager.continueMovement(&mCells[i][j]);
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
	std::cerr << "END FALL" << std::endl;

	//the falling animation is complete
	for (int i = height - 1; i >= 0; --i)
		for (int j = 0; j < width; ++j)
		{
			//Update the cells with the new values
			if (i == 0 && (mCells[i][j].getDirection() == Direction::DOWN || mCells[i][j].getType() == Cell::EMPTY))
			{
				mCells[i][j].setType(mGenerator[j].getType());
				mCells[i][j].setDirection(Direction::NONE);
				mCells[i][j].setX(j * Cell::getDefaultWidth());
				mCells[i][j].setY(i * Cell::getDefaultHeight());

				mGenerator[j].setType(Cell::EMPTY);
				mGenerator[j].setX(j * Cell::getDefaultWidth());
				mGenerator[j].setY(-1 * Cell::getDefaultHeight());
			}
			else
				if ((i > 0) && (mCells[i][j].getDirection() == Direction::DOWN || mCells[i][j].getType() == Cell::EMPTY))
				{
					mCells[i][j].setType(mCells[i - 1][j].getType());
					mCells[i][j].setDirection(Direction::NONE);
					mCells[i][j].setX(j * Cell::getDefaultWidth());
					mCells[i][j].setY(i * Cell::getDefaultHeight());
				}
			}

	//check if we need falling to happen again
	//the falling animation processes line by line
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