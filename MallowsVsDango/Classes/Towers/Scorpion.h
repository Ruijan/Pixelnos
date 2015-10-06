#ifndef ARCHER_HPP
#define ARCHER_HPP

#include "cocos2d.h"
#include "Tower.h"

class Scorpion : public Tower{

public:
	Scorpion();
	static Scorpion* create();
	static Json::Value getConfig();
	virtual Json::Value getSpecConfig();

};


#endif
