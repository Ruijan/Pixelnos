#pragma once
#include "cocos2d.h"
#include "../Config/json.h"
#include "../Config/Config.h"
#include "ui/CocosGUI.h"

class Tower;
class MyGame;

class TowerInformationPanel: public cocos2d::ui::Layout {
public:
	static TowerInformationPanel* create(MyGame* game, Tower* tower, Config* config);
	bool init(MyGame* game, Tower* tower, Config* config);
	void createMainPanel();
	void update();

protected:
	void createCurrentLevelPanel();
	void createNextLevelPanel();
	void createLockLayout();
	void createDescriptionLayout();
	void createNextLevelButton();
	void createSellButton();
	void createMaxLevelLabel();
	void updateDisplay();

protected:
	Tower * tower;
	MyGame * game;
	Config* configClass;

	double spriteSize;

	cocos2d::ui::Button* mainPanel;
	cocos2d::ui::Layout* currentLevelInfos;
	cocos2d::Label* currentLevelLabel;
	cocos2d::Label* currentAttackLabel;
	cocos2d::Label* currentSpeedLabel;
	cocos2d::Label* currentRangeLabel;

	cocos2d::ui::Layout* nextLevelInfos;
	cocos2d::Label* nextLevelLabel;
	cocos2d::Label* nextAttackLabel;
	cocos2d::Label* nextSpeedLabel;
	cocos2d::Label* nextRangeLabel;
	cocos2d::Label* nextCostLabel;

	cocos2d::Layer* descriptionLayout;
	cocos2d::Layer* lockedLayout;

	cocos2d::ui::Button* nextLevelButton;
	cocos2d::Label* maxLevelLabel;
};