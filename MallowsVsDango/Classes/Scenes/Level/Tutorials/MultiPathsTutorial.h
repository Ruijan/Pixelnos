#pragma once

#include "DialogueTutorial.h"
#include "../Level.h"
#include "../Interface/LevelInterface.h"

class LevelInterface;
class Level;
class MultiPathsTutorial : public DialogueTutorial {
public:
	MultiPathsTutorial(TutorialSettings* settings, LevelInterface* levelInterface, Level* level, GUISettings* guiSettings);
	bool isDone();
	bool areConditionsMet();
	virtual ~MultiPathsTutorial();


protected:
	void startDialogues();
	void endTutorial();

protected:
	LevelInterface* levelInterface;
	Level* level;
};
