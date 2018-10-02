#ifndef SAUCER_HPP
#define SAUCER_HPP

#include "cocos2d.h"
#include "Tower.h"


class Saucer : public Tower{

public:
	Saucer();
	static Saucer* create(Config* configClass);
	static const Json::Value& getConfig();
	virtual const Json::Value& getSpecConfig();
	void initSpecial();
	
	virtual void attack();
	virtual void startLimit();
	virtual void handleEndEnrageAnimation();
	virtual bool isPotentialTarget(Dango * cTarget);

protected:
	virtual TowerType getType();

private:
	int nb_limit_attack;
	float slow_percent;
	float slow_duration;

};


#endif
