#pragma once
#include "../MyGame.h"
#include "../../../GUI/ParametersMenu.h"

class LevelParametersMenu : public ParametersMenu {
public:
	static LevelParametersMenu* create(MyGame* game, Config* config);
	LevelParametersMenu(MyGame * game, Config * config);

	virtual void handleAlwaysShowGridButton(cocos2d::ui::Widget::TouchEventType type);
	virtual void handleMovingGridButton(cocos2d::ui::Widget::TouchEventType type);
	virtual void handleNeverShowGridButton(cocos2d::ui::Widget::TouchEventType type);
	virtual void rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	virtual void leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
protected:
	MyGame * game;
};