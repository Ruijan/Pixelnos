#include "Slash.h"
#include "../../SceneManager.h"

Slash::Slash(Dango* ntarget, double ndamages) :
	Attack(ntarget, ndamages, "cutter") {

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

bool Slash::affectEnemy(Dangobese* enemy) {
	return true;
}

void Slash::applyDamagesOnTarget() {
	target->removeTargetingAttack(this);
	target->applyProspectiveDamages(damages_id);
}

void Slash::updateTowerXP()
{
	if (target->isDying() && !target->isAlive()) {
		rewardedXP += target->getXP();
	}
}
