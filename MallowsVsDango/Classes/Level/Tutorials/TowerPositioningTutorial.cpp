#include "TowerPositioningTutorial.h"
#include "../InterfaceGame.h"
#include "../../Config/Config.h"



TowerPositioningTutorial::TowerPositioningTutorial(Config* config, InterfaceGame * nInterfaceGame):
	Tutorial(config),
	interfaceGame(nInterfaceGame)
{
}

void TowerPositioningTutorial::update(float dt)
{
	if (!isDone() && !running && interfaceGame->getGameState() == InterfaceGame::GameState::TITLE) {
		cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
		running = true;
		shakeElement(interfaceGame->getMenuTower("bomber"), false);
		interfaceGame->hideStartMenu();
		cocos2d::Sprite* hand = cocos2d::Sprite::create("res/buttons/hand.png");
		hand->setAnchorPoint(cocos2d::Vec2(0.15f, 0.5f));
		hand->setScale(visibleSize.width / 10 / hand->getContentSize().width);
		hand->setPosition(interfaceGame->getAbsoluteMenuTowerPosition("bomber"));
		hand->setOpacity(0.0f);
		interfaceGame->addChild(hand, 3, "hand");
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
			cocos2d::MoveTo::create(0.f, interfaceGame->getAbsoluteMenuTowerPosition("bomber")),
			nullptr))
		);
	}
	else if (running && !isDone() && isLastTowerCreatedABomber()) {
		endTutorial();
	}
}

void TowerPositioningTutorial::endTutorial() {
	config->completeTutorial("tower_positioning");
	interfaceGame->removeChildByName("hand");
	interfaceGame->resetTowerMenu();
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
}

bool TowerPositioningTutorial::isDone()
{
	return config->isGameTutorialComplete("tower_positioning");
}

bool TowerPositioningTutorial::isLastTowerCreatedABomber() {
	return config->getLastLevelAction()["action"] == "create_tower" &&
		config->getLastLevelAction()["tower_name"] == "bomber";
}

TowerPositioningTutorial::~TowerPositioningTutorial()
{
}
