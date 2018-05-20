#pragma once
#include "cocos2d.h"

class InterfaceGame;

class Tutorial {
public:
	Tutorial();
	virtual void update(float dt) = 0;
	void shakeElement(cocos2d::Node * element, bool loop);
	void shakeScaleElement(cocos2d::Node * element, bool loop);
	virtual bool isDone() = 0;
	virtual ~Tutorial();

protected:
	virtual void endTutorial();

protected:
	bool running;
};