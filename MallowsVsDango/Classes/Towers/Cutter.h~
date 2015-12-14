#ifndef GRILL_HPP
#define GRILL_HPP

#include "cocos2d.h"
#include "Tower.h"

class Grill : public Tower{
protected:
	std::vector<Dango*> otherTargets;

	

public:
	Grill();
	static Grill* create();

	static Json::Value getConfig();
	virtual Json::Value getSpecConfig();

	virtual void chooseTarget(std::vector<Dango*> targets);
	virtual void attack();
};

#endif