#include "Dango.h"
#include "../Level/Cell.h"
#include "../AppDelegate.h"

USING_NS_CC;

Dango::Dango(std::vector<Cell*> npath, double nspeed, double hp, int nlevel) :
	path(npath), targetedCell(0), speed(nspeed), hitPoints(hp), level(nlevel),
	cAction(nullptr), cDirection(RIGHT), pDamages(0.0), state(IDLE), timer(0) {
}

Dango::~Dango() { 
	//std::cerr << "Dango Destroyed ! confirmed !" << std::endl;
}

void Dango::update(float dt) {
	move(dt);
	if(state == HIT){
		timer += dt;
		setVisible(false);
		if(timer > 0.1){
			state = IDLE;
			setVisible(true);
		}
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
		}
		else{
			setPosition(path[targetedCell]->getPosition());
		}
	}
	else{
		Vec2 previousPos = getPosition();
		Vec2 direction = path[targetedCell]->getPosition() - path[targetedCell - 1]->getPosition();
		direction.normalize();
		DIRECTION nDirection = cDirection;
		if(direction.x > direction.y && direction.x > 0){
			nDirection = RIGHT;
		}
		else if(abs(direction.x) > abs(direction.y) && direction.x < 0){
			nDirection = LEFT;
		}
		else if(abs(direction.x) < abs(direction.y) && direction.y < 0){
			nDirection = DOWN;
		}
		else if(abs(direction.x) < abs(direction.y) && direction.y > 0){
			nDirection = UP;
		}
		
		if(nDirection != cDirection){
			cDirection = nDirection;
			updateAnimation();
		}
		setPosition(previousPos + direction * getSpeed() * dt);
	}
}

void Dango::updateAnimation(){
	if(cAction != nullptr){
		stopAction(cAction);
	}
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	std::string preString(getSpecConfig()["level"][level]["name"].asString());
	double x = this->getScaleX();
	switch(cDirection){
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
	cocos2d::Vector<SpriteFrame*> animFrames;
	char str[100] = { 0 };
	for (int i = 0; i < 24; ++i){
		sprintf(str, "%s%03d.png", preString.c_str(), i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 
		Cell::getCellWidth() / getSpeed() / 24 * getSpecConfig()["cellperanim"].asFloat());
	cAction = runAction(RepeatForever::create(Animate::create(animation)));
}

double Dango::getHitPoints(){
	return hitPoints;
}

double Dango::getGain(){
	return getSpecConfig()["level"][level]["gain"].asDouble();
}

void Dango::takeDamages(double damages){
	state = HIT;
	timer = 0;
	hitPoints -= damages;
	pDamages -= damages;
	if(hitPoints < 0){
		hitPoints = 0;
	}
}

void Dango::takePDamages(double damages){
	pDamages += damages;
}


bool Dango::isAlive(){
	if(hitPoints > 0){ return true;}
	else{return false;}
}

bool Dango::willBeAlive(){
	if(hitPoints - pDamages > 0){ return true;}
	else{return false;}
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
