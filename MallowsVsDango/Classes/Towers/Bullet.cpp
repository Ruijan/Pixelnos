#include "Bullet.h"
#include "../Dangos/Dango.h"
#include "../Towers/Tower.h"
#include "../Lib/Functions.h"
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;


Bullet::Bullet(Dango* ntarget, double ndamages, double nspeed, 
bool nrotate): target(ntarget), 
damages(ndamages), touched(false), speed(nspeed), rotate(nrotate),
action(nullptr),hasToBeDeleted(false){
}

Bullet::~Bullet(){
	
}

Bullet* Bullet::create(std::string image, Dango* ntarget, double damages, 
double nspeed, bool nrotate){
	Bullet* pSprite = new Bullet(ntarget, damages, nspeed, nrotate);

	if (pSprite->initWithFile(image))
	{
		pSprite->autorelease();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

void Bullet::update(float dt){
	if(target != nullptr){
		Vec2 direction = target->getPosition() - getPosition();
		double distance = sqrt(direction.x*direction.x + direction.y*direction.y);
		Size visibleSize = Director::getInstance()->getVisibleSize();
		if(distance < 10*visibleSize.width/960 && !touched){
			touched = true;
			target->takeDamages(damages);
			startAnimation();
		}
		else{
			direction.normalize();
			setPosition(getPosition() + direction * speed * dt);
			setRotation(getRotation() + 360 * dt);
		}
	}
	else{
		hasToBeDeleted = true;
	}
}

bool Bullet::hasTouched(){
	return touched;
}

bool Bullet::isDone(){
	if(touched){
		return action->isDone();
	}
	else{
		return hasToBeDeleted;
	}
}

Dango* Bullet::getTarget(){
	return target;
}

void Bullet::setTarget(Dango* dango){
	target = dango;
}

void Bullet::startAnimation(){
	setVisible(true);
	unsigned int animation_size = Tower::getConfig()[owner]["animation_bullet_size"].asInt();
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cocos2d::Vector<SpriteFrame*> animFrames;

	char str[100] = { 0 };
	std::vector<std::string> elements;
	std::string animation = split(Tower::getConfig()[owner]["animation_bullet"].asString(),'/',elements).back();
	animation = animation.substr(0,animation.size()-4);
	for (unsigned int i = 0; i <= animation_size; ++i)
	{
		std::string frameName =  animation+"_%03d.png";
		sprintf(str, frameName.c_str(), i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	setVisible(true);
	setScale(1 / 6.0f);
	Animation* currentAnimation = Animation::createWithSpriteFrames(animFrames, 0.016f);
	action = runAction(Animate::create(currentAnimation));
	action->retain();
	std::string sound = Tower::getConfig()[owner]["sound_bullet"].asString();
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(sound.c_str(),false);
}

void Bullet::setOwner(std::string nowner){
	owner = nowner;
}
