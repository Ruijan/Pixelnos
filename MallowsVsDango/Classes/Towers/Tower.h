#ifndef TOWER_HPP
#define TOWER_HPP

#include "cocos2d.h"
#include "../Config/json.h"


class Cell;
class Dango;
class Scorpion;
class Grill;

class Tower : public cocos2d::Sprite{

public:
	enum State{
		IDLE = 0,
		ATTACKING = 1,
		RELOADING = 2,
		BLINKING_UP = 3,
		BLINKING_DOWN = 4
	};

	enum TowerType{
		ARCHER,
		GRILL
	};

	Tower(double nspeed, double ndamage, double nrange, double ncost);
	virtual ~Tower();
	void initDebug();

	// Setters & Getters
	bool isFixed();
	void setFixed(bool f);
	bool isSelected();
	void setSelected(bool select);
	bool hasToBeDestroyed();
	double getRange();
	double getCost();
	Dango* getTarget();
	double getDamage();
	int getLevel();
	double getAttackSpeed();
	Tower::State getState();
	void setState(Tower::State state);
	void setTarget(Dango* dango);
	void displayRange(bool disp);
	static TowerType getTowerTypeFromString(std::string type);
	cocos2d::Vector<cocos2d::SpriteFrame*> getAnimation(Tower::State animState);
	static cocos2d::Vector<cocos2d::SpriteFrame*> getAnimationFromName(std::string name, Tower::State animState);
	
	// Updates
	virtual void update(float dt);
	virtual void updateDisplay(float dt);
	virtual void chooseTarget(std::vector<Dango*> targets);
	virtual void reload();
	
	
	// Callbacks
	void destroyCallback(Ref* sender);
	void builtCallback(Ref* sender);
	void upgradeCallback(Ref* sender);

	// Config
	static Json::Value getConfig();
	virtual Json::Value getSpecConfig() = 0;
	static const int MAXLEVEL = 5;

protected:
	
	//void updateDisplay(double dt);
	
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
	int level;
	
	//
	cocos2d::DrawNode* loadingCircle;
	cocos2d::Action* currentAction;
	
	//methods
	void shoot();
	void startAnimation();
	
};


#endif
