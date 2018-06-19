#pragma once
#include "Tutorial.h"

class Config;
class InterfaceGame;
class TowerPositioningTutorial : public Tutorial {
public:
	TowerPositioningTutorial(Config* config, InterfaceGame* interfaceGame);
	virtual void update(float dt);
	virtual bool isDone();
	bool isLastTowerCreatedABomber();
	virtual ~TowerPositioningTutorial();
	virtual void endTutorial();

private:
	InterfaceGame* interfaceGame;
	Config* config;
};