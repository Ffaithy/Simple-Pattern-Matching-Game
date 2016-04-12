#ifndef GAME_H
#define GAME_H

#include "Renderer.h"
#include "Board.h"
#include "Event.h"

class Game
{
	//events for updating Game state
	friend class EventMove;
	friend class EventScore;
	friend class EventCheckState;

private:
	Game() = default;

public:
	static Game& instance() { return _instance; }

	Renderer& getRenderer() { return *renderer; }
	Board& getBoard() { return *board; }

	//getters for Game members
	int getScore() const { return score; }
	int getLevel() const { return level; }
	int getNumMoves() const  { return numMoves; }
	bool getStop() const { return stop; }

	void onNotify(Event* ev);
	void init();
	void close();
	void handleInput(int x = 0, int y = 0);
	void update();

private:
	static Game _instance;

	static const int MAX_LEVEL;

	static const std::string SAVE_FILE;
	static const std::string LEVEL_FILE;
	static const std::string LEVEL_FILE_EXT;
	static const std::string BACKGROUND_IMG;
	static const std::string FONT_TYPE;

	//static members for the texts 
	static TextRenderable levelStr;
	static TextRenderable objStr;
	static TextRenderable movesStr;
	static TextRenderable scoreStr;
	static TextRenderable resultStr;
	static TextRenderable tryAgainStr;

	//static member for the game state
	static int score;
	static int numMoves;
	static int numInitMoves;
	static int level;
	static int objScore;
	static bool stop;


	Renderer* renderer;
	Board* board;

	void updateScore(int points);
	void updateNumMoves();
	void checkState();

	void saveGame();
	void loadGame();
	void loadLevel();
	void continueGame() { stop = false; }
	void reset();
};

#endif