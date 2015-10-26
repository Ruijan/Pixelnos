#ifndef TOWER_HPP
#define TOWER_HPP

#include "cocos2d.h"
#include "../Config/json.h"


class Cell;
class Dango;

class Tower : public cocos2d::Sprite{

public:
	enum State{
		IDLE = 0,
		AWARE,
		ATTACKING,
		RELOADING,
		BLINKING_UP,
		BLINKING_DOWN 
	};

	enum TowerType{
		ARCHER,
		CUTTER
	};

	Tower(double nspeed, double ndamage, double nrange, double ncost,
		double nd_damage, double nd_range, double nd_speed);
	virtual ~Tower();
	void initDebug();

	// Setters & Getters
	bool isFixed();
	void setFixed(bool f);
	bool isSelected();
	void setSelected(bool select);
	bool hasToBeDestroyed();
	double getRange();
	double getNextLevelRange();
	double getCost();
	Dango* getTarget();
	double getDamage();
	double getNextLevelDamage();
	double getAttackSpeed();
	double getNextLevelSpeed();
	int getLevel();
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
	double d_speed;
	double damage;
	double d_damage;
	double range;
	double d_range;
	double timer;
	double timerIDLE;
	int level;
	
	//
	cocos2d::DrawNode* loadingCircle;
	cocos2d::Action* currentAction;
	cocos2d::Animation currentAnimation;
	
	//methods
	virtual void attack() = 0;
	void startAnimation();
	
};


#endif
