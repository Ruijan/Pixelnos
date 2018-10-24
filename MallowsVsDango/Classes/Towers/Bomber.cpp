#include "Bomber.h"
#include "../Towers/Attacks/ThrowableAttacks/WaterBall.h"
#include "../Towers/Attacks/ThrowableAttacks/WaterBombBall.h"
#include "../AppDelegate.h"

USING_NS_CC;

Bomber::Bomber() : Tower(), nb_limit_attack(0){
}

Bomber* Bomber::create(Config* configClass, Level* globalLevel)
{
	Bomber* bomber = new Bomber();

	/*if (pSprite->initWithFile(Bomber::getConfig()["image"].asString()))
	{*/
	bomber->initFromConfig(configClass, globalLevel);
	bomber->initDebug();
	bomber->initEnragePanel();
	return bomber;
	//}

	CC_SAFE_DELETE(bomber);
	return NULL;
}

const Json::Value& Bomber::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::TOWER)["bomber"];
}

const Json::Value& Bomber::getSpecConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::TOWER)["bomber"];
}

void Bomber::attack(){
	if (target != nullptr) {
		Size visibleSize = Director::getInstance()->getVisibleSize();
		WaterBall* ball = nullptr;
		std::string file;
		
		spBone* water_ball;
		switch (direction) {
		case DOWN:
			water_ball = skeleton->findBone("yellow_bomb");
			file = "res/turret/yellow_bomb.png";
			break;
		case UP:
			water_ball = skeleton->findBone("green_bomb");
			file = "res/turret/green_bomb.png";
			break;
		case RIGHT:
			water_ball = skeleton->findBone("red_bomb");
			file = "res/turret/red_bomb.png";
			break;
		case LEFT:
			water_ball = skeleton->findBone("red_bomb");
			file = "res/turret/red_bomb.png";
			break;
		default:
			water_ball = skeleton->findBone("yellow_bomb");
			file = "res/turret/red_bomb.png";
			break;
		}
		//log("--> bullet thrown with id %i", attacked_enemies[target]);
		if (level >= settings->getMaxExistingLevel() - 1) {
			ball = WaterBombBall::create(target, settings->getDamage(level), 500 * visibleSize.width / 960, 100);
		}
		else {
			ball = WaterBall::createWithFile(file, target, settings->getDamage(level), 500 * visibleSize.width / 960);
		}
		ball->setDamagesId(attacked_enemies[target]);
		attacked_enemies.erase(attacked_enemies.find(target));
		target->addTargetingAttack(ball);
		ball->setPosition(getPosition() + skeleton->getPosition() + Vec2(water_ball->worldX, water_ball->worldY));
		//ball->setPosition(getPosition() - Vec2(0, getSpriteFrame()->getRect().size.width / 2 * getScale()));
		ball->setScale(0.020 * visibleSize.width / ball->getContentSize().width);

		globalLevel->addAttack(ball);
		if (target != nullptr) {
			target->removeTargetingTower(this);
		}
		target = nullptr;
		++nb_attacks;
	}
}

void Bomber::startLimit() {
	chooseTarget(globalLevel->getEnemies());

	if (isLimitReached() && nb_limit_attack < 2 && target != nullptr) {
		if (state == ATTACKING) {
			target->removePDamages(attacked_enemies[target]);
			attacked_enemies.empty();
		}
		state = LIMIT_BURSTING;
		log("LIMIT STARTED, round %i", nb_limit_attack);

		givePDamages(settings->getDamage(level));
		startAnimation(2.f);
		++nb_limit_attack;
		//((Label*)getChildByName("label_state"))->setString("LIMIT_BURSTING");
		/*cocos2d::Vector<SpriteFrame*> animFrames = getAnimation(ATTACKING);
		Animation* currentAnimation = Animation::createWithSpriteFrames(animFrames,
			animation_duration / nb_frames_anim / 3.f);*/

		/*auto callbackAttack = CallFunc::create([&]() {
			givePDamages(damage);
			attack();
			startLimit();
		});*/

		// create a sequence with the actions and callbacks
		//auto seq = Sequence::create(Animate::create(currentAnimation), callbackAttack, nullptr);
		
		//runAction(seq);
	}
	else if(isLimitReached()) {
		nb_attacks = 0;
		nb_limit_attack = 0;
		timer = 0;
		timerIDLE = 0;
		state = RELOADING;
		log("LIMIT STOPPED");
		skeleton->setSkin("normal" + Value(level + 1).asString());
		/*std::string frameName = name + "_attack_movement_000.png";
		SpriteFrameCache* cache = SpriteFrameCache::getInstance();
		setSpriteFrame(fache->getSpriteFrameByName(frameName.c_str()));*/
	}
}

void Bomber::handleEndEnrageAnimation() {
	startLimit();
}

Tower::TowerType Bomber::getType()
{
	return Tower::TowerType::BOMBER;
}