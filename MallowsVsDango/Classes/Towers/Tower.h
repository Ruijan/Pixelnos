#ifndef TOWER_HPP
#define TOWER_HPP

#include "cocos2d.h"
#include "../Config/json.h"
#include "ui/CocosGUI.h"

class Cell;
class Dango;
class InterfaceGame;

class Tower : public cocos2d::Sprite{

public:
	enum State{
		IDLE = 0,
		AWARE,
		ATTACKING,
		RELOADING,
		LIMIT_BURSTING,
		BLINKING_UP,
		BLINKING_DOWN 
	};
	enum TowerType{
		ARCHER,
		CUTTER
	};

	Tower(double nspeed, double ndamage, double nrange, double ncost);
	virtual ~Tower();
	void initDebug();

	// Setters & Getters
	bool isFixed();
	void setFixed(bool f);
	bool isSelected();
	void setSelected(bool select);
	bool isLimitReached();
	bool hasToBeDestroyed();
	double getRange();
	double getCost();
	Dango* getTarget();
	double getDamage();
	double getAttackSpeed();
	int getLevel();
	Tower::State getState();
	void setState(Tower::State state);
	void setTarget(Dango* dango);
	void displayRange(bool disp);
	cocos2d::ui::Layout* getInformationLayout(InterfaceGame* interface_game);
	static TowerType getTowerTypeFromString(std::string type);
	cocos2d::Vector<cocos2d::SpriteFrame*> getAnimation(Tower::State animState);
	static cocos2d::Vector<cocos2d::SpriteFrame*> getAnimationFromName(std::string name, Tower::State animState);
	virtual void removeTarget(Dango* dango);
	
	// Updates
	virtual void update(float dt);
	virtual void updateDisplay(float dt);
	virtual void updateInformationLayout(cocos2d::ui::Layout* layout);
	virtual void chooseTarget(std::vector<Dango*> targets);
	virtual void givePDamages(double damage);
	virtual void reload();
	
	// Callbacks
	void destroyCallback(Ref* sender);
	void builtCallback(Ref* sender);
	void upgradeCallback(Ref* sender);
	virtual void startLimit() = 0;

	// Config
	static Json::Value getConfig();
	virtual Json::Value getSpecConfig() = 0;

protected:
	//State attribute
	State state;
	bool fixed;
	bool selected;
	bool destroy;
	
	Dango* target;
	
	// Characteristics
	double cost;
	double attackSpeed;
	double damage;
	double range;
	double timer;
	double timerIDLE;
	int level;

	int nb_attacks;
	
	//
	cocos2d::DrawNode* loadingCircle;
	cocos2d::Action* currentAction;
	cocos2d::Animation currentAnimation;
	
	//methods
	virtual void attack() = 0;
	void startAnimation(float speed = 1);
};


#endif
