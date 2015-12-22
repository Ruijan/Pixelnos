#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "cocos2d.h"
#include "Lib/Loader.h"


class MainMenu :public cocos2d::Scene, public Loader
{
public:
	virtual bool init();
	CREATE_FUNC(MainMenu);
	void menuContinueCallback(cocos2d::Ref* sender);
	void update(float dt);
	enum State{
		marshmallowStart,
		marshmallowBouncing1,
		marshmallowBouncing2,
		marshmallowBouncing3,
		dangoStart,
		dangoBouncing1,
		dangoBouncing2,
		dangoBouncing3,
		vsStart,
		flashStart,
		flashEnd,
		menuShow,
		finished
	};

protected:
	virtual void onEnter();
	virtual void onEnterTransitionDidFinish();

private:
	cocos2d::Menu* menu;
	cocos2d::Sprite* dango;
	cocos2d::Sprite* marshmallow;
	cocos2d::Sprite* vs;
	cocos2d::Sprite* bg1;
	cocos2d::Sprite* bg2;
	cocos2d::Sprite* flash;
	State state;
};

#endif
