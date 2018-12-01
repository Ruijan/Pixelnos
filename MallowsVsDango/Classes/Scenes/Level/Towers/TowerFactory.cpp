#include "TowerFactory.h"
#include "Bomber.h"
#include "Cutter.h"
#include "Saucer.h"
#include "Exception/TowerTypeException.h"
#include "../Level.h"

Tower* TowerFactory::createTower(Tower::TowerType towerType, Config* config, Level* level)
{
	Tower* createrTower = nullptr;
	if (towerType == Tower::TowerType::BOMBER) {
		createrTower = Bomber::create(config, level);
	}
	else if (towerType == Tower::TowerType::CUTTER) {
		createrTower = Cutter::create(config, level);
	}
	else if (towerType == Tower::TowerType::SAUCER) {
		createrTower = Saucer::create(config, level);
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
	else {
		throw new TowerTypeException("Tower type does not exist");
	}
}

Tower::TowerType TowerFactory::getTowerTypeFromString(std::string type) {
	if (!strcmp(type.c_str(), "bomber")) {
		return Tower::TowerType::BOMBER;
	}
	else if (!strcmp(type.c_str(), "cutter")) {
		return Tower::TowerType::CUTTER;
	}
	else if (!strcmp(type.c_str(), "saucer")) {
		return Tower::TowerType::SAUCER;
	}
	else {
		throw new TowerTypeException("Tower type does not exist");
	}
}

std::vector<Tower::TowerType> TowerFactory::getAllTowerTypes()
{
	return  { Tower::TowerType::BOMBER, Tower::TowerType::CUTTER, Tower::TowerType::SAUCER };
}