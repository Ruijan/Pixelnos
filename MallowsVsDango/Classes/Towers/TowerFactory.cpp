#include "TowerFactory.h"
#include "Bomber.h"
#include "Cutter.h"
#include "Saucer.h"

Tower* TowerFactory::createTower(Tower::TowerType towerType, Config* config)
{
	Tower* createrTower = nullptr;
	if (towerType == Tower::TowerType::BOMBER) {
		createrTower = Bomber::create(config);
	}
	else if (towerType == Tower::TowerType::CUTTER) {
		createrTower = Cutter::create(config);
	}
	else if (towerType == Tower::TowerType::SAUCER) {
		createrTower = Saucer::create(config);
	}
	return createrTower;
}

std::string TowerFactory::getTowerNameFromType(Tower::TowerType type)
{
	if (type == Tower::TowerType::BOMBER) {
		return "bomber";
	}
	else if (type == Tower::TowerType::CUTTER) {
		return "cutter";
	}
	else if (type == Tower::TowerType::SAUCER) {
		return "saucer";
	}
}
