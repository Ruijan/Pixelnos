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
	if(state != State::ATTACKING && state != State::RELOADING){
		otherTargets.clear();
		for(auto cTarget : targets){
			if(cTarget != nullptr){
				int first = cTarget->getTargetedCell();
				double dist = cTarget->getPosition().distanceSquared(this->getPosition());
				double minDist = pow(getRange() , 2);
				if (dist <= minDist && cTarget->willBeAlive()){
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
	else if(state == State::RELOADING){
			target = nullptr;
		}
}

void Cutter::givePDamages(double damage){
	for (auto& cTarget : otherTargets){
		cTarget->takePDamages(damage);
	}
}

void Cutter::attack(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	for (auto& cTarget : otherTargets){
		if (cTarget != nullptr){
			Bullet* bullet = Bullet::create("res/turret/bullet.png", cTarget, damage,500,false);
			bullet->setOwner("cutter");
			bullet->setPosition(cTarget->getPosition());
			bullet->setScale(visibleSize.width/960);
			bullet->setVisible(false);
			SceneManager::getInstance()->getGame()->getLevel()->addBullet(bullet);
			//cTarget->takePDamages(damage);
		}
	}
}
