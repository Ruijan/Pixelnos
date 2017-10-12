#ifndef ATTACK_HPP
#define ATTACK_HPP

#include "cocos2d.h"
#include "Effect.h"

class Dango;
class Dangobese;

class Attack : public cocos2d::Sprite{
public:
	Attack(Dango* ntarget, double ndamages, std::string njsontype);
	virtual ~Attack();
	
	virtual void update(float dt) = 0;
	void startAnimation();

	void removeTarget(Dango* dango);
	bool hasTouched();
	bool isDone();
	Dango* getTarget();
	void setTarget(Dango* dango);
	std::string getType();
	void setEnabled(bool enable);
	void setHasToBeDeleted(bool deleted);
	void setDamagesId(int id);

	virtual bool affectEnemy(Dangobese* enemy) = 0;

protected:
	Dango* target;
	int id;
	double damages;
	int damages_id;
	cocos2d::Vec2 previous_pos;
	cocos2d::Vec2 previous_target_pos;

	// deletion attributes
	bool touched;
	bool hasToBeDeleted;
	bool enabled;
	cocos2d::Action* action;

	std::string jsontype;
};

class Suckable {
public:
	Suckable();
	bool isForceApplied();
	void setForceApplied(bool applied);
	void update();
protected:
	bool force_applied;
};

// WATERBALL
class WaterBall : public Attack, public Suckable {
public:
	WaterBall(Dango* ntarget, double ndamages, double nspeed);
	virtual ~WaterBall();
	static WaterBall* create(Dango* ntarget, double damages, double nspeed);
	static WaterBall* createWithFile(std::string file, Dango* ntarget, double damages, double nspeed);

	void update(float dt);
	virtual bool affectEnemy(Dangobese* enemy);
	
protected:
	double speed;
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

// CHOCOSPIT
class ChocoSpit : public Attack, public Suckable {
public:
	ChocoSpit(Dango* ntarget, double damages, double slow, double duration, double nspeed);
	virtual ~ChocoSpit();
	static ChocoSpit* create(Dango* ntarget, double damages, double slow, double slow_duration, double nspeed);
	static ChocoSpit* createWithFile(std::string file, Dango* ntarget, double damages, double slow, double slow_duration, double nspeed);

	void update(float dt);
	virtual bool affectEnemy(Dangobese* enemy);
protected:
	double speed;
	float slow_percent;
	float slow_duration;
};

/*// ACIDCHOCOSPIT
class AcidChocoSpit : public ChocoSpit {
public:
	AcidChocoSpit(Dango* ntarget, double slow, double slow_duration, double nspeed);
	virtual ~AcidChocoSpit();
	static AcidChocoSpit* create(Dango* ntarget, double slow, double slow_duration, double nspeed);
	static AcidChocoSpit* createWithFile(std::string file, Dango* ntarget, double slow, double slow_duration, double nspeed);

	void update(float dt);
	virtual bool affectEnemy(Dangobese* enemy);
protected:
	Burn* effect;
};*/

// SLASH
class Slash : public Attack {
public:
	Slash(Dango* ntarget, double ndamages);
	virtual ~Slash();
	static Slash* create(Dango* ntarget, double damages);

	void update(float dt);
	virtual bool affectEnemy(Dangobese* enemy);
};

// DEEP SLASH
class DeepSlash : public Slash {
public:
	DeepSlash(Dango* ntarget, double ndamages, double duration, double percent);
	virtual ~DeepSlash();
	static DeepSlash* create(Dango* ntarget, double damages, double duration, double percent);

	void update(float dt);
	virtual bool affectEnemy(Dangobese* enemy);

private:
	DeepWound* effect;
};



#endif
