#include <unordered_set>
#include "Game.h"
#include <fstream>
#include <iostream>

Game Game::_instance;
int Game::score = 0;
int Game::objScore = 0;
int Game::level = 0;
int Game::numMoves = 20;
int Game::numInitMoves = 0;
bool Game::stop = false;
const std::string Game::saveFile {"./save/data.txt"};
const std::string Game::levelFile{"./levels/level"};
const std::string Game::levelFileExt{".txt"};

TextRenderable Game::levelStr { "Level: " + std::to_string(level), 10, 10, 16, 16, 24};
TextRenderable Game::objStr { "Objective: " + std::to_string(objScore), 370, 10, 16, 16, 24 };
TextRenderable Game::movesStr { "Moves left: " + std::to_string(numMoves), 300, 50, 16, 16, 24 };
TextRenderable Game::scoreStr {"Score: " + std::to_string(score), 30, 50, 16, 16, 24};
TextRenderable Game::resultStr {"Game over", 100, 250, 16, 16, 24};

void Game::init()
{
	renderer = new Renderer();
	renderer->init();

	loadGame();

	//load background
	renderer->drawBackground("./assets/background/blue_backing.png");

	//show level
	renderer->setFontType("./fonts/ANUDI.ttf", 24);

	//show level and objective
	renderer->drawText(levelStr);
	renderer->drawText(objStr);

	//show num moves
	renderer->drawText(movesStr);

	//show score
	renderer->drawText(scoreStr);
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
	//std::string scoreStr("Score:  ");
	//scoreStr+=std::to_string(score);
	renderer->drawText(scoreStr);
}

void Game::updateNumMoves()
{
	numMoves--;
	//std::string scoreStr("Moves left:  ");
	//scoreStr += std::to_string(numMoves);
	renderer->drawText(scoreStr);
}

void Game::checkState()
{
	if (score > objScore)
	{
		saveGame();
		//std::string gameStr("Level Completed");
		renderer->drawText(resultStr);
		stop = true;

		level++;
	}
	else
		if (numMoves == 0)
		{
			//renderer->clear(100, 100, 250, 250);
			//std::string gameStr("Game Over");
		renderer->drawText(resultStr);

		//	std::string tryStr("Click to try again");
		//	renderer->drawText(tryStr, 100, 290);

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
		//level++;
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

		if (board != nullptr)
			delete board;

		board = new Board(probes);

		std::getline(file, line);
		sz = 0;
		numMoves = std::stoi(line.substr(sz), &sz);
		numInitMoves = numMoves;

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

void Game::reset()
{
	if (score >= objScore)
	{
		loadLevel();

		//show level and objective
		/*std::string levelStr("Level: ");
		levelStr += std::to_string(level);
		renderer->drawText(levelStr, 10, 10);

		std::string objStr("Objective: ");
		objStr += std::to_string(objScore);
		renderer->drawText(objStr, 370, 10);*/
		renderer->drawText(levelStr);
		renderer->drawText(objStr);
	}

	score = 0;
	numMoves = numInitMoves;

	//show num moves
	//updateNumMoves();
	/*std::string movesStr("Moves left:  ");
	movesStr += std::to_string(numMoves);
	renderer->drawText(movesStr, 300, 50);*/
	renderer->drawText(movesStr);

	//show score
	//updateScore(0);
	/*std::string scoreStr("Score:  ");
	scoreStr += std::to_string(score);
	renderer->drawText(scoreStr, 30, 50);*/
	renderer->drawText(scoreStr);
}