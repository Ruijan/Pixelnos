#include "Scorpion.h"
#include "../Towers/Bullet.h"
#include "../AppDelegate.h"

USING_NS_CC;

Scorpion::Scorpion() : Tower(Scorpion::getConfig()["attack_speed"].asDouble(), Scorpion::getConfig()["damages"].asDouble(),
	Scorpion::getConfig()["range"].asDouble(), Scorpion::getConfig()["cost"].asDouble(),
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
	Bullet* bullet = Bullet::create("res/turret/bullet.png", target, damage,300*visibleSize.width/960,true);
	bullet->setOwner("archer");
	bullet->setPosition(getPosition() - Vec2(0, getSpriteFrame()->getRect().size.width / 2 * getScale()));
	bullet->setScale(visibleSize.width/960);
	SceneManager::getInstance()->getGame()->getLevel()->addBullet(bullet);
}
