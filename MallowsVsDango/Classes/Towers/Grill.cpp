#include "Grill.h"
#include "../Dangos/Dango.h"
#include "../Level/Cell.h"

Grill::Grill() : Tower(Grill::getConfig()["attack_speed"].asDouble(), Grill::getConfig()["damages"].asDouble(), 
	Grill::getConfig()["range"].asDouble(), Grill::getConfig()["cost"].asDouble()){

}

Grill* Grill::create()
{
	Grill* pSprite = new Grill();

	if (pSprite->initWithFile(Grill::getConfig()["image"].asString()))
	{
		pSprite->initDebug();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

Json::Value Grill::getConfig(){
	return Tower::getConfig()["grill"];
}

Json::Value Grill::getSpecConfig(){
	return Tower::getConfig()["grill"];
}

void Grill::chooseTarget(std::vector<Dango*> targets){
	otherTargets.clear();
	for (auto cTarget : targets){
		double dist = cTarget->getPosition().distanceSquared(this->getPosition());
		double minDist = pow(getRange() + sqrt((pow(Cell::getCellWidth() * 3 / 8, 2) +
			pow(Cell::getCellHeight() * 3 / 8, 2))), 2);
		if (dist < minDist){
			otherTargets.push_back(cTarget);
		}
	}
}

void Grill::attack(){
	if (timer > attackSpeed){
		for (auto& cTarget : otherTargets){
			if (cTarget != nullptr){
				timer = 0;
				cTarget->takeDamages(damage);
			}
		}
	}
	else{
		getChildByName("range")->setRotation(timer / attackSpeed * 360);
	}
}
