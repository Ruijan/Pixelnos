#include "DeepSlash.h"
#include "../../SceneManager.h"

DeepSlash::DeepSlash(Dango* ntarget, double ndamages, double duration, double percent) :
	Slash(ntarget, ndamages) {
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

bool DeepSlash::affectEnemy(Dangobese* enemy) {
	return true;
}

void DeepSlash::applyDamagesOnTarget() {
	target->removeTargetingAttack(this);
	target->applyProspectiveDamages(damages_id);
	target->addEffect(effect);
	effect->applyModifierToTarget();
}