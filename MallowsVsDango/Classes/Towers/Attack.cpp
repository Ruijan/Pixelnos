#include "Attack.h"
#include "../Dangos/Dango.h"
#include "../Towers/Tower.h"
#include "../Lib/Functions.h"
#include "../AppDelegate.h"
#include "../Dangos/Dangobese.h"

USING_NS_CC;


Attack::Attack(Dango* ntarget, double ndamages, std::string njsontype): target(ntarget), 
	damages(ndamages), touched(false), action(nullptr), hasToBeDeleted(false), 
	enabled(true), jsontype(njsontype){
}

Attack::~Attack(){
	if (target != nullptr) {
		target->removeTargetingAttack(this);
	}
}

void Attack::removeTarget(Dango* dango) {
	if (target == dango) {
		target = nullptr;
		hasToBeDeleted = true;
	}
}

bool Attack::hasTouched(){
	return touched;
}

bool Attack::isDone(){
	if(touched){
		return action->isDone();
	}
	else{
		return hasToBeDeleted;
	}
}

Dango* Attack::getTarget(){
	return target;
}

void Attack::setTarget(Dango* dango){
	target = dango;
}

void Attack::startAnimation(){
	setVisible(true);
	unsigned int animation_size = Tower::getConfig()[getType()]["animation_bullet_size"].asInt();
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cocos2d::Vector<SpriteFrame*> animFrames;

	char str[100] = { 0 };
	std::vector<std::string> elements;
	std::string animation = split(Tower::getConfig()[getType()]["animation_bullet"].asString(),'/',elements).back();
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
	std::string sound = Tower::getConfig()[getType()]["sound_bullet"].asString();
	((AppDelegate*)Application::getInstance())->getAudioController()->playEffect(sound.c_str());
}

void Attack::setEnabled(bool enable) {
	enabled = enable;
	if (target != nullptr) {
		target->removePDamages(damages);
	}
}

void Attack::setHasToBeDeleted(bool deleted) {
	hasToBeDeleted = deleted;
}

std::string Attack::getType() {
	return jsontype;
}



/*
WATERBALL
*/

WaterBall::WaterBall(Dango* ntarget, double ndamages, double nspeed): 
	Attack(ntarget,ndamages, "archer"), speed(nspeed) {

}

WaterBall::~WaterBall() {

}

WaterBall* WaterBall::create(Dango* ntarget, double damages, double nspeed) {
	WaterBall* pSprite = new WaterBall(ntarget, damages, nspeed);

	if (pSprite->initWithFile("res/turret/bullet.png"))
	{
		pSprite->autorelease();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

void WaterBall::update(float dt) {
	force_applied = false;
	if (enabled) {
		if (target != nullptr) {
			if (!touched) {
				Vec2 direction = target->getPosition() - getPosition();
				double distance = sqrt(direction.x*direction.x + direction.y*direction.y);
				Size visibleSize = Director::getInstance()->getVisibleSize();
				if (distance < 10 * visibleSize.width / 960) {
					touched = true;
					target->removeTargetingAttack(this);
					target->takeDamages(damages);
					startAnimation();
					target = nullptr;
				}
				else {
					direction.normalize();
					setPosition(getPosition() + direction * speed * dt);
					setRotation(getRotation() + 360 * dt);
				}
			}
		}
		else {
			hasToBeDeleted = true;
		}
	}
}

bool WaterBall::affectEnemy(Dangobese* enemy) {
	return false;
}

bool WaterBall::isForceApplied() {
	return force_applied;
}

void WaterBall::setForceApplied(bool applied) {
	force_applied = applied;
}

/*
SLASH
*/
WaterBombBall::WaterBombBall(Dango* ntarget, double ndamages, double nspeed, double nrange):
	WaterBall(ntarget, ndamages, nspeed), range(nrange){
	jsontype = "archer";
}

WaterBombBall::~WaterBombBall() {

}

WaterBombBall* WaterBombBall::create(Dango* ntarget, double damages, double nspeed, double range) {
	WaterBombBall* pSprite = new WaterBombBall(ntarget, damages, nspeed, range);

	if (pSprite->initWithFile("res/turret/bullet.png"))
	{
		pSprite->autorelease();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

void WaterBombBall::update(float dt) {
	force_applied = false;
	if (enabled) {
		if (target != nullptr) {
			if (!touched) {
				Vec2 direction = target->getPosition() - getPosition();
				double distance = sqrt(direction.x*direction.x + direction.y*direction.y);
				Size visibleSize = Director::getInstance()->getVisibleSize();
				if (distance < 10 * visibleSize.width / 960) {
					touched = true;
					std::vector<Dango*> enemies = SceneManager::getInstance()->getGame()->
						getLevel()->getEnemiesInRange(target->getPosition(), range);
					target->removeTargetingAttack(this);
					for (auto& enemy : enemies) {
						if (enemy == target) {
							enemy->takeDamages(damages);
						}
						else{
							enemy->takeDamages(0.5*damages);
						}
					}
					target = nullptr;
					startAnimation();
				}
				else {
					direction.normalize();
					setPosition(getPosition() + direction * speed * dt);
					setRotation(getRotation() + 360 * dt);
				}
			}
		}
		else {
			hasToBeDeleted = true;
		}
	}
}

bool WaterBombBall::affectEnemy(Dangobese* enemy) {
	return false;
}

/*
SLASH
*/
Slash::Slash(Dango* ntarget, double ndamages) :
	Attack(ntarget, ndamages, "cutter"){

}

Slash::~Slash() {

}

Slash* Slash::create(Dango* ntarget, double damages) {
	Slash* pSprite = new Slash(ntarget, damages);

	if (pSprite->initWithFile("res/turret/bullet.png"))
	{
		pSprite->autorelease();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

void Slash::update(float dt) {
	if (enabled) {
		if (target != nullptr) {
			if (!touched) {
				touched = true;
				target->removeTargetingAttack(this);
				target->takeDamages(damages);
				target = nullptr;
				startAnimation();
			}
		}
		else {
			hasToBeDeleted = true;
		}
	}
}

bool Slash::affectEnemy(Dangobese* enemy) {
	return true;
}

/*
DEEP SLASH
*/
DeepSlash::DeepSlash(Dango* ntarget, double ndamages, double duration, double percent) :
	Slash(ntarget, ndamages){
	effect = DeepWound::create(ntarget, duration, percent);
}

DeepSlash::~DeepSlash() {

}

DeepSlash* DeepSlash::create(Dango* ntarget, double damages, double duration, double percent) {
	DeepSlash* pSprite = new DeepSlash(ntarget, damages, duration, percent);

	if (pSprite->initWithFile("res/turret/bullet.png"))
	{
		pSprite->autorelease();
		return pSprite;
	}
	CC_SAFE_DELETE(pSprite);
	return NULL;
}

void DeepSlash::update(float dt) {
	if (enabled) {
		if (target != nullptr) {
			if (!touched) {
				touched = true;
				target->removeTargetingAttack(this);
				target->takeDamages(damages);
				target->addEffect(effect);
				effect->applyModifierToTarget();
				target = nullptr;
				startAnimation();
			}
		}
		else {
			hasToBeDeleted = true;
		}
	}
}

bool DeepSlash::affectEnemy(Dangobese* enemy) {
	return true;
}