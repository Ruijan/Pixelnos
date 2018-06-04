#pragma once
#include "ui/CocosGUI.h"
#include "cocos2d.h"
#include "../Config/json.h"


class MyGame;
class LoseMenu : public cocos2d::ui::Layout {
public:
	static LoseMenu* create(MyGame *game);
	void showLose();

protected:
	virtual bool init(MyGame* game);

	void addAdvice(std::string &language, cocos2d::Size &visibleSize);
	void addYouLoseLabel(Json::Value &buttons, std::string &language, cocos2d::Size &visibleSize);
	void addMainMenuButton(cocos2d::Size &visibleSize, Json::Value &buttons, std::string &language);
	void addRetryButton(cocos2d::Size &visibleSize, Json::Value &buttons, std::string &language, MyGame * game);
	void addPanel(cocos2d::Size &visibleSize);

	static cocos2d::TargetedAction* createHideAction(cocos2d::Node* target);
	static cocos2d::Sprite* createButtonShadow(cocos2d::ui::Button* button);

protected:
	cocos2d::ui::Button* panel;
	MyGame* game;
};