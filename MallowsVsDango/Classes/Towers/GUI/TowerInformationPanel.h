#pragma once
#include "cocos2d.h"
#include "../../Config/json.h"
#include "../../Config/Config.h"
#include "ui/CocosGUI.h"
#include "CurrentLevelPanel.h"
#include "NextLevelPanel.h"

class Tower;
class MyGame;

class TowerInformationPanel: public cocos2d::ui::Layout {
public:
	~TowerInformationPanel();
	static TowerInformationPanel* create(MyGame* game, Tower* tower, Config* config);
	bool init(MyGame* game, Tower* tower, Config* config);
	void setTower(Tower * newTower);
	void update();

protected:
	void createMainPanel(cocos2d::Size &visibleSize);
	void createCurrentLevelPanel(cocos2d::Size &visibleSize);
	void setPanelPosition(cocos2d::Size &visibleSize);
	void createNextLevelPanel(cocos2d::Size &visibleSize);
	void createLockLayout(cocos2d::Size &visibleSize);
	void createDescriptionLayout(cocos2d::Size &visibleSize);
	void createNextLevelButton(cocos2d::Size &visibleSize);
	void createSellButton(cocos2d::Size &visibleSize);
	void createMaxLevelLabel(cocos2d::Size &visibleSize);

	void updateDisplay();
	void updateDisplayNextLevel();

protected:
	Tower * tower;
	MyGame * game;
	Config* configClass;

	double spriteWidth;

	cocos2d::ui::Button* mainPanel;
	CurrentLevelPanel* currentLevelInfos;
	NextLevelPanel* nextLevelInfos;

	cocos2d::Layer* descriptionLayout;
	cocos2d::Layer* lockedLayout;

	cocos2d::ui::Button* nextLevelButton;
	cocos2d::Label* maxLevelLabel;
};