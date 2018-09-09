#pragma once
#include "DialogueTutorial.h"


class LevelInterface;
class Level;
class Tower;
class UpgradeTutorial : public DialogueTutorial {
public:
	UpgradeTutorial(TutorialSettings* settings, LevelInterface* levelInterface, Level* level);
	virtual void update(float dt);
	bool isDone();
	virtual ~UpgradeTutorial();

protected:
	void startDialogues();
	void showHand();
	void endTutorial();
	bool areConditionsMet();

protected:
	Tower* selectedTower;
	LevelInterface* levelInterface;
	Level* level;
};