#include "TowerPositioningTutorial.h"
#include "../Interface/LevelInterface.h"
#include "../../Config/Config.h"



TowerPositioningTutorial::TowerPositioningTutorial(Config* config, LevelInterface * nInterfaceGame):
	Tutorial(config->getGameTutorialSettings()),
	levelInterface(nInterfaceGame),
	config(config)
{
}

void TowerPositioningTutorial::update(float dt)
{
	if (!isDone() && !running && levelInterface->getGameState() == LevelInterface::GameState::TITLE) {
		cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
		running = true;
		shakeElement(levelInterface->getTowerFromMenu("bomber"), false);
		levelInterface->hideStartMenu();
		levelInterface->lockStartMenu();
		cocos2d::Sprite* hand = cocos2d::Sprite::create("res/buttons/hand.png");
		hand->setAnchorPoint(cocos2d::Vec2(0.15f, 0.5f));
		hand->setScale(visibleSize.width / 10 / hand->getContentSize().width);
		hand->setPosition(levelInterface->getAbsoluteMenuTowerPosition("bomber"));
		hand->setOpacity(0.0f);
		levelInterface->addChild(hand, 3, "hand");
		hand->runAction(cocos2d::RepeatForever::create(cocos2d::Sequence::create(
			cocos2d::DelayTime::create(1.f),
			cocos2d::FadeIn::create(0.5f),
			cocos2d::DelayTime::create(0.5f),
			cocos2d::Spawn::createWithTwoActions(
				cocos2d::MoveBy::create(1.5f, cocos2d::Vec2(-visibleSize.width / 2, 0)),
				cocos2d::EaseBackOut::create(
					cocos2d::MoveBy::create(1.5f, cocos2d::Vec2(0, -visibleSize.height / 3)))),
			cocos2d::DelayTime::create(0.5f),
			cocos2d::FadeOut::create(0.5f),
			cocos2d::MoveTo::create(0.f, levelInterface->getAbsoluteMenuTowerPosition("bomber")),
			nullptr))
		);
	}
	else if (running && !isDone() && isLastTowerCreatedABomber()) {
		endTutorial();
	}
}

void TowerPositioningTutorial::endTutorial() {
	settings->completeTutorial("tower_positioning");
	levelInterface->removeChildByName("hand");
	levelInterface->resetTowerMenu();
	levelInterface->unlockStartMenu();
	levelInterface->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
	running = false;
}

bool TowerPositioningTutorial::isDone()
{
	return settings->isTutorialComplete("tower_positioning");
}

bool TowerPositioningTutorial::isLastTowerCreatedABomber() {
	return config->getLastLevelAction()["action"] == "create_tower" &&
		config->getLastLevelAction()["tower_name"] == "bomber";
}

TowerPositioningTutorial::~TowerPositioningTutorial()
{
	if (running) {
		levelInterface->removeChildByName("hand");
	}
}
