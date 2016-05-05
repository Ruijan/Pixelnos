#ifndef EFFECT_HPP
#define EFFECT_HPP

#include "cocos2d.h"

class Dango;

// EFFECT
class Effect: public cocos2d::Sprite{
public:
	Effect( Dango* ntarget, float duration);
	virtual ~Effect();
	bool initWithFileName(std::string image);
	
	virtual void update(float dt);
	bool isDone();
	bool willBeDone(double delay);

protected:
	float duration;
	float timer;
	Dango* target;
};

// DEEPWOUND EFFECT
class DeepWound: public Effect {
public:
	DeepWound(Dango* ntarget, float duration, float dmg_percent);
	virtual ~DeepWound();
	static DeepWound* create(Dango* ntarget, float duration, float dmg_percent);

	virtual void update(float dt);
	float getDmgPercent();
	void applyModifierToTarget();

protected:
	float dmg_percent;
	int id;
};

// BURN EFFECT
class Burn : public Effect {
public:
	Burn(Dango* ntarget, float duration, float dmg);
	virtual ~Burn();
	static Burn* create(Dango* ntarget, float duration, float dmg_percent);

	virtual void update(float dt);
	float getDamage();

protected:
	float dmg;
	float timer_dmg;
};

#endif