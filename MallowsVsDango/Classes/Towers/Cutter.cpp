#include "Cutter.h"
#include "../Dangos/Dango.h"
#include "../Towers/Bullet.h"
#include "../Level/Cell.h"
#include "../AppDelegate.h"

USING_NS_CC;

Cutter::Cutter() : Tower(Cutter::getConfig()["attack_speed"].asDouble(), Cutter::getConfig()["damages"].asDouble(), 
	Cutter::getConfig()["range"].asDouble(), Cutter::getConfig()["cost"].asDouble(),
	Cutter::getConfig()["d_damage"].asDouble(),Cutter::getConfig()["d_range"].asDouble(),
	Cutter::getConfig()["d_speed"].asDouble()){

}

Cutter* Cutter::create()
{
	Cutter* pSprite = new Cutter();

	if (pSprite->initWithFile(Cutter::getConfig()["image"].asString()))
	{
		pSprite->initDebug();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

Json::Value Cutter::getConfig(){
	return Tower::getConfig()["cutter"];
}

Json::Value Cutter::getSpecConfig(){
	return Tower::getConfig()["cutter"];
}

void Cutter::chooseTarget(std::vector<Dango*> targets){
	otherTargets.clear();
	for(auto cTarget : targets){
		if(cTarget != nullptr){
			int first = cTarget->getTargetedCell();
			double dist = cTarget->getPosition().distanceSquared(this->getPosition());
			double minDist = pow(getRange() + sqrt((pow(Cell::getCellWidth() * 3 / 8.0, 2) +
				pow(Cell::getCellHeight() * 3 / 8.0, 2))), 2);
			if (dist < minDist){
				otherTargets.push_back(cTarget);
			}
		}
	}
	if(otherTargets.size() > 0){
		target = otherTargets[0]; 
	}
	else{
		target = nullptr;
	}
}

void Cutter::attack(){
	for (auto& cTarget : otherTargets){
		if (cTarget != nullptr){
			Bullet* bullet = Bullet::create("res/turret/bullet.png", cTarget, damage,500,true);
			bullet->setPosition(cTarget->getPosition());
			bullet->setVisible(false);
			SceneManager::getInstance()->getGame()->getLevel()->addBullet(bullet);
			//cTarget->takePDamages(damage);
		}
	}
}
