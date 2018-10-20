#include "TutorialFactory.h"
#include "../../Config/Config.h"

Tutorial * TutorialFactory::createTutorial(TutorialType tutorialType, Config* config, LevelInterface* menu, Level* level)
{
	Tutorial* tutorial = nullptr;
	switch (tutorialType) {
	case DANGOBESE:
		tutorial = new DangobeseTutorial(config->getGameTutorialSettings(), menu, level);
		break;
	case DANGORILLA:
		tutorial = new DangorillaTutorial(config->getGameTutorialSettings(), menu, level);
		break;
	case MULTIPATHS:
		tutorial = new MultiPathsTutorial(config->getGameTutorialSettings(), menu, level);
		break;
	case LIFE:
		tutorial = new LifeTutorial(config->getGameTutorialSettings(), menu);
		break;
	case SUGAR:
		tutorial = new SugarTutorial(config->getGameTutorialSettings(), menu);
		break;
	case SAUCER:
		tutorial = new SaucerTutorial(config, menu, level);
		break;
	case TOWERPOSITIONING:
		tutorial = new TowerPositioningTutorial(config, menu);
		break;
	case UPGRADE:
		tutorial = new UpgradeTutorial(config->getGameTutorialSettings(), menu, level);
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
