#include "TutorialFactory.h"


Tutorial * TutorialFactory::createTutorial(TutorialType tutorialType, MyGame* game)
{
	Tutorial* tutorial = nullptr;
	switch (tutorialType) {
	case DANGOBESE:
		tutorial = new DangobeseTutorial(game->getMenu(), game->getLevel());
		break;
	case DANGORILLA:
		tutorial = new DangorillaTutorial(game->getMenu(), game->getLevel());
		break;
	case MULTIPATHS:
		tutorial = new MultiPathsTutorial(game->getMenu(), game->getLevel());
		break;
	case LIFE:
		tutorial = new LifeTutorial(game->getMenu());
		break;
	case SUGAR:
		tutorial = new SugarTutorial(game->getMenu());
		break;
	case SAUCER:
		tutorial = new SaucerTutorial(game->getMenu(), game->getLevel());
		break;
	case TOWERPOSITIONING:
		tutorial = new TowerPositioningTutorial(game->getMenu());
		break;
	case UPGRADE:
		tutorial = new UpgradeTutorial(game->getMenu(), game->getLevel());
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
