#include "DialogueTutorial.h"

DialogueTutorial::DialogueTutorial(Config * config):
	Tutorial(config),
	dialogues(nullptr)
{
}

void DialogueTutorial::update(float dt)
{
	if (!isDone() && areConditionsMet()) {
		if (!running) {
			startDialogues();
			running = true;
		}
		else {
			dialogues->update();
			if (dialogues->hasFinished()) {
				endTutorial();
			}
		}
	}
}
