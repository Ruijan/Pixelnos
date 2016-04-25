#include "Scorpion.h"
#include "../Towers/Attack.h"
#include "../AppDelegate.h"

USING_NS_CC;

Scorpion::Scorpion() : Tower(Scorpion::getConfig()["attack_speed"][0].asDouble(), Scorpion::getConfig()["damages"][0].asDouble(),
	Scorpion::getConfig()["range"][0].asDouble(), Scorpion::getConfig()["cost"][0].asDouble()), nb_limit_attack(0){
}

Scorpion* Scorpion::create()
{
	Scorpion* pSprite = new Scorpion();

	if (pSprite->initWithFile(Scorpion::getConfig()["image"].asString()))
	{
		pSprite->initDebug();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

Json::Value Scorpion::getConfig(){
	return Tower::getConfig()["archer"];
}

Json::Value Scorpion::getSpecConfig(){
	return Tower::getConfig()["archer"];
}

void Scorpion::attack(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	WaterBall* ball = nullptr;
	if (level >= (int)getConfig()["cost"].size() - 1) {
		ball = WaterBombBall::create(target, damage, 300 * visibleSize.width / 960, 100);
	}
	else {
		ball = WaterBall::create(target, damage, 300 * visibleSize.width / 960);
	}
	target->addTargetingAttack(ball);
	ball->setPosition(getPosition() - Vec2(0, getSpriteFrame()->getRect().size.width / 2 * getScale()));
	ball->setScale(visibleSize.width / 960);
	SceneManager::getInstance()->getGame()->getLevel()->addAttack(ball);
	++nb_attacks;
}

void Scorpion::startLimit() {
	if (isLimitReached() && nb_limit_attack < 2) {
		state = LIMIT_BURSTING;
		chooseTarget(SceneManager::getInstance()->getGame()->getLevel()->getEnemies());
		
		if (target != nullptr) {
			cocos2d::Vector<SpriteFrame*> animFrames = getAnimation(ATTACKING);
			Animation* currentAnimation = Animation::createWithSpriteFrames(animFrames,
				getSpecConfig()["animation_attack_time"].asDouble() / getSpecConfig()["animation_attack_size"].asDouble() / 5.f);

			auto callbackAttack = CallFunc::create([&]() {
				attack();
				startLimit();
			});

			// create a sequence with the actions and callbacks
			auto seq = Sequence::create(Animate::create(currentAnimation), callbackAttack, nullptr);
			++nb_limit_attack;
			runAction(seq);
		}
	}
	else if(isLimitReached()) {
		nb_attacks = 0;
		nb_limit_attack = 0;
		timer = 0;
		timerIDLE = 0;
		state = RELOADING;
	}
}
