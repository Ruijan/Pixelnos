#ifndef INTERFACEGAME_HPP
#define INTERFACEGAME_HPP

#include "cocos2d.h"
#include <unordered_map>
#include "../Towers/Tower.h"
#include "ui/CocosGUI.h"
#include "Dialogue.h"
#include "ChallengeHandler.h"

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
	static InterfaceGame* create(MyGame* ngame);
	
	void setGame(MyGame* ngame);
	void menuTurretTouchCallback(Tower::TowerType turret);
	void accelerateOnOffCallback(Ref* sender);
	void setListening(bool listening);
	
	void showWin();
	void showLose();
	
	void update(float dt);
	void updateTutorial(float dt);
	void updateButtonDisplay();
	void updateObjectDisplay(float dt);
	void updateIncrementXP(cocos2d::Label* exp_label, cocos2d::ui::LoadingBar* loading_bar, 
		std::string tower_name, float* increment, int initial_value, int diff_exp, int loop, int max_level);

	void reset();
	GameState getGameState() const;
	Dango* getCurrentDango();
	void setGameState(GameState g_state);
	void generateHolySugar(cocos2d::Vec2 pos);
	void startRewarding(cocos2d::Vec2 pos);
	void removeTower();
	void handleDeadDango();

	void showDangoInfo();
	void hideDangoInfo();
	void showTowerInfo();
	void hideTowerInfo();

	static void shakeElement(cocos2d::Node* element, bool loop = true);
	static void shakeScaleElement(Node* element, bool loop = true);

private:
	std::unordered_map<std::string, std::pair<cocos2d::Sprite*, double>> towers_menu;
	cocos2d::EventListenerTouchOneByOne* listener;

	MyGame* game;
	State state;
	GameState game_state;
	Tower* selected_turret;
	Dango* selected_dango;

	const double sizeButton;
	const double sizeTower;
	Dialogue* dialogues;
	ChallengeHandler* challenges;
	bool tutorial_running;
	
protected:
	void moveSelectedTurret(cocos2d::Vec2 pos);
	bool isOnTower(cocos2d::Vec2 pos);
	void mainMenuCallBack(std::string id_menu);
	
	void initParametersMenu(const Json::Value& config);
	void initLoseMenu(const Json::Value& config);
	void initWinMenu(const Json::Value& config);
	void initRightPanel(const Json::Value& config);
	void initLabels(const Json::Value& config);
	void initStartMenu(const Json::Value& config);
	void initDialoguesFromLevel(const Json::Value& config);

	void displayTowerInfos(std::string itemName);
	void createTowersLayout();

	void destroyCallback(Ref* sender);
	void builtCallback(Ref* sender);
	std::pair<std::string, cocos2d::Sprite*> getTowerFromPoint(cocos2d::Vec2 location);
	void displayTowerMenu(cocos2d::Sprite*);
	void endGame();
};

#endif
