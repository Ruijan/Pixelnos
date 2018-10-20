#ifndef ATTACK_HPP
#define ATTACK_HPP

#include "cocos2d.h"
#include "Effect.h"

class Dango;
class Dangobese;

class Attack : public cocos2d::Sprite{
public:
	enum State {
		IDLE,
		REACHED_TARGET,
		ANIMATING,
		DONE
	};
	Attack(Dango* ntarget, double ndamages, std::string njsontype);
	virtual ~Attack();
	
	virtual void update(float dt);
	virtual void updateIDLE(float dt);
	void applyDamagesToTarget();
	void startAnimation();

	void removeTarget(Dango* dango);
	void setTarget(Dango* dango);
	bool isDone();
	int getRewardedXP();
	std::string getType();
	void setEnabled(bool enable);
	void setHasToBeDeleted();
	void setDamagesId(int id);

	virtual bool affectEnemy(Dangobese* enemy) = 0;

protected:
	Dango* target;
	int id;
	double damages;
	int damages_id;
	State state;

	// deletion attributes
	bool enabled;
	cocos2d::Action* action;

	std::string jsontype;
	int rewardedXP;
};



#endif
