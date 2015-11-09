#ifndef GRILL_HPP
#define GRILL_HPP

#include "cocos2d.h"
#include "Tower.h"

class Cutter : public Tower{
protected:
	std::vector<Dango*> otherTargets;

	

public:
	Cutter();
	static Cutter* create();

	static Json::Value getConfig();
	virtual Json::Value getSpecConfig();

	virtual void chooseTarget(std::vector<Dango*> targets);
	virtual void givePDamages(double damage);
	virtual void attack();
};

#endif
