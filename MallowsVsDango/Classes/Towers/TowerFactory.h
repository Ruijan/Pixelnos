#pragma once
#include "Tower.h"
#include "../Config/Config.h"

class TowerFactory {
public:
	static Tower* createTower(Tower::TowerType towerType, Config* config);
	static std::string getTowerNameFromType(Tower::TowerType type);
	static Tower::TowerType getTowerTypeFromString(std::string type);
	static std::vector<Tower::TowerType> getAllTowerTypes();
};