#ifndef EFFECT_HPP
#define EFFECT_HPP

#include "cocos2d.h"

class Dango;

/*
List Effects
DEEPWOUND
BURN
SLOW
*/

// EFFECT
class Effect: public cocos2d::Sprite{
public:
	Effect( Dango* ntarget, float duration, std::string animation_name = "");
	virtual ~Effect();
	bool initWithFileName(std::string image);
	std::string getAnimationName();
	
	virtual void update(float dt);
	bool isDone();
	bool willBeDone(double delay);

protected:
	float duration;
	float timer;
	Dango* target;
	std::string name;
};

// DEEPWOUND EFFECT
class DeepWound: public Effect {
public:
	DeepWound(Dango* ntarget, float duration, float dmg_percent, std::string animation_name = "");
	virtual ~DeepWound();
	static DeepWound* create(Dango* ntarget, float duration, float dmg_percent, std::string animation_name = "");

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
	Burn(Dango* ntarget, float duration, float dmg, std::string animation_name = "");
	virtual ~Burn();
	static Burn* create(Dango* ntarget, float duration, float dmg_percent, std::string animation_name = "");

	virtual void update(float dt);
	float getDamage();

protected:
	float dmg;
	float timer_dmg;
};

// SLOW EFFECT
class Slow : public Effect {
public:
	Slow(Dango* ntarget, float duration, float nslow, std::string animation_name = "");
	virtual ~Slow();
	static Slow* create(Dango* ntarget, float duration, float slow_percent, std::string animation_name = "");

	virtual void update(float dt);
	float getSlow();
	void applyModifierToTarget();

protected:
	float slow_percent;
	int id;
};

#endif