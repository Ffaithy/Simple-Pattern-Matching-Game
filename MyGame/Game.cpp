#include <unordered_set>
#include "Game.h"
#include <fstream>
#include <iostream>

Game Game::_instance;
int Game::score = 0;
int Game::objScore = 0;
int Game::level = 0;
int Game::numMoves = 20;
bool Game::stop = false;
const std::string Game::saveFile {"./save/data.txt"};
const std::string Game::levelFile{"./levels/level"};
const std::string Game::levelFileExt{".txt"};

void Game::init()
{
	renderer = new Renderer();
	renderer->init();

	loadGame();

	//load background
	renderer->loadBackground("./assets/background/blue_backing.png");

	//show level
	renderer->setFontType("./fonts/ANUDI.ttf", 24);

	//show num moves
	updateNumMoves();
	//show score
	updateScore(0);
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
	std::string scoreStr("Score:  ");
	scoreStr+=std::to_string(score);
	renderer->drawText(scoreStr, 30, 50);
}

void Game::updateNumMoves()
{
	numMoves--;
	std::string scoreStr("Moves left:  ");
	scoreStr += std::to_string(numMoves);
	renderer->drawText(scoreStr, 300, 50);
}

void Game::checkState()
{
	if (score > objScore)
	{
		saveGame();
		std::string gameStr("Level Completed");
		renderer->drawText(gameStr, 250, 250);
		stop = true;
	}
	else
		if (numMoves == 0)
		{
			std::string gameStr("Game Over");
			renderer->drawText(gameStr, 250, 250);
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

void Game::loadGame()
{
	std::string line;
	std::ifstream file;
	file.open(saveFile, std::ios::in);
	
	if (file.is_open())
	{
		int currLevel = 0;
		int maxLevel = currLevel;
		while (std::getline(file, line))
		{
			std::string::size_type sz;   // alias of size_t
			int currLevel = std::stoi(line, &sz);
			if (currLevel > maxLevel)
			{
				maxLevel = currLevel;
			}
		}
		file.close();
		level = maxLevel;
		level++;
	}
	else
	{
		level = 0;
	}

	loadLevel();
}

void Game::loadLevel()
{
	std::string line;
	std::ifstream file;
	std::string levelFileStr = levelFile + std::to_string(level) + levelFileExt;
	file.open(levelFileStr, std::ios::in);
	if (file.is_open())
	{
		std::getline(file, line);

		std::string::size_type sz;   // alias of size_t
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

		board = new Board(probes);

		std::getline(file, line);
		sz = 0;
		numMoves = std::stoi(line.substr(sz), &sz);

		std::getline(file, line);
		sz = 0;
		objScore = std::stoi(line.substr(sz), &sz);

		file.close();
	}
}

void Game::saveGame()
{
	std::ofstream file;
	file.open(saveFile, std::ios::out | std::ios::app);
	file << level << " " << score << std::endl;
	file.close();
}