#pragma once
#include "cocos2d.h"
#include "../../../../Config/json.h"
#include "../../../../Config/Config.h"
#include "ui/CocosGUI.h"
#include "../TowerSettings.h"
#include "CurrentLevelPanel.h"
#include "NextLevelPanel.h"
#include "../../../../GUI/GUISettings.h"

class Tower;
class MyGame;
class Level;
class LevelInterface;

class TowerInformationPanel: public cocos2d::ui::Layout {
public:
	~TowerInformationPanel();
	static TowerInformationPanel* create(MyGame* game, Tower* tower, GUISettings* settings);
	bool init(MyGame* game, Tower* tower, GUISettings* settings);
	void setTower(Tower * newTower);
	void update();

protected:
	void createMainPanel(cocos2d::Size &visibleSize);
	void createCurrentLevelPanel(cocos2d::Size &visibleSize);
	void setPanelPosition(const cocos2d::Size &visibleSize);
	void createNextLevelPanel(cocos2d::Size &visibleSize);
	void createLockLayout(cocos2d::Size &visibleSize);
	void createDescriptionLayout(cocos2d::Size &visibleSize);
	void createNextLevelButton(cocos2d::Size &visibleSize);
	void addTowerActionToTracker(std::string towerAction);
	void createSellButton(cocos2d::Size &visibleSize);
	void createMaxLevelLabel(cocos2d::Size &visibleSize);

	void updateDisplay();
	void updateDisplayNextLevel();

protected:
	Tower * tower;
	MyGame * game;
	Level* level;
	LevelInterface* menu;
	TowerSettings* settings;
	GUISettings* guiSettings;
	double spriteWidth;

	cocos2d::ui::Button* mainPanel;
	CurrentLevelPanel* currentLevelInfos;
	NextLevelPanel* nextLevelInfos;

	cocos2d::Layer* descriptionLayout;
	cocos2d::Layer* lockedLayout;

	cocos2d::ui::Button* nextLevelButton;
	cocos2d::Label* maxLevelLabel;

	
};