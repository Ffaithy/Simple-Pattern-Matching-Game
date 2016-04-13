#include "AnimationManager.h"

template <class T>
void AnimationManager<T>::invertDirection(T *obj) const
{
	switch (obj->getDirection())
	{
	case Direction::UP:
		obj->setDirection(Direction::DOWN);
		break;
	case Direction::DOWN:
		obj->setDirection(Direction::UP);
		break;
	case Direction::LEFT:
		obj->setDirection(Direction::RIGHT);
		break;
	case Direction::RIGHT:
		obj->setDirection(Direction::LEFT);
		break;
	default:
		obj->setDirection(Direction::NONE);
	}
}

template <class T>
void AnimationManager<T>::computeNextCoordinates(const T* const obj, int& x, int& y) const
{
	if (obj == nullptr)
		return; 

	switch (obj->getDirection())
	{
	case Direction::UP:
		x = obj->getX();
		y = obj->getY() - mMoveStep;
		break;
	case Direction::DOWN:
		x = obj->getX();
		y = obj->getY() + mMoveStep;
		break;
	case Direction::LEFT:
		x = obj->getX() - mMoveStep;
		y = obj->getY();
		break;
	case Direction::RIGHT:
		x = obj->getX() + mMoveStep;
		y = obj->getY();
		break;
	default:
		;
	}
}

template <class T>
void  AnimationManager<T>::beginSwapAnimation(T* obj0, T* obj1) const
{
	if (obj0 == nullptr || obj1 == nullptr)
		return;

	if (obj0->getY() == obj1->getY()) ///swap on column
	{
		T *min = (obj0->getX() < obj1->getX()) ? obj0 : obj1;
		T *max = (obj0->getX() < obj1->getX()) ? obj1 : obj0;

		min->setDirection(Direction::RIGHT);
		max->setDirection(Direction::LEFT);
	}
	else
		if (obj0->getCol() == obj1->getCol()) //swap on line
		{
			T *min = (obj0->getRow() < obj1->getRow()) ? obj0 : obj1;
			T *max = (obj0->getRow() < obj1->getRow()) ? obj1 : obj0;

			min->setDirection(Direction::DOWN);
			max->setDirection(Direction::UP);
		}
}


template <class T>
void  AnimationManager<T>::continueMovement(T* obj) const
{
	//compute new coordinates
	int x = 0;
	int y = 0;
	computeNextCoordinates(obj, x, y);

	obj->setX(x);
	obj->setY(y);
}