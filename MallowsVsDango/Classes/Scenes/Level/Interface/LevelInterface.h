#pragma once

#include "cocos2d.h"
#include <unordered_map>
#include "../Towers/Tower.h"
#include "ui/CocosGUI.h"
#include "../Dialogue.h"
#include "../ChallengeHandler.h"
#include "StartMenu.h"
#include "WinMenu.h"
#include "LoseMenu.h"
#include "RightPanel.h"
#include "LevelInfo.h"
#include "../Towers/GUI/TowerInformationPanel.h"
#include "../Dangos/GUI/DangoInformationPanel.h"
#include "../Dangos/Dango.h"

class MyGame;
class Config;
class ParametersMenu;
class Monkey;
class Level;

class LevelInterface : public cocos2d::Layer
{
public:
	enum State {
		IDLE = 0,
		TOUCHED = 1,
		TURRET_CHOSEN = 2,
		TURRET_SELECTED = 3,
		DANGO_SELECTED = 4,
	};
	enum GameState {
		INTRO,
		TITLE,
		STARTING,
		RUNNING,
		ENDING,
		DONE,
		NEXT_LEVEL
	};

	LevelInterface(MyGame* ngame, Config* config);
	~LevelInterface();
	virtual bool init();
	void addRewardLayout();
	void addBlackMask();
	void addEvents();
	static LevelInterface* create(MyGame* ngame, Config* config);

	void menuTurretTouchCallback(Tower::TowerType turret);
	void setListening(bool listening);

	void showWin();
	void showLose();

	void update(float dt);

	void pauseLevel();
	void resumeLevel();

	void reset();
	void resetSugarLabel();
	void resetTowerMenu();

	GameState getGameState() const;
	cocos2d::Vec2 getAbsoluteMenuTowerPosition(std::string towerName);
	cocos2d::Sprite* getTowerFromMenu(std::string towerName);
	Dango* getCurrentDango();
	int getSugarQuantity();
	int getLifeQuantity();
	ParametersMenu* getPauseMenu() { return pauseMenu; };
	GUISettings* getGUISettings();

	void setGameState(GameState g_state);
	void setSelectedTower(Tower* tower);
	void generateHolySugar(cocos2d::Vec2 pos);
	void startRewarding(cocos2d::Vec2 pos);
	void removeTower();
	void handleDeadDango();
	void addMonkey(Monkey * monkey);
	unsigned int getNbOfMonkeys();
	void makeAllMonkeysGoAway();

	void showDangoInfo();
	void hideDangoInfo();
	void showTowerInfo();
	void hideTowerInfo();
	void hideStartMenu();
	void lockStartMenu();
	void unlockStartMenu();

	void showLabelInformation();

	static void shakeScaleElement(Node* element, bool loop = true);

	void displayStartMenuIfInTitleState();

private:
	cocos2d::EventListenerTouchOneByOne* listener;

	MyGame* game;
	Level* level;
	State state;
	GameState game_state;
	Tower* selectedTower;
	Dango* selectedDango;
	StartMenu* startMenu;
	WinMenu* winMenu;
	LoseMenu* loseMenu;
	RightPanel* rightPanel;
	LevelInfo* levelInfo;
	ParametersMenu* pauseMenu;
	TowerInformationPanel* towerPanel;
	std::vector<Monkey*> monkeys;

	const double sizeButton;
	Dialogue* dialogues;
	ChallengeHandler* challenges;

	Config* configClass;
	GUISettings* settings;

protected:
	void moveSelectedTurret(cocos2d::Vec2 pos);
	bool isOnTower(cocos2d::Vec2 pos);

	void initParametersMenu();
	void initLoseMenu(const Json::Value& advice);
	void initWinMenu();
	void initRightPanel();
	void initLabels();
	void initStartMenu();
	void initDialoguesFromLevel();

	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void unselectDango();
	void unselectTower();
	void handleRightPanelTouch(cocos2d::Touch * touch);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	void handleEndTouchBuildingTower(cocos2d::Rect &rectrightpanel, cocos2d::Vec2 &p);
	void handleEndTouchForDango(cocos2d::Vec2 &p);
	void handleEndTouchForTower(cocos2d::Vec2 &p);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);

	void builtCallback(Ref* sender);
	void endGame();
};