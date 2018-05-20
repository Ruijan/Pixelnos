#pragma once

#include "DangobeseTutorial.h"
#include "DangorillaTutorial.h"
#include "LifeTutorial.h"
#include "MultiPathsTutorial.h"
#include "SaucerTutorial.h"
#include "SugarTutorial.h"
#include "TowerPositioningTutorial.h"
#include "UpgradeTutorial.h"
#include "../../Scenes/MyGame.h"

enum TutorialType {
	DANGOBESE,
	DANGORILLA,
	LIFE,
	MULTIPATHS,
	SAUCER,
	SUGAR,
	TOWERPOSITIONING,
	UPGRADE
};

class TutorialFactory {
public:
	static Tutorial* createTutorial(TutorialType tutorialType, MyGame* game);
	static TutorialType getTutorialTypeFromString(std::string tutorialName);
};