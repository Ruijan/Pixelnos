#include "Tower.h"
#include "../Level/Cell.h"
#include "../Dangos/Dango.h"
#include "../Towers/Scorpion.h"
#include "../Towers/Grill.h"
#include "../AppDelegate.h"
#include <iostream>

USING_NS_CC;

Tower::Tower(double nspeed = 1, double ndamage = 2, double nrange = 200, double ncost = 30) :
cost(ncost), state(State::IDLE), fixed(false), destroy(false), menu(nullptr), target(0), attackSpeed(nspeed), damage(ndamage), range(nrange), timer(0) {
}

Tower::~Tower() {}

/*Tower* Tower::create()
{
	Tower* pSprite = new Tower();
	std::string name = Tower::getConfig()["image"].asString();
	if (pSprite->initWithFile(name))
	{
		pSprite->initDebug();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}
*/
void Tower::initDebug(){
	setScale(Cell::getCellWidth() / getContentSize().width);
	autorelease();

	DrawNode *circle = DrawNode::create();
	circle->setPosition(Vec2(getSpriteFrame()->getRect().size.width / 2,
		getSpriteFrame()->getRect().size.height / 2));
	circle->drawCircle(Vec2(0, 0), getRange() / getScaleX(),
		360, 60, true, 1, 1, Color4F(0, 0, 0, 1));
	circle->setVisible(false);
	addChild(circle, 0, "range");
}



void Tower::destroyCallback(Ref* sender){
	destroy = true;
}

void Tower::builtCallback(Ref* sender){
	fixed = true;
}

void Tower::upgradeCallback(Ref* sender){

}

bool Tower::isFixed(){
	return fixed;
}

void Tower::setFixed(bool f){
	fixed = f;
}

bool Tower::isSelected(){
	return selected;
}

void Tower::chooseTarget(std::vector<Dango*> targets){
	if(state == State::IDLE){
		double bestScore(0);
		bool chosen = false;

		for(auto cTarget : targets){
			int first = cTarget->getTargetedCell();
			double dist = cTarget->getPosition().distanceSquared(this->getPosition());
			double minDist = pow(getRange() + sqrt((pow(Cell::getCellWidth() * 3 / 8.0, 2) +
				pow(Cell::getCellHeight() * 3 / 8.0, 2))), 2);
			if (first > bestScore && dist < minDist){
				bestScore = first;
				target = cTarget;
				chosen = true;
			}
		}
		if(!chosen){
			target = nullptr;
		}
	}
}

double Tower::getRange(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	return range * visibleSize.width / 960.0;
}

double Tower::getCost(){
	return cost;
}

void Tower::update(float dt) {
	// display update
	//updateDisplay(dt);
	if(fixed){
		timer += dt;
		switch(state){
			case State::IDLE:
				attack();
				break;
			case State::ATTACKING:
				if(currentAction->isDone()){
					currentAction->release();
					state = State::IDLE;
					if(target != nullptr){
						target->takeDamages(damage);
					}
				}
				break;
			
		}
	}
}

bool Tower::hasToBeDestroyed(){
	return destroy;
}

void Tower::displayRange(bool disp){
	getChildByName("range")->setVisible(disp);
}

void Tower::startAnimation(){
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cocos2d::Vector<SpriteFrame*> animFrames;

	char str[100] = { 0 };
	for (int i = 1; i < 16; ++i)
	{
		sprintf(str, "archer_movement%d.png", i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.05f);
	currentAction = runAction(Animate::create(animation));
	currentAction->retain();
}

Json::Value Tower::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfig()["towers"];
}

void Tower::attack(){
	
	if (timer > attackSpeed){
		if (target != nullptr){
			state = State::ATTACKING;
			startAnimation();
			timer = 0;
		}
	}
	else{
		getChildByName("range")->setRotation(timer / attackSpeed * 360);
	}
}

Tower::TowerType Tower::getTowerTypeFromString(std::string type){
	if (!strcmp(type.c_str(), "archer")){
		return Tower::TowerType::ARCHER;
	}
	else if (!strcmp(type.c_str(), "grill")){
		return Tower::TowerType::GRILL;
	}
	else{
		return Tower::TowerType::ARCHER;
	}
}

Tower::State Tower::getState(){
	return state;
}

void Tower::setState(Tower::State nstate){
	state = nstate;
}

void Tower::setSelected(bool select){
	selected = select;
}
