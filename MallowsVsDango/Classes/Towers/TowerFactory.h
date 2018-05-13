#pragma once
#include "Tower.h"


class TowerFactory {
public:
	static Tower* createTower(Tower::TowerType towerType);
};