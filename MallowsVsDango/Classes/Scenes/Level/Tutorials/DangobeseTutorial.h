#pragma once
#include "DialogueTutorial.h"


class LevelInterface;
class Level;
class DangobeseTutorial : public DialogueTutorial {
public:
	DangobeseTutorial(TutorialSettings* settings, LevelInterface* levelInterface, Level* level, GUISettings* guiSettings);
	bool isDone();
	bool areConditionsMet();
	virtual ~DangobeseTutorial ();

protected:
	void startDialogues();
	void endTutorial();

protected:
	LevelInterface* levelInterface;
	Level* level;
};
