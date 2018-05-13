#include "TowerFactory.h"
#include "Bomber.h"
#include "Cutter.h"
#include "Saucer.h"

Tower* TowerFactory::createTower(Tower::TowerType towerType)
{
	Tower* createrTower = nullptr;
	if (towerType == Tower::TowerType::BOMBER) {
		createrTower = Bomber::create();
	}
	else if (towerType == Tower::TowerType::CUTTER) {
		createrTower = Cutter::create();
	}
	else if (towerType == Tower::TowerType::SAUCER) {
		createrTower = Saucer::create();
	}
	return createrTower;
}
