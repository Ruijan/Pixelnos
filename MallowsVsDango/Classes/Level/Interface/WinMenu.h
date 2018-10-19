#pragma once
#include "../../GUI/CentralMenu.h"
#include "../../Config/json.h"

class MyGame;

class WinMenu : public CentralMenu {
public:
	static WinMenu* create(MyGame *game);
	void showWin();

protected:
	virtual bool init(MyGame* game);
	void addTowerExperiences(cocos2d::Size &visibleSize);
	void addWinMallowsImages();
	void addTowerLoadingExp(std::string & towerName, cocos2d::Size &visibleSize, Json::Value &root, Json::Value &towerConfig);
	void addRewardSugar(Json::Value &buttons, std::string &language, cocos2d::Size &visibleSize);
	void addStars();
	void addYouWinLabel(Json::Value &buttons, std::string &language, cocos2d::Size &visibleSize);
	virtual void rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	virtual void leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updateIncrementXP(cocos2d::Label* exp_label, cocos2d::ui::LoadingBar* loading_bar, std::string tower_name,
		float* increment, int initial_value, int diff_exp, int loop, int max_level);

protected:
	cocos2d::Vec2 previousObjectPos;
	cocos2d::Size previousObjectSize;

	MyGame* game;
	cocos2d::Label* youWinLabel;
	cocos2d::Label* rewardSugarValueLabel;
};