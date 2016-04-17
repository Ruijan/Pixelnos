#include "Scorpion.h"
#include "../Towers/Bullet.h"
#include "../AppDelegate.h"

USING_NS_CC;

Scorpion::Scorpion() : Tower(Scorpion::getConfig()["attack_speed"][0].asDouble(), Scorpion::getConfig()["damages"][0].asDouble(),
	Scorpion::getConfig()["range"][0].asDouble(), Scorpion::getConfig()["cost"][0].asDouble(),
	Scorpion::getConfig()["d_damage"].asDouble(),Scorpion::getConfig()["d_range"].asDouble(),
	Scorpion::getConfig()["d_speed"].asDouble()){
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
	if (level >= getConfig()["cost"].size() - 1) {
		ball = WaterBombBall::create(target, damage, 300 * visibleSize.width / 960, 100);
	}
	else {
		ball = WaterBall::create(target, damage, 300 * visibleSize.width / 960);
	}
	ball->setPosition(getPosition() - Vec2(0, getSpriteFrame()->getRect().size.width / 2 * getScale()));
	ball->setScale(visibleSize.width / 960);
	SceneManager::getInstance()->getGame()->getLevel()->addAttack(ball);
}
