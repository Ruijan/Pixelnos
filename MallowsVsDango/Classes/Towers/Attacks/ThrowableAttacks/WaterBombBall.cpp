#include "WaterBombBall.h"
#include "../../../SceneManager.h"

WaterBombBall::WaterBombBall(Dango* ntarget, double ndamages, double nspeed, double nrange) :
	WaterBall(ntarget, ndamages, nspeed), range(nrange) {
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

bool WaterBombBall::affectEnemy(Dangobese* enemy) {
	return false;
}

void WaterBombBall::applyDamagesToTarget()
{
	std::vector<Dango*> enemies = SceneManager::getInstance()->getGame()->
		getLevel()->getEnemiesInRange(target->getPosition(), range);
	target->removeTargetingAttack(this);
	for (auto& enemy : enemies) {
		bool is_enemy_alive = enemy->isDying();

		if (enemy == target) {
			enemy->applyProspectiveDamages(damages_id);
		}
		else {
			enemy->takeDamages(0.5*damages);
		}
		if (enemy->isDying() && is_enemy_alive) {
			rewardedXP += target->getXP();
		}
	}
	target = nullptr;
}
