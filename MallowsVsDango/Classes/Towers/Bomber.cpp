#include "Bomber.h"
#include "../Towers/Attack.h"
#include "../AppDelegate.h"

USING_NS_CC;

Bomber::Bomber() : Tower(), nb_limit_attack(0){
}

Bomber* Bomber::create()
{
	Bomber* bomber = new Bomber();

	/*if (pSprite->initWithFile(Bomber::getConfig()["image"].asString()))
	{*/
	bomber->initFromConfig();
	bomber->initDebug();
	bomber->initEnragePanel();
	return bomber;
	//}

	CC_SAFE_DELETE(bomber);
	return NULL;
}

const Json::Value& Bomber::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues()["towers"]["bomber"];
}

const Json::Value& Bomber::getSpecConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues()["towers"]["bomber"];
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
		if (level >= (int)getConfig()["cost"].size() - 1) {
			ball = WaterBombBall::create(target, damage, 500 * visibleSize.width / 960, 100);
		}
		else {
			ball = WaterBall::createWithFile(file, target, damage, 500 * visibleSize.width / 960);
		}
		ball->setDamagesId(attacked_enemies[target]);
		attacked_enemies.erase(attacked_enemies.find(target));
		target->addTargetingAttack(ball);
		ball->setPosition(getPosition() + skeleton->getPosition() + Vec2(water_ball->worldX, water_ball->worldY));
		//ball->setPosition(getPosition() - Vec2(0, getSpriteFrame()->getRect().size.width / 2 * getScale()));
		ball->setScale(0.020 * visibleSize.width / ball->getContentSize().width);

		SceneManager::getInstance()->getGame()->getLevel()->addAttack(ball);
		if (target != nullptr) {
			target->removeTargetingTower(this);
		}
		target = nullptr;
		++nb_attacks;
	}
}

void Bomber::startLimit() {
	chooseTarget(SceneManager::getInstance()->getGame()->getLevel()->getEnemies());

	if (isLimitReached() && nb_limit_attack < 2 && target != nullptr) {
		if (state == ATTACKING) {
			target->removePDamages(attacked_enemies[target]);
			attacked_enemies.empty();
		}
		state = LIMIT_BURSTING;
		log("LIMIT STARTED, round %i", nb_limit_attack);

		givePDamages(damage);
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

bool Bomber::isSameType(std::string type) {
	return Tower::getTowerTypeFromString(type) == Tower::TowerType::BOMBER;
}
