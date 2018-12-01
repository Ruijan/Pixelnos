#pragma once
#include "DialogueTutorial.h"

class LevelInterface;
class LifeTutorial : public DialogueTutorial {
public:
	LifeTutorial(TutorialSettings* settings, LevelInterface* levelInterface, GUISettings* guiSettings);
	bool isDone();
	virtual ~LifeTutorial();


protected:
	void startDialogues();
	void endTutorial();
	bool areConditionsMet();

protected:
	LevelInterface* levelInterface;
};