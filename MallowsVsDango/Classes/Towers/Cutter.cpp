#include "Cutter.h"
#include "../Dangos/Dango.h"
#include "../Towers/Attack.h"
#include "../Level/Cell.h"
#include "../AppDelegate.h"

USING_NS_CC;

Cutter::Cutter() : Tower(Cutter::getConfig()["attack_speed"][0].asDouble(), Cutter::getConfig()["damages"][0].asDouble(),
	Cutter::getConfig()["range"][0].asDouble(), Cutter::getConfig()["cost"][0].asDouble()){

}

Cutter* Cutter::create()
{
	Cutter* pSprite = new Cutter();

	if (pSprite->initWithFile(Cutter::getConfig()["image"].asString()))
	{
		pSprite->initDebug();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

Json::Value Cutter::getConfig(){
	return Tower::getConfig()["cutter"];
}

Json::Value Cutter::getSpecConfig(){
	return Tower::getConfig()["cutter"];
}

void Cutter::removeTarget(Dango* dango) {
	Tower::removeTarget(dango);
	for (auto& cTarget : otherTargets) {
		if (cTarget == dango) {
			cTarget = nullptr;
		}
	}
	otherTargets.erase(std::remove(otherTargets.begin(), otherTargets.end(), nullptr), otherTargets.end());
}

void Cutter::chooseTarget(std::vector<Dango*> targets){
	if(state != State::ATTACKING && state != State::RELOADING){
		for (auto& c_target : otherTargets) {
			c_target->removeTargetingTower(this);
		}
		otherTargets.clear();
		for(auto& cTarget : targets){
			if(cTarget != nullptr){
				int first = cTarget->getTargetedCell();
				double dist = cTarget->getPosition().distanceSquared(this->getPosition());
				double minDist = pow(getRange() , 2);
				if (dist <= minDist && cTarget->willBeAlive()){
					cTarget->addTargetingTower(this);
					otherTargets.push_back(cTarget);
				}
			}
		}
		if(otherTargets.size() > 0){
			target = otherTargets[0];
		}
		else{
			target = nullptr;
		}
	}
	else if(state == State::RELOADING){
		for (auto& c_target : otherTargets) {
			c_target->removeTargetingTower(this);
		}
		otherTargets.clear();
		target = nullptr;
	}
}

void Cutter::givePDamages(double damage){
	for (auto& cTarget : otherTargets){
		cTarget->takePDamages(damage);
	}
}

void Cutter::attack(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	for (auto& cTarget : otherTargets){
		if (cTarget != nullptr){
			Slash* slash = nullptr;
			if (level >= (int)getConfig()["cost"].size() - 1 || state == LIMIT_BURSTING) {
				slash = DeepSlash::create(cTarget, damage, getConfig()["deep_wound_duration"].asDouble(),
					getConfig()["deep_wound_percent"].asDouble());
			}
			else {
				slash = Slash::create(cTarget, damage);
			}
			target->addTargetingAttack(slash);
			slash->setPosition(cTarget->getPosition());
			slash->setScale(visibleSize.width/960);
			slash->setVisible(false);
			SceneManager::getInstance()->getGame()->getLevel()->addAttack(slash);
			++nb_attacks;
		}
	}
}

void Cutter::startLimit() {
	if (isLimitReached()) {
		chooseTarget(SceneManager::getInstance()->getGame()->getLevel()->getEnemies());
		if (target != nullptr) {
			state = LIMIT_BURSTING;
			cocos2d::Vector<SpriteFrame*> animFrames = getAnimation(ATTACKING);
			Animation* currentAnimation = Animation::createWithSpriteFrames(animFrames,
				getSpecConfig()["animation_attack_time"].asDouble() / getSpecConfig()["animation_attack_size"].asDouble() / 3.0f);

			auto callbackAttack = CallFunc::create([&]() {
				attack();
				nb_attacks = 0;
				timer = 0;
				timerIDLE = 0;
				state = RELOADING;
			});

			// create a sequence with the actions and callbacks
			auto seq = Sequence::create(Animate::create(currentAnimation), callbackAttack, nullptr);
			runAction(seq);
		}
	}
}