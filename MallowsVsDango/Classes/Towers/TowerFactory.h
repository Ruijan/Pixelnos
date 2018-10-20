#pragma once
#include "Tower.h"
#include "../Config/Config.h"

class Level;

class TowerFactory {
public:
	static Tower* createTower(Tower::TowerType towerType, Config* config, Level* level);
	static std::string getTowerNameFromType(Tower::TowerType type);
	static Tower::TowerType getTowerTypeFromString(std::string type);
	static std::vector<Tower::TowerType> getAllTowerTypes();
};