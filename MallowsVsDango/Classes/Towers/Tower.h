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
		RELOADING = 2
	};

	enum TowerType{
		ARCHER,
		GRILL
	};

	Tower(double nspeed, double ndamage, double nrange, double ncost);
	virtual ~Tower();
	//static Tower* create();

	virtual void chooseTarget(std::vector<Dango*> targets);

	virtual void attack();

	bool isFixed();
	void setFixed(bool f);
	bool isSelected();
	void setSelected(bool select);
	bool hasToBeDestroyed();
	double getRange();
	double getCost();
	Tower::State getState();
	void setState(Tower::State state);
	virtual void update(float dt);

	void displayRange(bool disp);
	
	void destroyCallback(Ref* sender);
	void builtCallback(Ref* sender);
	void upgradeCallback(Ref* sender);

	static Json::Value getConfig();
	virtual Json::Value getSpecConfig() = 0;

	void initDebug();

	static TowerType getTowerTypeFromString(std::string type);

protected:
	void startAnimation();
	//void updateDisplay(double dt);
	
	//attribute
	State state;
	double cost;
	bool fixed;
	bool selected;
	bool destroy;
	cocos2d::Menu* menu;
	Dango* target;
	double attackSpeed;
	bool attacking;
	double damage;
	double range;
	double timer;
	cocos2d::Action* currentAction;

	//methods
	
};


#endif
