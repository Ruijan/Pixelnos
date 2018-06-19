#pragma once
#include "DialogueTutorial.h"


class InterfaceGame;
class Level;
class DangobeseTutorial : public DialogueTutorial {
public:
	DangobeseTutorial(TutorialSettings* settings, InterfaceGame* interfaceGame, Level* level);
	bool isDone();
	bool areConditionsMet();
	virtual ~DangobeseTutorial ();

protected:
	void startDialogues();
	void endTutorial();

protected:
	InterfaceGame* interfaceGame;
	Level* level;
};
