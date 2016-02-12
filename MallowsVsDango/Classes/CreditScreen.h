#ifndef CREDITSCREEN_HPP
#define CREDITSCREEN_HPP

#include "cocos2d.h"

class CreditScreen : public cocos2d::Scene
{
public:
	virtual bool init();
	CREATE_FUNC(CreditScreen);
	virtual void onEnterTransitionDidFinish();

	void returnToGame(Ref* sender);
	int countLine(std::string text);

private:
	cocos2d::Menu* menu;
	//State state;
};

#endif
