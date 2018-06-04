#pragma once
#include "ui/CocosGUI.h"
#include "cocos2d.h"
#include "../Config/json.h"

class MyGame;

class WinMenu : public cocos2d::ui::Layout {
public:
	static WinMenu* create(MyGame *game);
	void showWin();

protected:
	virtual bool init(MyGame* game);
	void addTowerExperiences(cocos2d::Size &visibleSize);
	void addWinMallowsImages();
	void addTowerLoadingExp(std::string & towerName, cocos2d::Size &visibleSize, Json::Value &root);
	void addHolySugar();
	void addRewardSugar(Json::Value &buttons, std::string &language, cocos2d::Size &visibleSize);
	void addStars();
	void addYouWinLabel(Json::Value &buttons, std::string &language, cocos2d::Size &visibleSize);
	void addPanel(cocos2d::Size &visibleSize);
	void addMainMenuButton(cocos2d::Size &visibleSize, Json::Value &buttons, std::string &language);
	void addNextLevelButton(cocos2d::Size &visibleSize, Json::Value &buttons, std::string &language);
	void updateIncrementXP(cocos2d::Label* exp_label, cocos2d::ui::LoadingBar* loading_bar, std::string tower_name,
		float* increment, int initial_value, int diff_exp, int loop, int max_level);

	static cocos2d::TargetedAction* createHideAction(cocos2d::Node* target);
	static cocos2d::Sprite* createButtonShadow(cocos2d::ui::Button* button);

protected:
	cocos2d::Vec2 previousObjectPos;
	cocos2d::Size previousObjectSize;

	MyGame* game;
	cocos2d::ui::Button* panel;
	cocos2d::Label* youWinLabel;
	cocos2d::Label* rewardSugarValueLabel;
};