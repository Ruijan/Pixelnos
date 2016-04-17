#include "Dangobese.h"
#include "../Level/Cell.h"
#include "../AppDelegate.h"
#include "../Towers/Bullet.h"

USING_NS_CC;

Dangobese::Dangobese(std::vector<Cell*> npath, int nlevel) :
Dango(npath, Dangobese::getConfig()["level"][nlevel]["speed"].asDouble(),
	Dangobese::getConfig()["level"][nlevel]["hitpoints"].asDouble(),nlevel,
	Dangobese::getConfig()["level"][nlevel]["attack"].asDouble(),
	Dangobese::getConfig()["level"][nlevel]["reload"].asDouble()),
	attack_duration(Dangobese::getConfig()["level"][nlevel]["attack_duration"].asDouble()),
	attack_timer(0){
}

Dangobese::~Dangobese() {
	//std::cerr << "Dangobese Destroyed ! confirmed !" << std::endl;
}

Dangobese* Dangobese::create(std::vector<Cell*> npath, int nlevel)
{
	Dangobese* pSprite = new Dangobese(npath, nlevel);

	if (pSprite->initWithFile(getConfig()["level"][nlevel]["image"].asString()))
	{
		pSprite->setScale(Cell::getCellWidth() / pSprite->getContentSize().width);
		pSprite->autorelease();

		pSprite->setAnchorPoint(Point(0.5,0.25));
		pSprite->updateAnimation();

		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

Json::Value Dangobese::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfig()["dangos"]["dangobese"];
}
Json::Value Dangobese::getSpecConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfig()["dangos"]["dangobese"];
}

void Dangobese::attack(float dt) {
	// list all the bullets in the area to suck
	Size visibleSize = Director::getInstance()->getVisibleSize();

	std::vector<Attack*> attacks = ((SceneManager*)SceneManager::getInstance())->getGame()->getLevel()->getAttacks();
	for (auto& attack : attacks) {
		if (!isAffectedByAttack(attack)) {
			double distance = attack->getPosition().distance(getPosition());
			double speed = 500;
			double time_to_reach = distance / speed;
			if (distance < 150*visibleSize.width/960 && 
				time_to_reach < attack_duration - attack_timer && 
				!((WaterBall*)attack)->isForceApplied()){
				Vec2 direction = Vec2(attack->getPosition().x - getPosition().x,
					attack->getPosition().y - getPosition().y).getNormalized();
				Vec2 npos = attack->getPosition() - direction * speed * dt * visibleSize.width / 960;
				attack->setPosition(npos);
				attack->setEnabled(false);
				((WaterBall*)attack)->setForceApplied(true);
				if (distance < 20 * visibleSize.width / 960) {
					attack->setHasToBeDeleted(true);
				}
			}
		}
	}
}

bool Dangobese::shouldAttack() {
	std::vector<Attack*> attacks = ((SceneManager*)SceneManager::getInstance())->getGame()->getLevel()->getAttacks();
	for (auto& attack : attacks) {
		if (!isAffectedByAttack(attack)) {
			double distance = sqrt(pow(attack->getPosition().x - getPosition().x, 2) +
				pow(attack->getPosition().y - getPosition().y, 2));
			if (distance < 150 && !((WaterBall*)attack)->isForceApplied()) {
				return true;
			}
		}
	}
	return false;
}

void Dangobese::runAnimation(Animation* anim) {
	if (state == MOVE) {
		cAction = runAction(RepeatForever::create(Animate::create(anim)));
	}
	else if (state == ATTACK) {
		cAction = runAction(RepeatForever::create(Animate::create(anim)));
	}
}

void Dangobese::update(float dt) {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	reload_timer += dt;
	switch (state) {
	case IDLE:
		if (shouldAttack() && reload_timer > attack_reloading) {
			state = ATTACK;
			updateAnimation();
			reload_timer = 0;
		}
		else if(path[targetedCell]->isFree()) {
			state = MOVE;
			updateAnimation();
		}
		break;
	case ATTACK:
		attack_timer += dt;
		if (attack_timer < attack_duration) {
			attack(dt);
		}
		else {
			state = IDLE;
			attack_timer = 0;
			reload_timer = 0;
		}
		break;
	case MOVE:
		if (path[targetedCell]->isFree()) {
			move(dt);
			if (shouldAttack() && reload_timer > attack_reloading) {
				state = ATTACK;
				updateAnimation();
				reload_timer = 0;
			}
		}
		else {
			state = IDLE;
		}
		break;
	}
}

bool Dangobese::isAffectedByAttack(Attack* attack) {
	return attack->affectEnemy(this);
}
