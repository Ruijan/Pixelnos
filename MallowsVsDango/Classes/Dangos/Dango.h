#ifndef DANGO_HPP
#define DANGO_HPP

#include "cocos2d.h"
#include "../Config/json.h"

class Cell;

class Dango : public cocos2d::Sprite{
	enum DIRECTION{
		RIGHT,
		UP,
		DOWN,
		LEFT
	};
	enum STATE{
		IDLE,
		ATTACK,
		RELOAD,
		MOVE
	};


public:
	Dango(std::vector<Cell*> npath, double nspeed, double hp, int level,
		double damages, double a_speed);
	virtual ~Dango();
	//static Dango* create(std::string image, std::vector<Cell*> npath, int level);

	static Json::Value getConfig();
	virtual Json::Value getSpecConfig() = 0;

	bool isAlive();
	bool isDone();
	bool willBeAlive();
	double getHitPoints();
	double getGain();
	int getTargetedCell();
	double getSpeed();

	void update(float dt);
	void updateAnimation();
	void updateDirection(cocos2d::Vec2 direction);

	void takeDamages(double damages);
	void takePDamages(double damages);
	
	void move(float dt);
	void attack(float dt);
	
private:
	STATE state;
	DIRECTION cDirection;

	unsigned int targetedCell;
	std::vector<Cell*> path;
	cocos2d::Action* cAction;

	double speed;
	double hitPoints;
	double pDamages;				// prosepctive damages
	double reload_timer;
	double attack_damages;
	double attack_reloading;
	int level;
	
};
#endif
