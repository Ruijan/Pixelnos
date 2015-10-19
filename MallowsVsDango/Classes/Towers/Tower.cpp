#include "Tower.h"
#include "../Level/Cell.h"
#include "../Dangos/Dango.h"
#include "../Towers/Bullet.h"
#include "../AppDelegate.h"
#include <iostream>

USING_NS_CC;

Tower::Tower(double nspeed = 1, double ndamage = 2, double nrange = 200, double ncost = 30) :
state(State::IDLE), fixed(false), destroy(false), target(nullptr), cost(ncost), 
attackSpeed(nspeed), damage(ndamage), range(nrange), timer(0), level(0)
{}

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

	loadingCircle = DrawNode::create();
	loadingCircle->setVisible(false);
	addChild(loadingCircle);
		
	DrawNode *circle = DrawNode::create();
	circle->setPosition(Vec2(getSpriteFrame()->getRect().size.width / 2,
		getSpriteFrame()->getRect().size.height / 2));
	circle->drawCircle(Vec2(0, 0), getRange() / getScaleX(),
		0, 60, false, 1, 1, Color4F(0, 0, 0, 1));
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
	if(level < 5){
		range *= 1.2;
		damage *= 1.5;
		attackSpeed *= 0.8;
		++level;
		removeChild(getChildByName("range"),true);
		DrawNode *circle = DrawNode::create();
		circle->setPosition(Vec2(getSpriteFrame()->getRect().size.width / 2,
			getSpriteFrame()->getRect().size.height / 2));
		circle->drawCircle(Vec2(0, 0), getRange() / getScaleX(),
			360, 60, true, 1, 1, Color4F(0, 0, 0, 1));
		addChild(circle, 0, "range");
	}
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
	double bestScore(0);
	bool chosen = false;

	for(auto cTarget : targets){
		if(cTarget != nullptr){
			int first = cTarget->getTargetedCell();
			double dist = cTarget->getPosition().distanceSquared(this->getPosition());
			double minDist = pow(getRange() + sqrt((pow(Cell::getCellWidth() * 3 / 8.0, 2) +
				pow(Cell::getCellHeight() * 3 / 8.0, 2))), 2);
			if (first > bestScore && dist < minDist && cTarget->willBeAlive()){
				bestScore = first;
				target = cTarget;
				chosen = true;
			}
		}
	}
	if(!chosen){
		target = nullptr;
	}
}

double Tower::getRange(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	return range * visibleSize.width / 960.0;
}

double Tower::getCost(){
	return cost;
}

Dango* Tower::getTarget(){
	return target;
}

double Tower::getDamage(){
	return damage;
}
double Tower::getAttackSpeed(){
	return attackSpeed;
}

cocos2d::Vector<SpriteFrame*> Tower::getAnimation(Tower::State animState){
	std::string action("");
	switch(animState){
		case IDLE:
			action = "steady";
			break;
		case ATTACKING:
			action = "attack";
			break;
		default:
			std::cerr << "No animation found for this state.";
			std::cerr << "Steady state animation used instead." << std::endl;
			action = "steady";
			break;
	};
	
	unsigned int animation_size = getSpecConfig()["animation_"+ action +"_size"].asInt();
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cocos2d::Vector<SpriteFrame*> animFrames;

	char str[100] = { 0 };
	for (int i = 0; i <= animation_size; ++i)
	{
		std::string frameName =  getSpecConfig()["name"].asString()+"_"+action+"_movement_%03d.png";
		sprintf(str, frameName.c_str(), i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	return animFrames;
}

cocos2d::Vector<SpriteFrame*> Tower::getAnimationFromName(std::string name, Tower::State animState){
	std::string action("");
	switch(animState){
		case IDLE:
			action = "steady";
			break;
		case ATTACKING:
			action = "attack";
			break;
		default:
			std::cerr << "No animation found for this state.";
			std::cerr << "Steady state animation used instead." << std::endl;
			action = "steady";
			break;
	};
	
	unsigned int animation_size = Tower::getConfig()[name]["animation_"+ action +"_size"].asInt();
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cocos2d::Vector<SpriteFrame*> animFrames;

	char str[100] = { 0 };
	for (int i = 0; i <= animation_size; ++i)
	{
		std::string frameName =  name+"_"+action+"_movement_%03d.png";
		sprintf(str, frameName.c_str(), i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	return animFrames;
}

void Tower::update(float dt) {
	// display update
	updateDisplay(dt);
	
	if(fixed){
		timer += dt;
		switch(state){
			case State::IDLE:
				if (target != nullptr){
					state = State::ATTACKING;
					target->takePDamages(damage);
					startAnimation();
				}
				break;
			case State::ATTACKING:
				if(currentAction->isDone()){
					currentAction->release();
					state = State::RELOADING;
					timer = 0;
					if(target != nullptr){
						shoot();
						
					}
				}
				break;
			case State::RELOADING:
				reload();
				break;
			default:
				state = IDLE;
				break;
		}
	}
	else{
		
	}
}

void Tower::updateDisplay(float dt){
	if(!fixed){
		float opacity = getOpacity();
		if (state == BLINKING_UP){
			if(opacity < 250){
				opacity = getOpacity() + 510 * dt < 250 ? getOpacity() + 510 * dt : 250;
				setOpacity(opacity);
			}
			else{
				state = BLINKING_DOWN;
			}
		}
		else if(state == BLINKING_DOWN){
			if(opacity > 50){
				opacity = getOpacity() + 510 * dt >50 ? getOpacity() - 510 * dt : 50;
				setOpacity(opacity);
			}
			else{
				state = BLINKING_UP;
			}
		}
		else{
			state = BLINKING_UP;
		}
	}
	else{
		setOpacity(255);
	}
}

bool Tower::hasToBeDestroyed(){
	return destroy;
}

void Tower::displayRange(bool disp){
	getChildByName("range")->setVisible(disp);
	loadingCircle->setVisible(disp);
}

void Tower::startAnimation(){
	cocos2d::Vector<SpriteFrame*> animFrames = getAnimation(state);
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.05f);
	currentAction = runAction(Animate::create(animation));
	currentAction->retain();
}

Json::Value Tower::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfig()["towers"];
}

void Tower::reload(){
	
	if (timer > attackSpeed){
		state = State::IDLE;
	}
	else{
		double iniAngle = M_PI_2;
		loadingCircle->clear();
		loadingCircle->drawSolidCircle(Vec2(0, 0), 0.11 * 50 / getScaleX(),
			0, 60, 1, 1, Color4F(0, 0, 0, 1));
		loadingCircle->drawSolidCircle(Vec2(0, 0), 0.1 * timer / attackSpeed * 50 / getScaleX(),
			0, 60, 1, 1, Color4F(1 - timer / attackSpeed, timer / attackSpeed, 0, 1));
		/*double angle1 = timer / attackSpeed * 2 * M_PI;
		if(-angle1 + iniAngle > M_PI){
			loadingCircle->drawSolidArc(Vec2(0, 0), 0.1 * getRange() / getScaleX(), -M_PI + iniAngle,
				-angle1 + iniAngle,60,Color4F(1 - timer / attackSpeed,timer / attackSpeed,0,1));
			angle1 = M_PI;
		}
		loadingCircle->drawSolidArc(Vec2(0, 0), 0.1 * getRange() / getScaleX(),iniAngle,
			-angle1 + iniAngle,60,Color4F(1 - timer / attackSpeed,timer / attackSpeed,0,1));*/
		
		
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

int Tower::getLevel(){
	return level;
}

void Tower::setTarget(Dango* dango){
	target = dango;
}

void Tower::setSelected(bool select){
	selected = select;
}

void Tower::shoot(){
	Bullet* bullet = Bullet::create("res/turret/bullet.png", target, damage);
	bullet->setPosition(getPosition() - Vec2(0, getSpriteFrame()->getRect().size.width / 2 * getScale()));
	SceneManager::getInstance()->getGame()->getLevel()->addBullet(bullet);
}
