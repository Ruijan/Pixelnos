#include "Saucer.h"
#include "../Towers/Attack.h"
#include "../AppDelegate.h"
#include "../Level/Cell.h"
//#include "../Lib/ScreenLog.h"

USING_NS_CC;

Saucer::Saucer() : Tower(), nb_limit_attack(0){
}

Saucer* Saucer::create()
{
	Saucer* pSprite = new Saucer();

	/*if (pSprite->initWithFile(Saucer::getConfig()["image"].asString()))
	{*/
	pSprite->initFromConfig();
	pSprite->initSpecial();
	pSprite->initDebug();
	pSprite->initEnragePanel();
	return pSprite;
	//}

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
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues()["towers"]["saucer"];
}

const Json::Value& Saucer::getSpecConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues()["towers"]["saucer"];
}

void Saucer::attack(){
	if (target != nullptr) {
		Size visibleSize = Director::getInstance()->getVisibleSize();
		ChocoSpit* spit = nullptr;
		if (level >= (int)getConfig()["cost"].size() - 1) {
			//spit = AcidChocoSpit::create(target, damage, 500 * visibleSize.width / 960);
			spit = ChocoSpit::create(target, damage, slow_percent, slow_duration, 500 * visibleSize.width / 960);
		}
		else {
			spit = ChocoSpit::create(target, damage, slow_percent, slow_duration, 500 * visibleSize.width / 960);
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
		/*std::string frameName = name + "_attack_movement_000.png";
		SpriteFrameCache* cache = SpriteFrameCache::getInstance();
		setSpriteFrame(cache->getSpriteFrameByName(frameName.c_str()));*/
	}
}

void Saucer::handleEndEnrageAnimation() {
	startLimit();
}

bool Saucer::isSameType(std::string type) {
	return Tower::getTowerTypeFromString(type) == Tower::TowerType::SAUCER;
}

void Saucer::chooseTarget(std::vector<Dango*> targets) {
	double bestScore(1000);
	bool chosen = false;
	for (auto& cTarget : targets) {
		if (cTarget != nullptr) {
			int first = cTarget->getNbCellsToPath();
			double dist = cTarget->getPosition().distanceSquared(this->getPosition());
			double minDist = pow(getRange(), 2);
			/*((AppDelegate*)Application::getInstance())->g_screenLog->log(LL_DEBUG, ("first: " + Value(first).asString() + " bestscore: " + Value(bestScore).asString()).c_str());
			((AppDelegate*)Application::getInstance())->g_screenLog->log(LL_DEBUG, ("speed: " + Value(abs(cTarget->getSpeedRedtuctionRatio())).asString() + " slow: " + Value(abs(slow_percent)).asString()).c_str());
			((AppDelegate*)Application::getInstance())->g_screenLog->log(LL_DEBUG, ("speed: " + Value(abs(cTarget->getSpeedRedtuctionRatio())).asString() + " slow: " + Value(abs(-0.5)).asString()).c_str());
			*/
			if (first < bestScore && dist <= minDist && cTarget->willBeAlive() &&
				cTarget->getSpeedRedtuctionRatio() > slow_percent) {
				bestScore = first;
				if (target != nullptr) {
					target->removeTargetingTower(this);
				}
				target = cTarget;
				target->addTargetingTower(this);
				chosen = true;
			}
		}
	}
	if (!chosen) {
		if (target != nullptr) {
			target->removeTargetingTower(this);
		}
		target = nullptr;
	}
}