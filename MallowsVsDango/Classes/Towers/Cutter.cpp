#include "Cutter.h"
#include "../Dangos/Dango.h"
#include "../Towers/Attack.h"
#include "../Level/Cell.h"
#include "../AppDelegate.h"

USING_NS_CC;

Cutter::Cutter() : Tower(){

}

Cutter* Cutter::create(Config* configClass)
{
	Cutter* pSprite = new Cutter();

	/*if (pSprite->initWithFile(Cutter::getConfig()["image"].asString()))
	{*/
		pSprite->initFromConfig(configClass);
		pSprite->initDebug();
		pSprite->initEnragePanel();
		return pSprite;
	//}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

const Json::Value& Cutter::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::TOWER)["cutter"];
}

const Json::Value& Cutter::getSpecConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::TOWER)["cutter"];
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
	if(state != State::ATTACKING /*&& state != State::RELOADING*/){
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
		if (cTarget != nullptr) {
			attacked_enemies[cTarget] = cTarget->takePDamages(damage, animation_duration);
		}
	}
}

void Cutter::attack(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	for (auto& cTarget : otherTargets){
		if (cTarget != nullptr){
			Slash* slash = nullptr;
			if (level >= getConfig()["cost"].size() - 1 || state == LIMIT_BURSTING) {
				slash = DeepSlash::create(cTarget, settings->getDamage(level), getConfig()["deep_wound_duration"].asDouble(),
					getConfig()["deep_wound_percent"].asDouble());
			}
			else {
				slash = Slash::create(cTarget, settings->getDamage(level));
			}
			slash->setDamagesId(attacked_enemies[cTarget]);
			attacked_enemies.erase(attacked_enemies.find(cTarget));
			cTarget->addTargetingAttack(slash);
			slash->setPosition(cTarget->getPosition());
			slash->setScale(visibleSize.width/960);
			slash->setVisible(false);
			SceneManager::getInstance()->getGame()->getLevel()->addAttack(slash);
			++nb_attacks;
		}
	}
}

void Cutter::startLimit() {
	chooseTarget(SceneManager::getInstance()->getGame()->getLevel()->getEnemies());

	if (isLimitReached() && target != nullptr) {
		if (state == ATTACKING) {
			for (auto& cTarget : otherTargets) {
				cTarget->removePDamages(attacked_enemies[cTarget]);
			}
			attacked_enemies.empty();
		}

		state = LIMIT_BURSTING;
		//((Label*)getChildByName("label_state"))->setString("LIMIT_BURSTING");
		/*cocos2d::Vector<SpriteFrame*> animFrames = getAnimation(ATTACKING);
		Animation* currentAnimation = Animation::createWithSpriteFrames(animFrames,
			animation_duration / nb_frames_anim / 3.0f);*/
		givePDamages(settings->getDamage(level));
		startAnimation(3.f);

		/*auto callbackAttack = CallFunc::create([&]() {
			attack();
			nb_attacks = 0;
			timer = 0;
			timerIDLE = 0;
			state = RELOADING;
			std::string frameName = name + "_attack_movement_000.png";
			SpriteFrameCache* cache = SpriteFrameCache::getInstance();
			setSpriteFrame(cache->getSpriteFrameByName(frameName.c_str()));
		});

		// create a sequence with the actions and callbacks
		auto seq = Sequence::create(Animate::create(currentAnimation), callbackAttack, nullptr);
		runAction(seq);*/
	}
}

void Cutter::handleEndEnrageAnimation() {
	nb_attacks = 0;
	timer = 0;
	timerIDLE = 0;
	state = RELOADING;
}

void Cutter::stopAttacking() {
	std::vector<Dango*> targets = otherTargets;
	for (auto& cTarget : targets) {
		if (cTarget != nullptr) {
			if (attacked_enemies.find(cTarget) != attacked_enemies.end()) {
				cTarget->removePDamages(attacked_enemies[cTarget]);
			}
			cTarget->removeTargetingTower(this);
		}
		removeTarget(cTarget);
	}
}

Tower::TowerType Cutter::getType()
{
	return TowerType::CUTTER;
}
