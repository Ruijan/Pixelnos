#include "SaucerTutorial.h"
#include "../InterfaceGame.h"
#include "../../AppDelegate.h"

SaucerTutorial::SaucerTutorial(InterfaceGame* interfaceGame, Level* level) :
	interfaceGame(interfaceGame),
	level(level)
{
}

void SaucerTutorial::update(float dt)
{
	if (!isDone() && areConditionsMet()) {
		if (!running) {
			startDialogues();
			running = true;
		}
		else if (dialogues != nullptr){
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
	auto config = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GAMETUTORIAL);

	return level->getLevelId() == config["saucer"]["level"].asInt() &&
		level->getWorldId() == config["saucer"]["world"].asInt() &&
		interfaceGame->getGameState() == InterfaceGame::GameState::TITLE;
}

bool SaucerTutorial::isDone() {
	return ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->isGameTutorialComplete("saucer");
}

void SaucerTutorial::startDialogues() {
	auto config = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GAMETUTORIAL);
	interfaceGame->pauseLevel();
	dialogues = Dialogue::createFromConfig(config["saucer"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	interfaceGame->hideStartMenu();
	running = true;
}

void SaucerTutorial::endTutorial() {
	((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->completeTutorial("saucer");
	interfaceGame->removeChildByName("hand");
	interfaceGame->resetTowerMenu();
	interfaceGame->resumeLevel();
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
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
	Config*  config = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass();
	return config->getLastLevelAction()["action"] == "create_tower" &&
		config->getLastLevelAction()["tower_name"] == "bomber";
}

