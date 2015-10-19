#include "Dango.h"
#include "../Level/Cell.h"
#include "../AppDelegate.h"
#include <iostream>

USING_NS_CC;

Dango::Dango(std::vector<Cell*> npath, double nspeed = Dango::getConfig()["speed"].asDouble(), double hp = Dango::getConfig()["hitpoints"].asDouble()) : path(npath),
targetedCell(0), speed(nspeed), hitPoints(hp), cAction(nullptr), cDirection(IDLE), pDamages(0.0) {
	//move = nullptr;
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("res/dango/animations/dango1.plist", "res/dango/animations/dango1.png");
}

Dango::~Dango() { 
	std::cerr << "Dango Destroyed ! confirmed !" << std::endl;
}

Dango* Dango::create(std::string image, std::vector<Cell*> npath)
{
	Dango* pSprite = new Dango(npath);

	if (pSprite->initWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(image)))
	{
		pSprite->setScale(Cell::getCellWidth() / pSprite->getContentSize().width);
		pSprite->autorelease();

		pSprite->initOptions();
		pSprite->setAnchorPoint(Point(0.5,0.25));
		/*Node* healthBar = Node::create();
		for(int i(0); i < pSprite->getHitPoints(); ++i){
			auto rectNode = DrawNode::create();
			
			Vec2 rectangle[4];
			int shift1(25);
			int width(10);
			int height(15);
			int shift2(pSprite->getSpriteFrame()->getRect().size.height);
			rectangle[0] = Vec2(-width+i*shift1, -height + shift2);
			rectangle[1] = Vec2(width+i*shift1, -height + shift2);
			rectangle[2] = Vec2(width+i*shift1, height + shift2);
			rectangle[3] = Vec2(-width+i*shift1, height + shift2);

			Color4F green(0, 1, 0, 1);
			Color4F black(0, 0, 0, 1);
			rectNode->drawPolygon(rectangle, 4, green, 0, black);
			healthBar->addChild(rectNode,0,i);
			
		}
		pSprite->addChild(healthBar,0,1);*/

		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}


void Dango::initOptions()
{
	// do things here like setTag(), setPosition(), any custom logic.
}


void Dango::update(float dt) {
	move(dt);
}

void Dango::move(float dt){
	double distance = path[targetedCell]->getPosition().distanceSquared(getPosition());
	double distance2(0);
	double distance3(0);
	if (targetedCell - 1 >= 0){
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
	
	std::string preString;
	double x = this->getScaleX();
	switch(cDirection){
		case UP:
			preString = "dango1_ju_";
			break;
		case RIGHT:
			this->setScaleX(((x > 0) - (x < 0))* x);
			preString = "dango1_j_";
			break;
		case LEFT:
			this->setScaleX(-((x > 0) - (x < 0))*x);
			preString = "dango1_j_";
			break;
		case DOWN:
			this->setScaleX(-((x > 0) - (x < 0))*x);
			preString = "dango1_jd_";
			break;
	}
	cocos2d::Vector<SpriteFrame*> animFrames;
	char str[100] = { 0 };
	for (int i = 0; i < 24; ++i){
		sprintf(str, "%s%03d.png", preString.c_str(), i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	//Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.041f);
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 
		Cell::getCellWidth() / getSpeed() / 24 * 1.1);
	cAction = runAction(RepeatForever::create(Animate::create(animation)));
}

double Dango::getHitPoints(){
	return hitPoints;
}

double Dango::getGain(){
	return ((AppDelegate*)Application::getInstance())->getConfig()["dangos"]["dango"]["gain"].asDouble();
}

void Dango::takeDamages(double damages){
	hitPoints -= damages;
	if(hitPoints < 0){
		hitPoints = 0;
	}
	/*for(int i(damages-1); i >= 0; --i){
		getChildByTag(1)->removeChild(getChildByTag(1)->getChildByTag(hitPoints+i));
	}*/
	
}

void Dango::takePDamages(double damages){
	pDamages -= damages;
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
	return ((AppDelegate*)Application::getInstance())->getConfig()["dangos"]["dango"];
}

int Dango::getTargetedCell(){
	return targetedCell;
}

double Dango::getSpeed(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	return speed * visibleSize.width / 960.0;
}
