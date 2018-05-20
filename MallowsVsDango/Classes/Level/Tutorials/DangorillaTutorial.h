#pragma once
#include "DialogueTutorial.h"
#include "../Dialogue.h"
#include "../Level.h"

class InterfaceGame;
class DangorillaTutorial : public DialogueTutorial {
public:
	DangorillaTutorial(InterfaceGame* interfaceGame, Level* level);
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
