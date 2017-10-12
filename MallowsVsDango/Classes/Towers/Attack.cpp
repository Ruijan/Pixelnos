#include "Attack.h"
#include "../Dangos/Dango.h"
#include "../Towers/Tower.h"
#include "../Lib/Functions.h"
#include "../AppDelegate.h"
#include "../Dangos/Dangobese.h"

USING_NS_CC;


Attack::Attack(Dango* ntarget, double ndamages, std::string njsontype): target(ntarget), 
	damages(ndamages), touched(false), action(nullptr), hasToBeDeleted(false), 
	enabled(true), jsontype(njsontype), previous_pos(Vec2(0,0)), previous_target_pos(Vec2(0, 0)) {
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

void Attack::setDamagesId(int id) {
	damages_id = id;
}

void Attack::setEnabled(bool enable) {
	enabled = enable;
	if (target != nullptr && !enable) {
		target->removePDamages(damages_id);
		target = nullptr;
	}
}

void Attack::setHasToBeDeleted(bool deleted) {
	hasToBeDeleted = deleted;
}

std::string Attack::getType() {
	return jsontype;
}

/*
SUCKABLE
*/
Suckable::Suckable(): force_applied(false) {

}

bool Suckable::isForceApplied() {
	return force_applied;
}

void Suckable::setForceApplied(bool applied) {
	force_applied = applied;
}

void Suckable::update() {
	force_applied = false;
}
/*
WATERBALL
*/

WaterBall::WaterBall(Dango* ntarget, double ndamages, double nspeed): 
	Attack(ntarget,ndamages, "bomber"), speed(nspeed), Suckable(){

}

WaterBall::~WaterBall() {

}

WaterBall* WaterBall::create(Dango* ntarget, double damages, double nspeed) {
	WaterBall* pSprite = new WaterBall(ntarget, damages, nspeed);

	if (pSprite->initWithFile("res/turret/yellow_bomb.png"))
	{
		pSprite->autorelease();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

WaterBall* WaterBall::createWithFile(std::string file, Dango* ntarget, double damages, double nspeed) {
	WaterBall* pSprite = new WaterBall(ntarget, damages, nspeed);

	if (pSprite->initWithFile(file))
	{
		pSprite->autorelease();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

void WaterBall::update(float dt) {
	Suckable::update();
	if (enabled) {
		if (target != nullptr) {
			if (!touched) {
				// check if bullet has touched the enemy during the time interval
				Vec2 direction = target->getPosition() - getPosition();
				Vec2 prev_direction = previous_target_pos - previous_pos;
				bool has_touched = false;
				if (prev_direction != Vec2(0, 0) && previous_target_pos != Vec2(0, 0)) {
					if ((prev_direction.x < 0 && direction.x > 0) || (prev_direction.x > 0 && direction.x < 0) ||
						(prev_direction.y < 0 && direction.y > 0) || (prev_direction.y > 0 && direction.y < 0)) {
						has_touched = true;
					}
				}
				double distance = sqrt(direction.x*direction.x + direction.y*direction.y);
				Size visibleSize = Director::getInstance()->getVisibleSize();
				if (distance < 10 * visibleSize.width / 960) {
					has_touched = true;
				}

				// if yes then apply damages/effects
				if (has_touched) {
					bool is_enemy_alive = target->isDying();

					touched = true;
					target->removeTargetingAttack(this);
					target->applyProspectiveDamages(damages_id);
					startAnimation();
					if (target->isDying() && !is_enemy_alive) {
						SceneManager::getInstance()->getGame()->getLevel()->incrementXPTower(jsontype,
							target->getXP());
					}
					target = nullptr;
				}
				else {
					direction.normalize();
					previous_pos = getPosition();
					previous_target_pos = target->getPosition();
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

/*
WaterBombBall
*/
WaterBombBall::WaterBombBall(Dango* ntarget, double ndamages, double nspeed, double nrange):
	WaterBall(ntarget, ndamages, nspeed), range(nrange){
	jsontype = "bomber";
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
	Suckable::update();
	if (enabled) {
		if (target != nullptr) {
			if (!touched) {
				// check if bullet has touched the enemy during the time interval
				Vec2 direction = target->getPosition() - getPosition();
				Vec2 prev_direction = previous_target_pos - previous_pos;
				bool has_touched = false;
				if (prev_direction != Vec2(0, 0) && previous_target_pos != Vec2(0, 0)) {
					if ((prev_direction.x < 0 && direction.x > 0) || (prev_direction.x > 0 && direction.x < 0) ||
						(prev_direction.y < 0 && direction.y > 0) || (prev_direction.y > 0 && direction.y < 0)) {
						has_touched = true;
					}
				}
				double distance = sqrt(direction.x*direction.x + direction.y*direction.y);
				Size visibleSize = Director::getInstance()->getVisibleSize();
				if (distance < 10 * visibleSize.width / 960) {
					has_touched = true;
				}

				// if yes then apply damages/effects
				if (has_touched) {
					touched = true;
					std::vector<Dango*> enemies = SceneManager::getInstance()->getGame()->
						getLevel()->getEnemiesInRange(target->getPosition(), range);
					target->removeTargetingAttack(this);
					for (auto& enemy : enemies) {
						bool is_enemy_alive = enemy->isDying();

						if (enemy == target) {
							enemy->applyProspectiveDamages(damages_id);
						}
						else{
							enemy->takeDamages(0.5*damages);
						}
						if (enemy->isDying() && is_enemy_alive) {
							SceneManager::getInstance()->getGame()->getLevel()->incrementXPTower(jsontype,
								enemy->getXP());
						}
					}
					
					target = nullptr;
					startAnimation();
				}
				else {
					direction.normalize();
					previous_pos = getPosition();
					previous_target_pos = target->getPosition();
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
ChocoSpit
*/

ChocoSpit::ChocoSpit(Dango* ntarget, double damages, double slow, double duration, double nspeed):
	Attack(ntarget, damages, "saucer"), speed(nspeed), Suckable(), slow_percent(slow),
	slow_duration(duration){
}

ChocoSpit::~ChocoSpit() {

}

ChocoSpit* ChocoSpit::create(Dango* ntarget, double damages, double slow, double slow_duration, double nspeed) {
	ChocoSpit* pSprite = new ChocoSpit(ntarget, damages, slow, slow_duration, nspeed);

	if (pSprite->initWithFile("res/turret/choco_spit.png"))
	{
		pSprite->autorelease();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

ChocoSpit* ChocoSpit::createWithFile(std::string file, Dango* ntarget, double damages, double slow, double slow_duration, double nspeed) {
	ChocoSpit* pSprite = new ChocoSpit(ntarget, damages, slow, slow_duration, nspeed);

	if (pSprite->initWithFile(file))
	{
		pSprite->autorelease();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

void ChocoSpit::update(float dt) {
	Suckable::update();
	if (enabled) {
		if (target != nullptr) {
			if (!touched) {
				// check if bullet has touched the enemy during the time interval
				Vec2 direction = target->getPosition() - getPosition();
				Vec2 prev_direction = previous_target_pos - previous_pos;
				bool has_touched = false;
				if (prev_direction != Vec2(0, 0) && previous_target_pos != Vec2(0, 0)) {
					if ((prev_direction.x < 0 && direction.x > 0) || (prev_direction.x > 0 && direction.x < 0) ||
						(prev_direction.y < 0 && direction.y > 0) || (prev_direction.y > 0 && direction.y < 0)) {
						has_touched = true;
					}
				}
				double distance = sqrt(direction.x*direction.x + direction.y*direction.y);
				Size visibleSize = Director::getInstance()->getVisibleSize();
				if (distance < 10 * visibleSize.width / 960) {
					has_touched = true;
				}

				// if yes then apply damages/effects
				if (has_touched) {
					bool is_enemy_alive = target->isDying();

					touched = true;
					target->applyProspectiveDamages(damages_id);
					target->removeTargetingAttack(this);
					Slow* effect = Slow::create(target, slow_duration, slow_percent, "choco");
					target->addEffect(effect);
					effect->applyModifierToTarget();
					startAnimation();
					if (target->isDying() && !is_enemy_alive) {
						SceneManager::getInstance()->getGame()->getLevel()->incrementXPTower(jsontype,
							target->getXP());
					}
					target = nullptr;
				}
				else {
					direction.normalize();
					previous_pos = getPosition();
					previous_target_pos = target->getPosition();
					setPosition(getPosition() + direction * speed * dt);
					//float rot_angle = 180 * direction.getAngle() / M_PI;
					setRotation(-180 * direction.getAngle() / M_PI);
				}
			}
		}
		else {
			hasToBeDeleted = true;
		}
	}
}

bool ChocoSpit::affectEnemy(Dangobese* enemy) {
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
				bool is_enemy_alive = target->isAlive();

				touched = true;
				target->removeTargetingAttack(this);
				target->applyProspectiveDamages(damages_id);
				if (target->isDying() && !is_enemy_alive) {
					SceneManager::getInstance()->getGame()->getLevel()->incrementXPTower(jsontype,
						target->getXP());
				}
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
				bool is_enemy_alive = target->isAlive();

				touched = true;
				target->removeTargetingAttack(this);
				target->applyProspectiveDamages(damages_id);
				target->addEffect(effect);
				effect->applyModifierToTarget();
				if (target->isDying() && !is_enemy_alive) {
					SceneManager::getInstance()->getGame()->getLevel()->incrementXPTower(jsontype,
						target->getXP());
				}
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