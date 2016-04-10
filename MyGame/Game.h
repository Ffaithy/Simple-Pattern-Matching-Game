#ifndef GAME_H
#define GAME_H

#include "Renderer.h"
#include "Board.h"
#include "Event.h"

class Game
{
	friend class EventMove;
	friend class EventScore;
	friend class EventCheckState;
public:
	static Game& instance() { return _instance; }

	Renderer& getRenderer() { return *renderer; }
	Board& getBoard() { return *board; }
	int getScore() { return score; }
	int getLevel() { return level; }
	int getNumMoves() { return numMoves; }
	void onNotify(Event* ev);
	void saveGame();
	void loadGame();
	void loadLevel();
	bool getStop() const { return stop; }

	void init();
	void close();
private:
	static Game _instance;

	static int score;
	static int numMoves;
	static int level;
	static int objScore;
	static bool stop;

	Renderer* renderer;
	Board* board;

	void updateScore(int points);
	void updateNumMoves();
	void checkState();

	static const std::string saveFile;
	static const std::string levelFile;
	static const std::string levelFileExt;
};

#endif