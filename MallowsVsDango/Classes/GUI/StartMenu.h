#pragma once
#include "ui/CocosGUI.h"
#include "cocos2d.h"
#include "../Level/InterfaceGame.h"


class StartMenu : public cocos2d::ui::Layout {
public:
	StartMenu();
	static StartMenu* create(InterfaceGame* interfaceGame, int levelId);
	void displayWithAnimation();
	virtual void hide();
	virtual ~StartMenu();
	virtual void reset(int levelId);

protected:
	InterfaceGame* interfaceGame;
	bool hidden;

protected:
	void startButtonCallback(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	virtual bool init(InterfaceGame* interfaceGame, int levelId);
	virtual void addTitleLabel(Json::Value &buttons, std::string &language, int levelId, cocos2d::Size &visibleSize);
	virtual void addAdviceLabel(Json::Value &buttons, std::string &language, cocos2d::Size &visibleSize);
	virtual void addStartButton(cocos2d::Size &visibleSize, Json::Value &buttons, std::string &language);
};