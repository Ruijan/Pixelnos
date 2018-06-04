#pragma once
#include "ParametersMenu.h"

class StoryParametersMenu : public ParametersMenu {
public:
	static StoryParametersMenu* create(MyGame* game);
	virtual ~StoryParametersMenu();
	virtual bool init(MyGame* game);

protected:
	virtual void rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	virtual void leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	virtual void addBottomButtons(Json::Value & buttons, std::string & language);

};

