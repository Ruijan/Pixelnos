#include "DialogueTutorial.h"

DialogueTutorial::DialogueTutorial(TutorialSettings * settings):
	Tutorial(settings),
	dialogues(nullptr)
{
}

DialogueTutorial::~DialogueTutorial() {
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
