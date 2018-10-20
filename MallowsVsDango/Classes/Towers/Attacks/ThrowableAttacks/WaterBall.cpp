#include "WaterBall.h"

WaterBall::WaterBall(Dango* ntarget, double ndamages, double nspeed) :
	ThrowableAttack(ntarget, ndamages, nspeed, "bomber"){
}

WaterBall::~WaterBall() {}

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

bool WaterBall::affectEnemy(Dangobese* enemy) {
	return false;
}
