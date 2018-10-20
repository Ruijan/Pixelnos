#include "ChocoSpit.h"
#include "../../../SceneManager.h"

ChocoSpit::ChocoSpit(Dango* ntarget, double damages, double slow, double duration, double nspeed) :
	WaterBall(ntarget, damages, nspeed), 
	slow_percent(slow), slow_duration(duration) {
	jsontype = "saucer";
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

void ChocoSpit::applyDamagesToTarget()
{
	target->removeTargetingAttack(this);
	target->applyProspectiveDamages(damages_id);
	Slow* effect = Slow::create(target, slow_duration, slow_percent, "choco");
	target->addEffect(effect);
	effect->applyModifierToTarget();
	if (target->isDying() && !target->isDying()) {
		rewardedXP += target->getXP();
		/*SceneManager::getInstance()->getGame()->getLevel()->incrementXPTower(jsontype,
		target->getXP());*/
	}
	target = nullptr;
}

bool ChocoSpit::affectEnemy(Dangobese* enemy) {
	return false;
}
