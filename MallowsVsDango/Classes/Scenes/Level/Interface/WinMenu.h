#pragma once
#include "../../../GUI/CentralMenu.h"
#include "../../../Config/json.h"
#include "../../../GUI/GUISettings.h"

class MyGame;
class Level;

class WinMenu : public CentralMenu {
public:
	static WinMenu* create(MyGame *game, GUISettings* settings);
	void showWin();
	void showIncreasingTowerExperience(std::string &tower_name, Json::Value &root, Level * cLevel);

protected:
	virtual bool init(MyGame* game, GUISettings* settings);
	void addTowerExperiences();
	void addWinMallowsImages();
	void addTowerLoadingExp(std::string & towerName, Json::Value &root, Json::Value &towerConfig);
	void addRewardSugar();
	void addStars();
	void addYouWinLabel();
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
	GUISettings* settings;
};