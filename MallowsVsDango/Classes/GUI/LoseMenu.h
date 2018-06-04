#pragma once
#include "CentralMenu.h"
#include "../Config/json.h"


class MyGame;
class LoseMenu : public CentralMenu {
public:
	static LoseMenu* create(MyGame *game);
	void showLose();

protected:
	virtual bool init(MyGame* game);

	void addAdvice(std::string &language, cocos2d::Size &visibleSize);
	void addYouLoseLabel(Json::Value &buttons, std::string &language, cocos2d::Size &visibleSize);
	virtual void rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	virtual void leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

protected:
	MyGame* game;
};