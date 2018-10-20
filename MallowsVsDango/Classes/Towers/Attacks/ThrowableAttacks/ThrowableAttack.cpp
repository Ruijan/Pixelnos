#include "ThrowableAttack.h"
#include "../../../SceneManager.h"

ThrowableAttack::ThrowableAttack(Dango* ntarget, double ndamages, double speed, std::string njsontype) : Attack(ntarget, ndamages, njsontype),
previousPosition(cocos2d::Vec2(0, 0)), previousTargetPosistion(cocos2d::Vec2(0, 0)), speed(speed) {
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
	if (previousDirection != cocos2d::Vec2(0, 0) && previousTargetPosistion != cocos2d::Vec2(0, 0)) {
		if ((previousDirection.x < 0 && direction.x > 0) || (previousDirection.x > 0 && direction.x < 0) ||
			(previousDirection.y < 0 && direction.y > 0) || (previousDirection.y > 0 && direction.y < 0)) {
			hasReached = true;
		}
	}
	return hasReached;
}

void ThrowableAttack::moveAndRotate(cocos2d::Vec2 &direction, float dt)
{
	direction.normalize();
	previousPosition = getPosition();
	previousTargetPosistion = target->getPosition();
	setPosition(getPosition() + direction * speed * dt);
	setRotation(getRotation() + 360 * dt);
}