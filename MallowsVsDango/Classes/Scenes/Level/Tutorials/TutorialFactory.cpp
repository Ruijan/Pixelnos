#include "TutorialFactory.h"
#include "../../../Config/Config.h"

Tutorial * TutorialFactory::createTutorial(TutorialType tutorialType, Config* config, LevelInterface* menu, Level* level)
{
	Tutorial* tutorial = nullptr;
	TutorialSettings* settings = config->getGameTutorialSettings();
	GUISettings* guiSettings = config->getGUISettings();
	switch (tutorialType) {
	case DANGOBESE:
		tutorial = new DangobeseTutorial(settings, menu, level, guiSettings);
		break;
	case DANGORILLA:
		tutorial = new DangorillaTutorial(settings, menu, level, guiSettings);
		break;
	case MULTIPATHS:
		tutorial = new MultiPathsTutorial(settings, menu, level, guiSettings);
		break;
	case LIFE:
		tutorial = new LifeTutorial(settings, menu, guiSettings);
		break;
	case SUGAR:
		tutorial = new SugarTutorial(settings, menu, guiSettings);
		break;
	case SAUCER:
		tutorial = new SaucerTutorial(config, menu, level, guiSettings);
		break;
	case TOWERPOSITIONING:
		tutorial = new TowerPositioningTutorial(config, menu);
		break;
	case UPGRADE:
		tutorial = new UpgradeTutorial(settings, menu, level, guiSettings);
		break;
	}
	if (tutorial == nullptr) {
		throw std::invalid_argument("Type of tutorial not handled");
	}
	return tutorial;
}

TutorialType TutorialFactory::getTutorialTypeFromString(const std::string& tutorialName)
{
	if (tutorialName == "dangobese") {
		return DANGOBESE;
	}
	else if (tutorialName == "dangorilla") {
		return DANGORILLA;
	}
	else if (tutorialName == "life") {
		return LIFE;
	}
	else if (tutorialName == "multi_paths") {
		return MULTIPATHS;
	}
	else if (tutorialName == "saucer") {
		return SAUCER;
	}
	else if (tutorialName == "sugar") {
		return SUGAR;
	}
	else if (tutorialName == "tower_positioning") {
		return TOWERPOSITIONING;
	}
	else if (tutorialName == "upgrade") {
		return UPGRADE;
	}
	else {
		throw std::invalid_argument("Unknown tutorial name : " + tutorialName);
	}
}
