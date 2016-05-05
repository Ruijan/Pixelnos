#ifndef SAUCER_HPP
#define SAUCER_HPP

#include "cocos2d.h"
#include "Tower.h"

class Saucer : public Tower{

public:
	Saucer();
	static Saucer* create();
	static Json::Value getConfig();
	virtual const Json::Value getSpecConfig();
	
	virtual void attack();
	virtual void startLimit();

private:
	int nb_limit_attack;

};


#endif
