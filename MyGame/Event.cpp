#include "Event.h"
#include "Game.h"

void EventMove::update() const
{
	Game::instance().updateNumMoves();
}

void EventScore::update() const
{
	Game::instance().updateScore(this->getParam());
}

void EventCheckState::update() const
{
	Game::instance().checkState();
}