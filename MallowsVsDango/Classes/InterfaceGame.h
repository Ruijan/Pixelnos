#ifndef INTERFACEGAME_HPP
#define INTERFACEGAME_HPP

#include "cocos2d.h"
#include <unordered_map>
#include "Towers/Tower.h"

class Game;

class InterfaceGame : public cocos2d::Layer
{
private:

	enum State{
		IDLE=0,
		TOUCHED=1,
		TURRET_CHOSEN=2,
		TURRET_SELECTED=3,
		SCROLL=4
	};
	
	std::unordered_map<std::string, std::pair<cocos2d::Sprite*, cocos2d::Sprite*>> turretsMenu;
	cocos2d::Menu* menuOpen;
	cocos2d::Node* menuLoose;
	cocos2d::Node* menuWin;
	cocos2d::Node* menuPause;
	std::unordered_map<std::string,cocos2d::Label*> infos;
	cocos2d::Node* informationPanel;
	cocos2d::Node* menuPanel;
	cocos2d::DrawNode* blackMask;

	Game* game;
	State state;
	Tower* selectedTurret;

	const double sizeButton;
	const double sizeTower;
	
	
protected:
	void moveSelectedTurret(cocos2d::Vec2 pos);
	void initParametersMenu();
	void initLooseMenu();
	void initWinMenu();
	void initRightPanel();
	void initLabels();
	void displayTowerInfos(std::string itemName);

	void destroyCallback(Ref* sender);
	void builtCallback(Ref* sender);
	void upgradeCallback(Ref* sender);
	std::pair<std::string, std::pair<cocos2d::Sprite*, cocos2d::Sprite*>> getTowerFromPoint(cocos2d::Vec2 location);
	
public:
	InterfaceGame();
	virtual bool init();
	void addEvents();
	static InterfaceGame* create(Game* ngame);
	
	void setGame(Game* ngame);
	void menuGameCallback(cocos2d::Ref* sender);
	void menuMainCallback(cocos2d::Ref* sender);
	void menuSaveCallback(cocos2d::Ref* sender);
	void setMenuVisibleCallback(cocos2d::Ref* sender, int origin);
	//void menuTurretTouchCallback(Ref* sender, unsigned int turret);
	void menuTurretTouchCallback(Tower::TowerType turret);
	void reloadCallback(cocos2d::Ref* sender);
	void accelerateOnOffCallback(Ref* sender);
	
	void touchEvent(cocos2d::Touch* touch, cocos2d::Vec2 point);

	void showLoose();
	void showWin();
	
	/*virtual bool touchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void touchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void touchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	*/
	void update();
	void updateButtonDisplay();
	void reset();
};

#endif
