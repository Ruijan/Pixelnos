#pragma once

#include "cocos2d.h"
#include <unordered_map>
#include "../../Towers/Tower.h"
#include "ui/CocosGUI.h"
#include "../Dialogue.h"
#include "../ChallengeHandler.h"
#include "StartMenu.h"
#include "WinMenu.h"
#include "LoseMenu.h"
#include "RightPanel.h"
#include "LevelInfo.h"
#include "../../Dangos/Dango.h"


class MyGame;
class Config;

class LevelInterface : public cocos2d::Layer
{	
public:
	enum State{
		IDLE=0,
		TOUCHED=1,
		TURRET_CHOSEN=2,
		TURRET_SELECTED=3,
		DANGO_SELECTED=4,
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
	
	LevelInterface();
	~LevelInterface();
	virtual bool init(MyGame* ngame);
	void addRewardLayout();
	void addBlackMask(cocos2d::Size &visibleSize);
	void addEvents();
	static LevelInterface* create(MyGame* ngame);
	
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

	void setGameState(GameState g_state);
	void setSelectedTower(Tower* tower);
	void generateHolySugar(cocos2d::Vec2 pos);
	void startRewarding(cocos2d::Vec2 pos);
	void removeTower();
	void handleDeadDango();

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
	State state;
	GameState game_state;
	Tower* selected_turret;
	Dango* selected_dango;
	StartMenu* startMenu;
	WinMenu* winMenu;
	LoseMenu* loseMenu;
	RightPanel* rightPanel;
	LevelInfo* levelInfo;

	const double sizeButton;
	Dialogue* dialogues;
	ChallengeHandler* challenges;

	Config* configClass;
	
protected:
	void moveSelectedTurret(cocos2d::Vec2 pos);
	bool isOnTower(cocos2d::Vec2 pos);
	
	void initParametersMenu(const Json::Value& config);
	void initLoseMenu(const std::string& language, const Json::Value& buttons, const Json::Value& advice);
	void initWinMenu(const Json::Value& config);
	void initRightPanel(const std::string& language, const Json::Value& buttons);
	void initLabels(const Json::Value& config);
	void initStartMenu(const Json::Value& config);
	void initDialoguesFromLevel(const Json::Value& config);

	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);


	void builtCallback(Ref* sender);
	std::pair<std::string, cocos2d::Sprite*> getTowerFromPoint(cocos2d::Vec2 location);
	void endGame();
};