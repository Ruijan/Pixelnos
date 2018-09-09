#include "SaucerTutorial.h"
#include "../Level.h"
#include "../Interface/LevelInterface.h"
#include "../../Config/Config.h"

SaucerTutorial::SaucerTutorial(Config* config, LevelInterface* levelInterface, Level* level) :
	DialogueTutorial(config->getGameTutorialSettings()),
	levelInterface(levelInterface),
	level(level),
	config(config)
{
}

SaucerTutorial::~SaucerTutorial() {
	if (running) {
		levelInterface->removeChild(dialogues);
		levelInterface->removeChildByName("hand");
	}
}

void SaucerTutorial::update(float dt)
{
	if (!isDone() && areConditionsMet()) {
		if (!running) {
			startDialogues();
			running = true;
		}
		else if (dialogues != nullptr) {
			dialogues->update();
			if (dialogues->hasFinished()) {
				levelInterface->removeChild(dialogues);
				dialogues = nullptr;
				showTower();
			}
		}
		else if (running && !isDone() && isLastTowerCreatedASaucer()) {
			endTutorial();
		}
	}
}

bool SaucerTutorial::areConditionsMet() {
	return level->getLevelId() == settings->getSettingsMap()["saucer"]["level"].asInt() &&
		level->getWorldId() == settings->getSettingsMap()["saucer"]["world"].asInt() &&
		levelInterface->getGameState() == LevelInterface::GameState::TITLE;
}

bool SaucerTutorial::isDone() {
	return settings->isTutorialComplete("saucer");
}

void SaucerTutorial::startDialogues() {
	running = true;
	levelInterface->pauseLevel();
	dialogues = Dialogue::createFromConfig(settings->getSettingsMap()["saucer"]["dialogue"]);
	levelInterface->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	levelInterface->hideStartMenu();
	levelInterface->lockStartMenu();
}

void SaucerTutorial::endTutorial() {
	settings->completeTutorial("saucer");
	levelInterface->removeChildByName("hand");
	levelInterface->resetTowerMenu();
	levelInterface->resumeLevel();
	levelInterface->unlockStartMenu();
	levelInterface->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
	running = false;
}

void SaucerTutorial::showTower()
{
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	shakeElement(levelInterface->getTowerFromMenu("saucer"), false);
	cocos2d::Sprite* hand = cocos2d::Sprite::create("res/buttons/hand.png");
	hand->setAnchorPoint(cocos2d::Vec2(0.15f, 0.5f));
	hand->setScale(visibleSize.width / 10 / hand->getContentSize().width);
	hand->setPosition(levelInterface->getAbsoluteMenuTowerPosition("saucer"));
	levelInterface->addChild(hand, 3, "hand");
	hand->setOpacity(0.f);
	hand->runAction(cocos2d::RepeatForever::create(cocos2d::Sequence::create(
		cocos2d::DelayTime::create(1.f),
		cocos2d::FadeIn::create(0.5f),
		cocos2d::DelayTime::create(0.5f),
		cocos2d::Spawn::createWithTwoActions(
			cocos2d::MoveBy::create(1.5f, cocos2d::Vec2(-visibleSize.width / 2, 0)),
			cocos2d::EaseBackOut::create(cocos2d::MoveBy::create(1.5f, cocos2d::Vec2(0, -visibleSize.height / 3)))),
		cocos2d::DelayTime::create(0.5f),
		cocos2d::FadeOut::create(0.5f),
		cocos2d::MoveTo::create(0.f, levelInterface->getAbsoluteMenuTowerPosition("saucer")),
		nullptr))
	);
}



bool SaucerTutorial::isLastTowerCreatedASaucer() {
	return config->getLastLevelAction()["action"] == "create_tower" &&
		config->getLastLevelAction()["tower_name"] == "bomber";
}


