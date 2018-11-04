#pragma once
#include "../../../GUI/CentralMenu.h"
#include "../../../Config/json.h"


class MyGame;
class LoseMenu : public CentralMenu {
public:
	static LoseMenu* create(MyGame *game, const std::string& language, const Json::Value& buttons, const Json::Value& advice);
	void showLose();

protected:
	virtual bool init(MyGame* game, const std::string& language, const Json::Value& buttons, const Json::Value& advice);

	void addAdvice(const std::string &language, cocos2d::Size &visibleSize, const Json::Value& advice);
	void addYouLoseLabel(const Json::Value &buttons, const std::string &language, cocos2d::Size &visibleSize);
	virtual void rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	virtual void leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

protected:
	MyGame* game;
};