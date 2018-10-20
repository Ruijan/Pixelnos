#include "Attack.h"
#include "../../Dangos/Dango.h"
#include "../../Towers/Tower.h"
#include "../../Lib/Functions.h"
#include "../../AppDelegate.h"
#include "../../Dangos/Dangobese.h"

USING_NS_CC;


Attack::Attack(Dango* ntarget, double ndamages, std::string njsontype): target(ntarget), 
	damages(ndamages), action(nullptr), 
	enabled(true), jsontype(njsontype), rewardedXP(0), state(IDLE){
}

Attack::~Attack(){
	if (target != nullptr) {
		target->removeTargetingAttack(this);
	}
}

void Attack::removeTarget(Dango* dango) {
	if (target == dango) {
		target = nullptr;
		state = DONE;
	}
}

void Attack::setTarget(Dango * dango)
{
	target = dango;
}

bool Attack::isDone(){
	return state == DONE;
}

int Attack::getRewardedXP()
{
	return rewardedXP;
}

void Attack::update(float dt)
{
	switch (state) {
	case IDLE:
		updateIDLE(dt);
		break;
	case REACHED_TARGET:
		if (enabled) {
			applyDamagesToTarget();
			startAnimation();
			target = nullptr;
			state = ANIMATING;
		}
		else {
			state = DONE;
		}
		break;
	case ANIMATING:
		if (action->isDone()) {
			state = DONE;
		}
		break;
	case DONE:
		break;
	default:
		state = IDLE;
		break;
	}
}

void Attack::updateIDLE(float dt) {
	state = REACHED_TARGET;
}

void Attack::applyDamagesToTarget()
{
	target->removeTargetingAttack(this);
	target->applyProspectiveDamages(damages_id);
	if (target->isDying() && !target->isDying()) {
		rewardedXP += target->getXP();
	}
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
	}
}

void Attack::setHasToBeDeleted() {
	state = DONE;
}

std::string Attack::getType() {
	return jsontype;
}