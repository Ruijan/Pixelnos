#ifndef BOMBER_HPP
#define BOMBER_HPP

#include "cocos2d.h"
#include "Tower.h"

class Level;

class Bomber : public Tower{

public:
	Bomber();
	static Bomber* create(Config* configClass, Level* globalLevel);
	static const Json::Value& getConfig();
	virtual const Json::Value& getSpecConfig();
	
	virtual void attack();
	virtual void startLimit();
	virtual void handleEndEnrageAnimation();

protected:
	virtual TowerType getType();

private:
	int nb_limit_attack;
};


#endif
