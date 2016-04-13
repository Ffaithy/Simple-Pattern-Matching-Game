#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <unordered_set>
#include <cstdint>
#include "AnimationManager.h"
#include "AnimationManager.cpp"
#include "Cell.h"

class Board
{
	enum class State
	{
		STATE_NONE,
		STATE_SWAP,
		STATE_INVALID_SWAP,
		STATE_EXPLODE,
		STATE_FALL
	};

public:
	Board(const std::vector<int>& mProbes, int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);
	
	~Board() { mProbes.clear(); mCells.clear(); }

	void setProbes(const std::vector<int>& probes);
	//generate the cells randomly
	void generate();	
	//make renderer load the necessary media 
	void initRenderer() const; 
	//draw the board on screen
	void render();		
	//initializes the additional row for filling blank spaces on the board
	void renderGenerator();
	//Handle the mouse click
	void handleInput(int x, int y);
	//Update the state of the Board
	void update();

	/** MATCHING LOGIC **/

	//these function returns true if a swap is valid and also performs the swap
	bool isSwapValid();

	//these functions check for matches and mark the matched cells
	bool rowHasMatches(int row);
	bool colHasMatches(int col);
	bool boardHasMatches();
	//computes the score based on the cells marked as to be exploded
	void computeScore();

	/** ANIMATIONS */

	//Swap Animation
	void continueSwapAnimation() const;
	void endSwap();

	//Falling animation
	void startFall();
	void animateFall();
	bool endFall();

private:
	const static int DEFAULT_WIDTH = 9;
	const static int DEFAULT_HEIGHT = 9;
	const static int NUM_MATCHES = 3;
	const static int OFFSET_Y;
	const static int OFFSET_X;

	//Explode animation constant
	const static int NUM_EXPLODE_STEPS = 30;

	//track the animation
	static int STEP;
	//store the current state
	static State STATE;
	
	//Swap animation constants
	const static int NUM_SWAP_STEPS = 15;
	const static int SWAP_DIFF_STEP = 4;

	//Data members
	const int width;
	const int height;
	std::vector<int> mProbes;
	std::vector<std::vector<Cell>> mCells;
	Cell* mSelected0;
	Cell* mSelected1;
	//Component for moving the cells on the screen
	AnimationManager<Cell> mAnimationManager;
	//Caches the lines that have matches
	std::unordered_set<int> mLinesToExplode;
	//Caches the columns that have matches
	std::unordered_set<int> mColsToExplode;
	//Generates new random values to fill the empty cells on board
	std::vector<Cell> mGenerator;
	//Input enabler
	bool mBlockInput;
};

#endif