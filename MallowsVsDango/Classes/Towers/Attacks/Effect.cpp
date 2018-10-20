#include "Effect.h"
#include "../../Dangos/Dango.h"
#include "../../Towers/Tower.h"
#include "../../Lib/Functions.h"
#include "../../AppDelegate.h"
#include "../../Dangos/Dangobese.h"

USING_NS_CC;

// EFFECT CLASS
Effect::Effect(Dango* ntarget, float nduration, const std::string& new_name): target(ntarget),
duration(nduration), timer(0), name(new_name){
}

bool Effect::initWithFileName(const std::string& image) {
	return Sprite::initWithFile(image);
}

Effect::~Effect() {}

const std::string& Effect::getAnimationName() {
	return name;
}

void Effect::update(float dt) {
	timer += dt;
}

bool Effect::isDone() {
	return timer > duration;
}

bool Effect::willBeDone(double delay) {
	return timer + delay > duration;
}


// DEEPWOUND CLASS
DeepWound::DeepWound(Dango* ntarget, float nduration, float ndmg_percent, const std::string& animation_name):
Effect(ntarget, nduration, animation_name), dmg_percent(ndmg_percent){
}

DeepWound::~DeepWound() {
	target->removeDamageModifier(id);
}

DeepWound* DeepWound::create(Dango* ntarget, float duration, float dmg_percent, const std::string& animation_name) {
	DeepWound* wound = new DeepWound(ntarget, duration, dmg_percent, animation_name);
	if (wound->Effect::initWithFile("res/turret/deepwound.png")) {
		wound->autorelease();
		return wound;
	}
	CC_SAFE_DELETE(wound);
	return NULL;
}

void DeepWound::update(float dt) {
	Effect::update(dt);
}

float DeepWound::getDmgPercent() {
	return dmg_percent;
}

void DeepWound::applyModifierToTarget() {
	id = target->addDamagesModifier(std::make_pair(dmg_percent,this));
}


// BURN CLASS
Burn::Burn(Dango* ntarget, float nduration, float ndmg, const std::string& animation_name) :
	Effect(ntarget, nduration, animation_name), dmg(ndmg), timer_dmg(0){
}

Burn::~Burn() {}

Burn* Burn::create(Dango* ntarget, float duration, float dmg_percent, const std::string& animation_name) {
	Burn* burn = new Burn(ntarget, duration, dmg_percent, animation_name);
	if (burn->Effect::initWithFile("res/turret/burn.png")) {
		burn->autorelease();

		return burn;
	}
	CC_SAFE_DELETE(burn);
	return NULL;
}

void Burn::update(float dt) {
	Effect::update(dt);
	timer_dmg += dt;
	if (timer_dmg > 1) {
		timer_dmg = 0;
		target->takeDamages(dmg);
	}
}

float Burn::getDamage() {
	return dmg;
}


// SLOW CLASS
Slow::Slow(Dango* ntarget, float nduration, float nslow, const std::string& animation_name) :
	Effect(ntarget, nduration, animation_name), slow_percent(nslow) {
}

Slow::~Slow() {
	if (target != nullptr) {
		target->removeSpeedModifier(id);
	}
}

Slow* Slow::create(Dango* ntarget, float duration, float slow_percent, const std::string& animation_name) {
	Slow* slow = new Slow(ntarget, duration, slow_percent, animation_name);
	if (slow->Effect::initWithFile("res/turret/burn.png")) {
		Size visibleSize = Director::getInstance()->getVisibleSize();

		slow->setScale(0.020 * visibleSize.width / slow->getContentSize().width);
		slow->autorelease();
		return slow;
	}
	CC_SAFE_DELETE(slow);
	return NULL;
}

void Slow::update(float dt) {
	Effect::update(dt);
}

float Slow::getSlow() {
	return slow_percent;
}

void Slow::applyModifierToTarget() {
	id = target->addSpeedModifier(std::make_pair(slow_percent, this));
}