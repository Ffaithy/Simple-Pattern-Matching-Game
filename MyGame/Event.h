#ifndef EVENT_H
#define EVENT_H

//Game Event for updating its state
//These events are sent from the Board to the game instance
//The event has one optional parameter

class Event
{
public:
	Event(int p = -1) : param(p) {}
	int getParam() const { return param; }
	void setParam(const int p) { this->param = p; }
	virtual void update() const = 0;
private:
	int param;
};

class EventMove : public Event
{
public:
	virtual void update() const override;
};

class EventScore : public Event
{
public:
	EventScore(int p = -1) : Event(p) {};
	virtual void update() const override;
};

class EventCheckState : public Event
{
public:
	virtual void update() const override;
};

#endif