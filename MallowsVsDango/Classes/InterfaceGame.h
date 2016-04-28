#ifndef INTERFACEGAME_HPP
#define INTERFACEGAME_HPP

#include "cocos2d.h"
#include <unordered_map>
#include "Towers/Tower.h"
#include "ui/CocosGUI.h"
#include "Dialogue.h"

class MyGame;

class InterfaceGame : public cocos2d::Layer
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
	
	InterfaceGame();
	~InterfaceGame();
	virtual bool init();
	void addEvents();
	static InterfaceGame* create(MyGame* ngame, int id_level);
	void initDialoguesFromLevel();
	
	void setGame(MyGame* ngame);
	void setLevel(int id_level);
	void menuTurretTouchCallback(Tower::TowerType turret);
	void accelerateOnOffCallback(Ref* sender);
	void setListening(bool listening);
	
	void showWin();
	void showLose();
	
	void update(float dt);
	void updateButtonDisplay();
	void updateObjectDisplay(float dt);

	void reset();
	GameState getGameState() const;
	Dango* getCurrentDango();
	void setGameState(GameState g_state);
	void startRewarding(cocos2d::Vec2 pos);
	void removeTower();
	void handleDeadDango();

	void showDangoInfo();
	void hideDangoInfo();
	void showTowerInfo();
	void hideTowerInfo();

private:
	std::unordered_map<std::string, cocos2d::Sprite*> towers_menu;
	cocos2d::EventListenerTouchOneByOne* listener;

	MyGame* game;
	int id;
	State state;
	GameState game_state;
	Tower* selected_turret;
	Dango* selected_dango;

	const double sizeButton;
	const double sizeTower;
	Dialogue* dialogues;
	
protected:
	void moveSelectedTurret(cocos2d::Vec2 pos);
	bool isOnTower(cocos2d::Vec2 pos);
	void mainMenuCallBack(std::string id_menu);
	
	void initParametersMenu();
	void initLoseMenu();
	void initWinMenu();
	void initRightPanel();
	void initLabels();
	void initStartMenu();

	void displayTowerInfos(std::string itemName);
	void resetTowerMenu();

	void destroyCallback(Ref* sender);
	void builtCallback(Ref* sender);
	std::pair<std::string, cocos2d::Sprite*> getTowerFromPoint(cocos2d::Vec2 location);
	void displayTowerMenu(cocos2d::Sprite*);
};

#endif
