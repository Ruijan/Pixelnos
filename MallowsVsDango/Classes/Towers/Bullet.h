#ifndef BULLET_HPP
#define BULLET_HPP

#include "cocos2d.h"

class Dango;
class Dangobese;

class Attack : public cocos2d::Sprite{
public:
	Attack(Dango* ntarget, double ndamages, std::string njsontype);
	virtual ~Attack();
	
	virtual void update(float dt) = 0;
	void startAnimation();

	bool hasTouched();
	bool isDone();
	Dango* getTarget();
	void setTarget(Dango* dango);
	std::string getType();
	void setEnabled(bool enable);
	void setHasToBeDeleted(bool deleted);

	virtual bool affectEnemy(Dangobese* enemy) = 0;

protected:
	Dango* target;
	double damages;

	// deletion attributes
	bool touched;
	bool hasToBeDeleted;
	bool enabled;
	cocos2d::Action* action;

	std::string jsontype;
};

// WATERBALL
class WaterBall : public Attack {
public:
	WaterBall(Dango* ntarget, double ndamages, double nspeed);
	virtual ~WaterBall();
	static WaterBall* create(Dango* ntarget, double damages, double nspeed);

	void update(float dt);
	virtual bool affectEnemy(Dangobese* enemy);
	bool isForceApplied();
	void setForceApplied(bool applied);
protected:
	double speed;
	bool force_applied;
};

// WATERBOMBBALL
class WaterBombBall : public WaterBall {
public:
	WaterBombBall(Dango* ntarget, double ndamages, double nspeed, double nrange);
	virtual ~WaterBombBall();
	static WaterBombBall* create(Dango* ntarget, double damages, double nspeed, double range);

	void update(float dt);
	virtual bool affectEnemy(Dangobese* enemy);

protected:
	double range;
};

// SLASH
class Slash : public Attack {
public:
	Slash(Dango* ntarget, double ndamages);
	virtual ~Slash();
	static Slash* create(Dango* ntarget, double damages);

	void update(float dt);
	virtual bool affectEnemy(Dangobese* enemy);
};

#endif
