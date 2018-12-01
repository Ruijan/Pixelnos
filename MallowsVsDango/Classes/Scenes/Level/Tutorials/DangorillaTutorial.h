#pragma once
#include "DialogueTutorial.h"
#include "../Level.h"

class LevelInterface;
class Level;
class DangorillaTutorial : public DialogueTutorial {
public:
	DangorillaTutorial(TutorialSettings* settings, LevelInterface* levelInterface, Level* level, GUISettings* guiSettings);
	bool isDone();
	bool areConditionsMet();
	virtual ~DangorillaTutorial();


protected:
	void startDialogues();
	void endTutorial();

protected:
	LevelInterface* levelInterface;
	Level* level;
};
