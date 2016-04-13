#include <cstdint>

class Timer
{
public:
	Timer();

	void start();
	void stop();
	void pause();
	void unpause();

	unsigned int getTicks();

	bool isStarted();
	bool isPaused();

private:
	uint32_t mStartTicks;
	uint32_t mPausedTicks;

	bool mPaused;
	bool mStarted;
};