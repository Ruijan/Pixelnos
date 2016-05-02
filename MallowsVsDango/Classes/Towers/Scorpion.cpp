#include "Scorpion.h"
#include "../Towers/Attack.h"
#include "../AppDelegate.h"

USING_NS_CC;

Scorpion::Scorpion() : Tower(), nb_limit_attack(0){
}

Scorpion* Scorpion::create()
{
	Scorpion* pSprite = new Scorpion();

	if (pSprite->initWithFile(Scorpion::getConfig()["image"].asString()))
	{
		pSprite->initFromConfig();
		pSprite->initDebug();
		pSprite->initEnragePanel();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

Json::Value Scorpion::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues()["towers"]["archer"];
}

const Json::Value Scorpion::getSpecConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues()["towers"]["archer"];
}

void Scorpion::attack(){
	if (target != nullptr) {
		Size visibleSize = Director::getInstance()->getVisibleSize();
		WaterBall* ball = nullptr;
		if (level >= (int)getConfig()["cost"].size() - 1) {
			ball = WaterBombBall::create(target, damage, 500 * visibleSize.width / 960, 100);
		}
		else {
			ball = WaterBall::create(target, damage, 500 * visibleSize.width / 960);
		}
		ball->setDamagesId(attacked_enemies[target]);
		attacked_enemies.erase(attacked_enemies.find(target));
		target->addTargetingAttack(ball);
		ball->setPosition(getPosition() - Vec2(0, getSpriteFrame()->getRect().size.width / 2 * getScale()));
		ball->setScale(visibleSize.width / 960);
		SceneManager::getInstance()->getGame()->getLevel()->addAttack(ball);
		++nb_attacks;
	}
}

void Scorpion::startLimit() {
	chooseTarget(SceneManager::getInstance()->getGame()->getLevel()->getEnemies());

	if (isLimitReached() && nb_limit_attack < 2 && target != nullptr) {
		if (state == ATTACKING) {
			target->removePDamages(attacked_enemies[target]);
			attacked_enemies.empty();
		}
		state = LIMIT_BURSTING;
		//((Label*)getChildByName("label_state"))->setString("LIMIT_BURSTING");
		cocos2d::Vector<SpriteFrame*> animFrames = getAnimation(ATTACKING);
		Animation* currentAnimation = Animation::createWithSpriteFrames(animFrames,
			animation_duration / nb_frames_anim / 3.f);

		auto callbackAttack = CallFunc::create([&]() {
			givePDamages(damage);
			attack();
			startLimit();
		});

		// create a sequence with the actions and callbacks
		auto seq = Sequence::create(Animate::create(currentAnimation), callbackAttack, nullptr);
		++nb_limit_attack;
		runAction(seq);
	}
	else if(isLimitReached()) {
		nb_attacks = 0;
		nb_limit_attack = 0;
		timer = 0;
		timerIDLE = 0;
		state = RELOADING;
		std::string frameName = name + "_attack_movement_000.png";
		SpriteFrameCache* cache = SpriteFrameCache::getInstance();
		setSpriteFrame(cache->getSpriteFrameByName(frameName.c_str()));
	}
}
