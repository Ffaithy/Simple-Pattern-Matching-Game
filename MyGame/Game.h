#ifndef GAME_H
#define GAME_H

#include "Renderer.h"
#include "Board.h"
#include "Event.h"

class Game
{
	//Events for updating Game state
	friend class EventMove;
	friend class EventScore;
	friend class EventCheckState;

private:
	Game() = default;
	Game(const Game&) = delete;
	Game& operator=(Game) = delete;

public:
	static Game& instance() { return _instance; }

	//Service locator for accesing the Renderer
	Renderer& getRenderer() { return *mRenderer; }

	//getters for Game members
	int getScore() const { return mScore; }
	int getLevel() const { return mLevel; }
	int getNumMoves() const  { return mNumMoves; }
	bool getStop() const { return mStop; }

	//Methods to modify the Game state
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

	//Static members for the texts that are rendered in the window
	static TextRenderable mLevelStr;
	static TextRenderable mObjStr;
	static TextRenderable mMovesStr;
	static TextRenderable mScoreStr;
	static TextRenderable mResultStr;
	static TextRenderable mTryAgainStr;

	//Static members for the game state
	static int mScore;
	static int mNumMoves;
	static int mNumInitMoves;
	static int mLevel;
	static int mObjScore;
	static bool mStop;

	Renderer* mRenderer;
	Board* mBoard;

	//private methods for updating the Game state
	void updateScore(int points);
	void updateNumMoves();
	void checkState();

	//private methods for loading/saving the Game state
	void saveGame();
	void loadGame();
	void loadLevel();
	void continueGame() { mStop = false; }
	void reset();
};

#endif