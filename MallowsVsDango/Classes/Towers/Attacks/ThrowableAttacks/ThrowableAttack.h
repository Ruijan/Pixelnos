#pragma once

#include "../Attack.h"
#include "../Suckable.h"

class ThrowableAttack : public Attack, public Suckable {
public:
	ThrowableAttack(Dango * ntarget, double ndamages, double speed, std::string njsontype);
	virtual void updateIDLE(float dt);
	bool hasReachedTarget(cocos2d::Vec2 & direction);
	void moveAndRotate(cocos2d::Vec2 & direction, float dt);
protected:
	cocos2d::Vec2 previousPosition;
	cocos2d::Vec2 previousTargetPosistion;
	double speed;
};