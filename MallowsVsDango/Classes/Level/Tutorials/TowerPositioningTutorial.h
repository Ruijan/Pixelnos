#pragma once
#include "Tutorial.h"

class Config;
class LevelInterface;
class TowerPositioningTutorial : public Tutorial {
public:
	TowerPositioningTutorial(Config* config, LevelInterface* levelInterface);
	virtual void update(float dt);
	virtual bool isDone();
	bool isLastTowerCreatedABomber();
	virtual ~TowerPositioningTutorial();
	virtual void endTutorial();

private:
	LevelInterface* levelInterface;
	Config* config;
};