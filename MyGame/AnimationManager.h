#ifndef ANIMATION_H
#define ANIMATION_H

//a class for animating objects

enum class Direction
{
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

template <class T>
class AnimationManager
{
public:
	AnimationManager() : mMoveStep(0) {};

	void invertDirection(T* obj) const;
	void computeNextCoordinates(const T* const obj, int& x, int& y) const;
	void setMoveStep(int moveStep) { mMoveStep = moveStep;  }

	void beginSwapAnimation(T* obj0, T* obj1) const;
	void continueMovement(T* obj) const;

private:
	int mMoveStep;
};

#endif