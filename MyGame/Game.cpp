#include <unordered_set>
#include "Game.h"
#include <fstream>
#include <iostream>

Game Game::_instance;

//Initializares for static memmbers
const int Game::MAX_LEVEL = 2;

int Game::mScore = 0;
int Game::mObjScore = 0;
int Game::mLevel = 0;
int Game::mNumMoves = 0;
int Game::mNumInitMoves = 0;
bool Game::mStop = false;

const std::string Game::SAVE_FILE { "./save/data.txt" };
const std::string Game::LEVEL_FILE { "./levels/level" };
const std::string Game::LEVEL_FILE_EXT { ".txt" };
const std::string Game::BACKGROUND_IMG { "./assets/background/blue_backing.png" }; 
const std::string Game::FONT_TYPE { "./fonts/ANUDI.ttf" };

TextRenderable Game::mLevelStr { "Level: ", 30, 10, 24};
TextRenderable Game::mObjStr { "Objective: ", 370, 10, 24 };
TextRenderable Game::mMovesStr { "Moves left: ", 310, 50, 24 };
TextRenderable Game::mScoreStr { "Score: ", 30, 50, 24};
TextRenderable Game::mResultStr { "Game over", 100, 250, 24};
TextRenderable Game::mTryAgainStr { "Click to try again", 100, 290, 24 };

void Game::init()
{
	mRenderer = new Renderer();
	mRenderer->init();

	//load background
	mRenderer->drawBackground(BACKGROUND_IMG);

	//set font type
	mRenderer->setFontType(FONT_TYPE, 24);

	//load the Game from files
	//the board is created in this step
	loadGame();

	//initialize the board
	mBoard->generate();
}

void Game::close()
{
	//free resources
	mRenderer->close();
	delete mRenderer;
	delete mBoard;
}

void Game::updateScore(int points)
{
	//update user's score and show the changes on the screen
	mScore += points;
	mScoreStr.setAddText(std::to_string(mScore));
	mRenderer->drawText(mScoreStr);
}

void Game::updateNumMoves()
{
	//update number of left moves and show the changes on the screen
	mNumMoves--;
	mMovesStr.setAddText(std::to_string(mNumMoves));
	mRenderer->drawText(mMovesStr);
}

void Game::checkState()
{
	//update game state when user passed the current level
	if (mScore >= mObjScore)
	{
		saveGame();

		mResultStr.setText("Level Completed");
		mRenderer->drawText(mResultStr);
		mStop = true;

		mLevel++;

		if (mLevel > MAX_LEVEL)
			mLevel = 0;
	}
	else
		//update game state when user is out of moves
		if (mNumMoves == 0)
		{
			mResultStr.setText("Game Over");
			mRenderer->drawText(mResultStr);

			mTryAgainStr.setText("Click to try again");
			mRenderer->drawText(mTryAgainStr);

			mStop = true;
		}
}

//function to process events
void Game::onNotify(Event* ev)
{
	if (ev != nullptr)
	{
		ev->update();
	}
}

//Input handler
//Currently, the only event accepted is Mouse click
//TODO extend this
void Game::handleInput(int x, int y)
{
	//if game is stopped, user can either repeat the level or advance to next level
	if (mStop)
	{
		reset();

		//user is able to continue playing
		continueGame();
		mBoard->generate();
	}
	else
	{
		mBoard->handleInput(x, y);
	}
}

//method for updating the Board state & show the changes on the screen
void Game::update()
{
	if (mStop == false)
	{
		mBoard->update();
		mBoard->render();
	}
	else
	{
		checkState();
	}

	mRenderer->update();
}

//Load the Game from a save file
void Game::loadGame()
{
	std::string line;
	std::ifstream file;
	file.open(SAVE_FILE, std::ios::in);
	
	mLevel = 0;
	if (file.is_open())
	{
		//the last level passed by user is on the last line of the file
		int currLevel = 0;
		while (std::getline(file, line))
		{
			std::string::size_type sz; 
			//increase the level from the file with 1 to get the next level for user
			currLevel = std::stoi(line, &sz) + 1;
		}
		file.close();

		if (currLevel > MAX_LEVEL)
		{
			mLevel = 0;
		}
		else
		{
			mLevel = currLevel;
		}
	}

	//load data level from file
	loadLevel();
}

//Load the data level from configuration file
//Each level is stored in a file
void Game::loadLevel()
{
	std::string line;
	std::ifstream file;

	//build the level file name
	std::string levelFileStr = LEVEL_FILE + std::to_string(mLevel) + LEVEL_FILE_EXT;
	file.open(levelFileStr, std::ios::in);
	if (file.is_open())
	{
		std::getline(file, line);

		//first line is number of probes
		std::string::size_type sz;
		int numProbes = std::stoi(line, &sz);

		std::vector<int> probes;
		std::getline(file, line);

		//next line contains all the pobes ids, divided by spaces
		sz = 0;
		std::string str = line;
		for (int i = 0; i < numProbes; ++i)
		{
			str = str.substr(sz);
			int probe = std::stoi(str, &sz);
			probes.push_back(probe);
		}

		//initializes the board with probes read from file
		if (mBoard == nullptr)
		{
			mBoard = new Board(probes);
		}
		else
		{
			mBoard->setProbes(probes);
		}

		//next line is total number of moves for user
		std::getline(file, line);
		sz = 0;
		mNumMoves = std::stoi(line.substr(sz), &sz);
		mNumInitMoves = mNumMoves;

		//next line is the objective for user
		std::getline(file, line);
		sz = 0;
		mObjScore = std::stoi(line.substr(sz), &sz);

		file.close();
	}

	//set dynamic texts
	mLevelStr.setAddText(std::to_string(mLevel));
	mObjStr.setAddText(std::to_string(mObjScore));
	mMovesStr.setAddText(std::to_string(mNumMoves));
	mScoreStr.setAddText(std::to_string(mScore));

	//show mLevel and objective
	mRenderer->drawText(mLevelStr);
	mRenderer->drawText(mObjStr);

	//show left moves
	mRenderer->drawText(mMovesStr);

	//show score
	mRenderer->drawText(mScoreStr);
}

void Game::saveGame()
{
	//save current level is user has completed it & the score
	std::ofstream file;
	file.open(SAVE_FILE, std::ios::out | std::ios::app);
	file << mLevel << " " << mScore << std::endl;
	file.close();
}

//reset the Game state
void Game::reset()
{
	mScore = 0;
	loadLevel();
}