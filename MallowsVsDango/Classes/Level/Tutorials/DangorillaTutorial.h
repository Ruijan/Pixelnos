#pragma once
#include "DialogueTutorial.h"
#include "../Level.h"

class InterfaceGame;
class Level;
class DangorillaTutorial : public DialogueTutorial {
public:
	DangorillaTutorial(TutorialSettings* settings, InterfaceGame* interfaceGame, Level* level);
	bool isDone();
	bool areConditionsMet();
	virtual ~DangorillaTutorial();


protected:
	void startDialogues();
	void endTutorial();

protected:
	InterfaceGame* interfaceGame;
	Level* level;
};
