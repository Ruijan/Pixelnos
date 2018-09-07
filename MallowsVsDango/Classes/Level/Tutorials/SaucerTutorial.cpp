#include "SaucerTutorial.h"
#include "../Level.h"
#include "../InterfaceGame.h"
#include "../../Config/Config.h"

SaucerTutorial::SaucerTutorial(Config* config, InterfaceGame* interfaceGame, Level* level) :
	DialogueTutorial(config->getGameTutorialSettings()),
	interfaceGame(interfaceGame),
	level(level),
	config(config)
{
}

SaucerTutorial::~SaucerTutorial() {
	if (running) {
		interfaceGame->removeChild(dialogues);
		interfaceGame->removeChildByName("hand");
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
				interfaceGame->removeChild(dialogues);
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
		interfaceGame->getGameState() == InterfaceGame::GameState::TITLE;
}

bool SaucerTutorial::isDone() {
	return settings->isTutorialComplete("saucer");
}

void SaucerTutorial::startDialogues() {
	running = true;
	interfaceGame->pauseLevel();
	dialogues = Dialogue::createFromConfig(settings->getSettingsMap()["saucer"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	interfaceGame->hideStartMenu();
	interfaceGame->lockStartMenu();
}

void SaucerTutorial::endTutorial() {
	settings->completeTutorial("saucer");
	interfaceGame->removeChildByName("hand");
	interfaceGame->resetTowerMenu();
	interfaceGame->resumeLevel();
	interfaceGame->unlockStartMenu();
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
	running = false;
}

void SaucerTutorial::showTower()
{
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	shakeElement(interfaceGame->getMenuTower("saucer"), false);
	cocos2d::Sprite* hand = cocos2d::Sprite::create("res/buttons/hand.png");
	hand->setAnchorPoint(cocos2d::Vec2(0.15f, 0.5f));
	hand->setScale(visibleSize.width / 10 / hand->getContentSize().width);
	hand->setPosition(interfaceGame->getAbsoluteMenuTowerPosition("saucer"));
	interfaceGame->addChild(hand, 3, "hand");
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
		cocos2d::MoveTo::create(0.f, interfaceGame->getAbsoluteMenuTowerPosition("saucer")),
		nullptr))
	);
}



bool SaucerTutorial::isLastTowerCreatedASaucer() {
	return config->getLastLevelAction()["action"] == "create_tower" &&
		config->getLastLevelAction()["tower_name"] == "bomber";
}


