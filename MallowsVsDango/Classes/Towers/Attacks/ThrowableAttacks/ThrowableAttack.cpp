#include "ThrowableAttack.h"
#include "../../../Dangos/Dangobese.h"

ThrowableAttack::ThrowableAttack(Dango* ntarget, double ndamages, double speed, const std::string& njsontype) : 
	Attack(ntarget, ndamages, njsontype), previousPosition(cocos2d::Vec2(0, 0)), 
	previousTargetPosistion(cocos2d::Vec2(0, 0)), speed(speed) {
}

void ThrowableAttack::updateIDLE(float dt)
{
	cocos2d::Vec2 direction = target->getPosition() - getPosition();
	if (hasReachedTarget(direction)) {
		state = REACHED_TARGET;
	}
	else {
		moveAndRotate(direction, dt);
	}
}

bool ThrowableAttack::hasReachedTarget(cocos2d::Vec2 &direction)
{
	cocos2d::Vec2 previousDirection = previousTargetPosistion - previousPosition;
	double distance = sqrt(direction.x*direction.x + direction.y*direction.y);
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	bool hasReached = distance < 10 * visibleSize.width / 960;
	if (initialized(previousDirection) && 
		(wentToFarHorizontally(previousDirection, direction) || 
			wentToFarVertically(previousDirection, direction))) {
			hasReached = true;
	}
	return hasReached;
}

bool ThrowableAttack::wentToFarHorizontally(cocos2d::Vec2 &previousDirection, cocos2d::Vec2 & direction)
{
	return (previousDirection.x < 0 && direction.x > 0) || (previousDirection.x > 0 && direction.x < 0);
}

bool ThrowableAttack::wentToFarVertically(cocos2d::Vec2 &previousDirection, cocos2d::Vec2 & direction)
{
	return (previousDirection.y < 0 && direction.y > 0) || (previousDirection.y > 0 && direction.y < 0);
}

bool ThrowableAttack::initialized(cocos2d::Vec2 &previousDirection)
{
	return previousDirection != cocos2d::Vec2(0, 0) && previousTargetPosistion != cocos2d::Vec2(0, 0);
}

void ThrowableAttack::moveAndRotate(cocos2d::Vec2 &direction, float dt)
{
	direction.normalize();
	previousPosition = getPosition();
	previousTargetPosistion = target->getPosition();
	setPosition(getPosition() + direction * speed * dt);
	setRotation(getRotation() + 360 * dt);
}