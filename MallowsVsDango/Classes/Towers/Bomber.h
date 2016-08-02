#ifndef BOMBER_HPP
#define BOMBER_HPP

#include "cocos2d.h"
#include "Tower.h"

class Bomber : public Tower{

public:
	Bomber();
	static Bomber* create();
	static Json::Value getConfig();
	virtual const Json::Value getSpecConfig();
	virtual bool isSameType(std::string type);
	
	virtual void attack();
	virtual void startLimit();
	virtual void handleEndEnrageAnimation();

private:
	int nb_limit_attack;

};


#endif
