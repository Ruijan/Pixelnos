#include "Saucer.h"
#include "../Towers/Attack.h"
#include "../AppDelegate.h"

USING_NS_CC;

Saucer::Saucer() : Tower(), nb_limit_attack(0){
}

Saucer* Saucer::create(Config* configClass)
{
	Saucer* pSprite = new Saucer();

	pSprite->initFromConfig(configClass);
	pSprite->initSpecial();
	pSprite->initDebug();
	pSprite->initEnragePanel();
	return pSprite;

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

void Saucer::initSpecial() {
	auto config = getSpecConfig();
	slow_percent = config["slow"][level].asDouble();
	slow_duration = config["slow_duration"][level].asDouble();
	/*((AppDelegate*)Application::getInstance())->g_screenLog->log(LL_DEBUG, ("Slow fail value: " + Value(slow_percent).asString()).c_str());
	((AppDelegate*)Application::getInstance())->g_screenLog->log(LL_DEBUG, ("Slow fail abs value: " + Value(abs(slow_percent)).asString()).c_str());*/
}

const Json::Value& Saucer::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::TOWER)["saucer"];
}

const Json::Value& Saucer::getSpecConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::TOWER)["saucer"];
}

void Saucer::attack(){
	if (target != nullptr) {
		Size visibleSize = Director::getInstance()->getVisibleSize();
		ChocoSpit* spit = nullptr;
		if (level >= (int)getConfig()["cost"].size() - 1) {
			//spit = AcidChocoSpit::create(target, damage, 500 * visibleSize.width / 960);
			spit = ChocoSpit::create(target, settings->getDamage(level), slow_percent, slow_duration, 500 * visibleSize.width / 960);
		}
		else {
			spit = ChocoSpit::create(target, settings->getDamage(level), slow_percent, slow_duration, 500 * visibleSize.width / 960);
		}
		spit->setDamagesId(attacked_enemies[target]);
		attacked_enemies.erase(attacked_enemies.find(target));
		target->addTargetingAttack(spit);
		spit->setPosition(getPosition() + skeleton->getPosition() + Vec2(skeleton->findBone("spit")->worldX, skeleton->findBone("spit")->worldY));
		spit->setScale(0.015 * visibleSize.width / spit->getContentSize().width);

		Vec2 direction = target->getPosition() - spit->getPosition();
		//spit->setRotation(180 * direction.getAngle() / M_PI);
		SceneManager::getInstance()->getGame()->getLevel()->addAttack(spit);
		if (target != nullptr) {
			target->removeTargetingTower(this);
		}
		target = nullptr;
		++nb_attacks;
	}
}

void Saucer::startLimit() {
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
			givePDamages(settings->getDamage(level));
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
	}
}

void Saucer::handleEndEnrageAnimation() {
	startLimit();
}

bool Saucer::isPotentialTarget(Dango* cTarget) {
	double dist = cTarget->getPosition().distanceSquared(this->getPosition());
	double minDist = pow(getRange(), 2);
	return   dist <= minDist && cTarget->willBeAlive() && cTarget->getSpeedRedtuctionRatio() > slow_percent;
}

Tower::TowerType Saucer::getType()
{
	return TowerType::SAUCER;
}
