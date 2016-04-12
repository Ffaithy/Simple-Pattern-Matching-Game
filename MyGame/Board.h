#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <unordered_set>
#include <cstdint>

class Board
{
	enum class Direction
	{
		NONE,
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	class Cell
	{
	public:
		Cell() : x{ 0 }, y{ 0 }, type{ EMPTY }, selected{ 0 }, direction{ Board::Direction::NONE } {};
		Cell(int ptype, int px, int py) : x{ px }, y{ py }, type{ ptype }, selected{ false }, explode{ false }, direction{ Board::Direction::NONE }{};

		//getters
		bool isSelected() const { return this->selected; }
		bool isExplode() const { return explode; }
		int getType() const { return type; }
		std::string getName() const { return getName(type); };
		int getX() const { return x; }
		int getY() const { return y; }
		int getRow() const { return x / width; }
		int getCol() const { return y / height; }
		Direction getDirection() const { return this->direction; }

		//setters
		void setSelected(bool selected) { this->selected = selected; }
		void setExplode(bool e) { this->explode = e; }
		void setType(int type) { this->type = type; }
		void setX(int x) { this->x = x; }
		void setY(int y) { this->y = y; }
		void setDirection(Direction dir) { this->direction = dir; }

		void render() const;
		
		static int getWidth() { return width; }
		static int getHeight() { return height;  }
		static void setWidth(int pwidth) { width = pwidth; }
		static void setHeight(int pheight) { height = pheight; }
		static std::string getName(int type) { return NAME_PREFIX + std::to_string(type) + NAME_SUFIX; }

		const static int DEFAULT_WIDTH;
		const static int DEFAULT_HEIGHT;
		const static std::string NAME_PREFIX; 
		const static std::string NAME_SUFIX;
		const static std::string SELECTOR;
		const static int EMPTY;

	private:
		static int width;
		static int height;
		mutable int type;
		int x;
		int y;
		bool selected;
		mutable bool explode;
		Board::Direction direction;
	};

	enum class State
	{
		STATE_NONE,
		STATE_SWAP,
		STATE_INVALID_SWAP,
		STATE_EXPLODE,
		STATE_FILL
	};

public:
	Board(const std::vector<int>& probes, int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);
	
	~Board() { probes.clear(); cells.clear(); }
	void generate();
	void initRenderer() const;
	void render();
	void renderGenerator();
	void handleInput(int x, int y);

	//Matching logic 

	//these functions check for matches
	bool isSwapValid() const;
	bool isPieceInARowMatch(int row, int col, int type) const;
	bool isPieceInAColMatch(int row, int col, int type) const;

	//these functions check for matches and mark the matched cells
	bool rowHasMatches(int row);
	bool colHasMatches(int col);
	bool boardHasMatches();

	void update();

	//Swap Animation
	void beginSwapAnimation();
	void continueSwapAnimation() const;
	void endSwap();

	//Falling animation
	void startFall();
	void animateFall();
	bool endFall();

	//General animation
	//Compute next coordinates of a cell, based on its direction
	void computeCoordinates(const Cell* cell, int& x, int& y) const;

private:
	const static int DEFAULT_WIDTH = 9;
	const static int DEFAULT_HEIGHT = 9;
	const static int NUM_MATCHES = 3;
	const static int OFFSET_Y;
	const static int OFFSET_X;

	const static int NUM_EXPLODE_STEPS = 30;
	const static uint8_t EXPLODE_DIFF_STEP = 8;
	const static uint8_t MAX_ALPHA = 255;
	const static uint8_t INIT_ALPHA = 240;

	static int STEP;
	static State STATE;
	
	//Swap animation
	const static int NUM_SWAP_STEPS = 30;
	const static int SWAP_DIFF_STEP = 2;

	const int width;
	const int height;
	std::vector<int> probes;
	std::vector<std::vector<Cell>> cells;
	Cell* selected0;
	Cell* selected1;
	std::unordered_set<int> linesToExplode;
	std::unordered_set<int> colsToExplode;
	std::vector<Cell> generator;
};

#endif