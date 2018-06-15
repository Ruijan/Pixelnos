#pragma once
#include "ParametersMenu.h"
#include "../Config/Config.h"

class StoryParametersMenu : public ParametersMenu {
public:
	StoryParametersMenu(MyGame* game, Config* config);
	static StoryParametersMenu* create(MyGame* game, Config* config);
	virtual ~StoryParametersMenu();
	virtual bool init();

protected:
	virtual void rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	virtual void leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	virtual void addBottomButtons(Json::Value & buttons);

};

