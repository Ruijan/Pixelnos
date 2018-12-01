#pragma once
#include "../../GUI/ParametersMenu.h"
#include "../../Config/Config.h"

class StoryParametersMenu : public ParametersMenu {
public:
	StoryParametersMenu(Config* config);
	static StoryParametersMenu* create(Config* config);
	virtual ~StoryParametersMenu();
	virtual bool init();

protected:
	virtual void rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	virtual void leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	virtual void addBottomButtons(Json::Value & buttons);

};

