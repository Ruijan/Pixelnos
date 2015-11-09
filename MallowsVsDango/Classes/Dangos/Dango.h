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
		HIT,
		IDLE
	};


public:
	Dango(std::vector<Cell*> npath, double nspeed, double hp, int level);
	virtual ~Dango();
	//static Dango* create(std::string image, std::vector<Cell*> npath, int level);

	static Json::Value getConfig();
	virtual Json::Value getSpecConfig() = 0;

	bool isMoving();
	bool isAlive();
	bool willBeAlive();
	void update(float dt);
	double getHitPoints();
	double getGain();
	void takeDamages(double damages);
	void takePDamages(double damages);
	bool isDone();
	void move(float dt);
	void updateAnimation();
	int getTargetedCell();
	double getSpeed();
	


private:
	int targetedCell;
	std::vector<Cell*> path;
	double speed;
	double hitPoints;
	DIRECTION cDirection;
	cocos2d::Action* cAction;
	double pDamages;				// prosepctive damages
	STATE state;
	double timer;
	int level;
	
};
#endif
