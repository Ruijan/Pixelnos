#include "Dango.h"
#include "../Level/Cell.h"
#include "../AppDelegate.h"
#include "../Level/Wall.h"

USING_NS_CC;

Dango::Dango(std::vector<Cell*> npath, double nspeed, double hp, int nlevel, 
	double damages, double a_reload) : path(npath), targetedCell(0), speed(nspeed),
	hitPoints(hp), level(nlevel), cAction(nullptr), cDirection(RIGHT), pDamages(0.0), 
	state(IDLE), reload_timer(0), attack_damages(damages), attack_reloading(a_reload){
}

Dango::~Dango() { 
	//std::cerr << "Dango Destroyed ! confirmed !" << std::endl;
}

void Dango::update(float dt) {
	switch (state) {
		case IDLE:
			if (!path[targetedCell]->isFree()) {
				state = ATTACK;
				updateAnimation();
			}
			else {
				state = MOVE;
				updateAnimation();
			}
			break;
		case ATTACK:
			if (!path[targetedCell]->isFree()) {
				attack(dt);
				state = RELOAD;
			}
			else {
				state = MOVE;
				updateAnimation();
			}
			break;
		case RELOAD:
			reload_timer += dt;
			if (reload_timer > attack_reloading) {
				reload_timer = 0;
				if (!path[targetedCell]->isFree()) {
					state = ATTACK;
					updateAnimation();
				}
				else {
					state = MOVE;
					updateAnimation();
				}
			}
			break;
		case MOVE:
			move(dt);
			if (!path[targetedCell]->isFree()) {
				state = ATTACK;
				updateAnimation();
			}
			break;
	}
}

void Dango::move(float dt){
	
	double distance = path[targetedCell]->getPosition().distanceSquared(getPosition());
	double distance2(0);
	double distance3(0);
	if (targetedCell >= 1){
		distance2 = path[targetedCell]->getPosition().distanceSquared(path[targetedCell - 1]->getPosition());
		distance3 = path[targetedCell-1]->getPosition().distanceSquared(getPosition());
	}
	if (targetedCell == 0 || distance3 >= distance2){
		if (targetedCell + 1 < path.size()){
			++targetedCell;
			Vec2 direction = path[targetedCell]->getPosition() - path[targetedCell - 1]->getPosition();
			direction.normalize();
			updateDirection(direction);
		}
		else{
			setPosition(path[targetedCell]->getPosition());
		}
	}
	else{
		Vec2 previousPos = getPosition();
		Vec2 direction = path[targetedCell]->getPosition() - path[targetedCell - 1]->getPosition();
		direction.normalize();
		updateDirection(direction);
		setPosition(previousPos + direction * getSpeed() * dt);
	}
	
}

void Dango::updateDirection(Vec2 direction) {
	DIRECTION nDirection = cDirection;
	if (direction.x > direction.y && direction.x > 0) {
		nDirection = RIGHT;
	}
	else if (abs(direction.x) > abs(direction.y) && direction.x < 0) {
		nDirection = LEFT;
	}
	else if (abs(direction.x) < abs(direction.y) && direction.y < 0) {
		nDirection = DOWN;
	}
	else if (abs(direction.x) < abs(direction.y) && direction.y > 0) {
		nDirection = UP;
	}
	if (nDirection != cDirection) {
		cDirection = nDirection;
		updateAnimation();
	}
}

void Dango::attack(float dt) {
	((Wall*)(path[targetedCell]->getObject()))->takeDamages(attack_damages);
}

void Dango::updateAnimation(){
	if(cAction != nullptr){
		stopAction(cAction);
	}
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	std::string preString(getSpecConfig()["level"][level]["name"].asString());
	double x = this->getScaleX();
	if (state == ATTACK) {
		switch (cDirection) {
		case UP:
			preString += "_ju_";
			break;
		case RIGHT:
			this->setScaleX(((x > 0) - (x < 0))* x);
			preString += "_attack_";
			break;
		case LEFT:
			this->setScaleX(-((x > 0) - (x < 0))*x);
			preString += "_attack_";
			break;
		case DOWN:
			this->setScaleX(-((x > 0) - (x < 0))*x);
			preString += "_jd_";
			break;
		}
	}
	else {
		switch (cDirection) {
		case UP:
			preString += "_ju_";
			break;
		case RIGHT:
			this->setScaleX(((x > 0) - (x < 0))* x);
			preString += "_j_";
			break;
		case LEFT:
			this->setScaleX(-((x > 0) - (x < 0))*x);
			preString += "_j_";
			break;
		case DOWN:
			this->setScaleX(-((x > 0) - (x < 0))*x);
			preString += "_jd_";
			break;
		}
	}
	cocos2d::Vector<SpriteFrame*> animFrames;
	char str[100] = { 0 };
	for (int i = 0; i < 24; ++i){
		sprintf(str, "%s%03d.png", preString.c_str(), i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 
		Cell::getCellWidth() / getSpeed() / 24 * getSpecConfig()["cellperanim"].asFloat());
	if (state == MOVE) {
		cAction = runAction(RepeatForever::create(Animate::create(animation)));
	}
	else if(state == ATTACK) {
		cAction = runAction(Animate::create(animation));
	}
}

double Dango::getHitPoints(){
	return hitPoints;
}

double Dango::getGain(){
	return getSpecConfig()["level"][level]["gain"].asDouble();
}

void Dango::takeDamages(double damages){
	runAction(Sequence::create(Hide::create(), DelayTime::create(0.1f), Show::create(), nullptr));
	hitPoints -= damages;
	pDamages -= damages;
	if(hitPoints < 0){
		hitPoints = 0;
	}
	if (pDamages < 0) {
		log("We have a problem, the prospective damages should not be below zero");
	}
}

void Dango::takePDamages(double damages){
	pDamages += damages;
}


bool Dango::isAlive(){
	return hitPoints > 0;
}

bool Dango::willBeAlive(){
	return (hitPoints - pDamages > 0);
}

bool Dango::isDone(){
	double distance = path[targetedCell]->getPosition().distanceSquared(getPosition());
	return (targetedCell == path.size() - 1 && distance < 10);
}

Json::Value Dango::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfig()["dangos"];
}

int Dango::getTargetedCell(){
	return targetedCell;
}

double Dango::getSpeed(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	return speed * visibleSize.width / 960.0;
}
