#ifndef STORYMENU_HPP
#define STORYMENU_HPP

#include "cocos2d.h"


class StoryMenu : public cocos2d::Scene
{
	public:
	virtual bool init();
	CREATE_FUNC(StoryMenu);
	
	private:
	cocos2d::Menu* menu;
	cocos2d::Sprite* bg1;
	State state;
};

#endif
