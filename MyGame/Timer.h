#include <cstdint>

//The application time based timer
class Timer
{
public:
	//Initializes variables
	Timer();

	//The various clock actions
	void start();
	void stop();
	void pause();
	void unpause();

	//Gets the timer's time
	unsigned int getTicks();

	//Checks the status of the timer
	bool isStarted();
	bool isPaused();

private:
	//The clock time when the timer started
	uint32_t mStartTicks;

	//The ticks stored when the timer was paused
	uint32_t mPausedTicks;

	//The timer status
	bool mPaused;
	bool mStarted;
};