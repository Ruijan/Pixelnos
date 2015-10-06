#include "Scorpion.h"

Scorpion::Scorpion() : Tower(Scorpion::getConfig()["attack_speed"].asDouble(), Scorpion::getConfig()["damages"].asDouble(),
	Scorpion::getConfig()["range"].asDouble(), Scorpion::getConfig()["cost"].asDouble()){
}

Scorpion* Scorpion::create()
{
	Scorpion* pSprite = new Scorpion();

	if (pSprite->initWithFile(Scorpion::getConfig()["image"].asString()))
	{
		pSprite->initDebug();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

Json::Value Scorpion::getConfig(){
	return Tower::getConfig()["archer"];
}

Json::Value Scorpion::getSpecConfig(){
	return Tower::getConfig()["archer"];
}