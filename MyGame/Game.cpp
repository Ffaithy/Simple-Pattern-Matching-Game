#include <unordered_set>
#include "Game.h"
#include <fstream>
#include <iostream>

Game Game::_instance;

const int Game::MAX_LEVEL = 2;

int Game::score = 0;
int Game::objScore = 0;
int Game::level = 0;
int Game::numMoves = 0;
int Game::numInitMoves = 0;
bool Game::stop = false;

const std::string Game::SAVE_FILE { "./save/data.txt" };
const std::string Game::LEVEL_FILE { "./levels/level" };
const std::string Game::LEVEL_FILE_EXT { ".txt" };
const std::string Game::BACKGROUND_IMG { "./assets/background/blue_backing.png" };
const std::string Game::FONT_TYPE { "./fonts/ANUDI.ttf" };

TextRenderable Game::levelStr { "Level: ", 30, 10, 24};
TextRenderable Game::objStr { "Objective: ", 370, 10, 24 };
TextRenderable Game::movesStr { "Moves left: ", 310, 50, 24 };
TextRenderable Game::scoreStr { "Score: ", 30, 50, 24};
TextRenderable Game::resultStr { "Game over", 100, 250, 24};
TextRenderable Game::tryAgainStr { "Click to try again", 100, 290, 24 };

void Game::init()
{
	renderer = new Renderer();
	renderer->init();

	//load background
	renderer->drawBackground(BACKGROUND_IMG);

	//show level
	renderer->setFontType(FONT_TYPE, 24);

	loadGame();
}

void Game::close()
{
	renderer->close();
	delete renderer;
	delete board;
}

void Game::updateScore(int points)
{
	score += points;
	scoreStr.setAddText(std::to_string(score));
	renderer->drawText(scoreStr);
}

void Game::updateNumMoves()
{
	numMoves--;
	movesStr.setAddText(std::to_string(numMoves));
	renderer->drawText(movesStr);
}

void Game::checkState()
{
	if (score >= objScore)
	{
		saveGame();

		resultStr.setText("Level Completed");
		renderer->drawText(resultStr);
		stop = true;

		level++;

		if (level > MAX_LEVEL)
			level = 0;
	}
	else
		if (numMoves == 0)
		{
			resultStr.setText("Game Over");
			renderer->drawText(resultStr);

			tryAgainStr.setText("Click to try again");
			renderer->drawText(tryAgainStr);

			stop = true;
		}
}

void Game::onNotify(Event* ev)
{
	if (ev != nullptr)
	{
		ev->update();
	}
}

void Game::handleInput(int x, int y)
{
	if (stop)
	{
		reset();
		continueGame();
		board->generate();
	}
	else
	{
		board->handleInput(x, y);
	}
}

void Game::update()
{
	if (stop == false)
	{
		board->update();

		if (!stop)
		{
			board->render();
		}
	}

	renderer->update();
}

void Game::loadGame()
{
	std::string line;
	std::ifstream file;
	file.open(SAVE_FILE, std::ios::in);
	
	level = 0;
	bool foundData = false;
	if (file.is_open())
	{
		int currLevel = 0;
		while (std::getline(file, line))
		{
			foundData = true;
			std::string::size_type sz; 
			int currLevel = std::stoi(line, &sz) + 1;
		}
		file.close();

		if (currLevel >= MAX_LEVEL)
		{
			level = 0;
		}
		else
		{
			level = foundData ? currLevel + 1 : currLevel;
		}
	}

	loadLevel();
}

void Game::loadLevel()
{
	std::string line;
	std::ifstream file;
	std::string levelFileStr = LEVEL_FILE + std::to_string(level) + LEVEL_FILE_EXT;
	file.open(levelFileStr, std::ios::in);
	if (file.is_open())
	{
		std::getline(file, line);

		std::string::size_type sz;
		int numProbes = std::stoi(line, &sz);

		std::vector<int> probes;
		std::getline(file, line);
		sz = 0;
		std::string str = line;
		for (int i = 0; i < numProbes; ++i)
		{
			str = str.substr(sz);
			int probe = std::stoi(str, &sz);
			probes.push_back(probe);
		}

		if (board == nullptr)
		{
			board = new Board(probes);
		}
		else
		{
			board->setProbes(probes);
		}

		std::getline(file, line);
		sz = 0;
		numMoves = std::stoi(line.substr(sz), &sz);
		numInitMoves = numMoves;

		std::getline(file, line);
		sz = 0;
		objScore = std::stoi(line.substr(sz), &sz);

		file.close();
	}

	//set dynamic texts
	levelStr.setAddText(std::to_string(level));
	objStr.setAddText(std::to_string(objScore));
	movesStr.setAddText(std::to_string(numMoves));
	scoreStr.setAddText(std::to_string(score));

	//show level and objective
	renderer->drawText(levelStr);
	renderer->drawText(objStr);

	//show left moves
	renderer->drawText(movesStr);

	//show score
	renderer->drawText(scoreStr);
}

void Game::saveGame()
{
	std::ofstream file;
	file.open(SAVE_FILE, std::ios::out | std::ios::app);
	file << level << " " << score << std::endl;
	file.close();
}

void Game::reset()
{
	score = 0;
	loadLevel();
}