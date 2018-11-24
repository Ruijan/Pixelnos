#pragma once
#include "../../../GUI/CentralMenu.h"
#include "../../../Config/json.h"
#include "../../../GUI/GUISettings.h"

class MyGame;
class LoseMenu : public CentralMenu {
public:
	static LoseMenu* create(MyGame *game, GUISettings* settings, const Json::Value& advice);
	void showLose();

protected:
	virtual bool init(MyGame* game, GUISettings* settings, const Json::Value& advice);

	void addAdvice(const Json::Value& advice);
	void addYouLoseLabel();
	virtual void rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	virtual void leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

protected:
	MyGame* game;
	GUISettings* settings;
};