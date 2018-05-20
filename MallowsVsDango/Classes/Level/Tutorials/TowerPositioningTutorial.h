#pragma once
#include "Tutorial.h"


class TowerPositioningTutorial : public Tutorial {
public:
	TowerPositioningTutorial(InterfaceGame* interfaceGame);
	virtual void update(float dt);
	virtual bool isDone();
	bool isLastTowerCreatedABomber();
	virtual ~TowerPositioningTutorial();
	virtual void endTutorial();

private:
	InterfaceGame* interfaceGame;
};