#include "UpgradeTutorial.h"
#include "../Level.h"
#include "../Interface/LevelInterface.h"


UpgradeTutorial::UpgradeTutorial(TutorialSettings* settings, LevelInterface* levelInterface, Level* level) :
	DialogueTutorial(settings),
	levelInterface(levelInterface),
	level(level),
	selectedTower(nullptr)
{
}

void UpgradeTutorial::update(float dt)
{
	if (!isDone() && areConditionsMet()) {
		if (!running) {
			startDialogues();
		}
		else if (dialogues != nullptr) {
			dialogues->update();
			if (dialogues->hasFinished()) {
				levelInterface->removeChild(dialogues);
				dialogues = nullptr;
				showHand();
			}
		}
	}
}

bool UpgradeTutorial::isDone()
{
	return settings->isTutorialComplete("upgrade");
}

UpgradeTutorial::~UpgradeTutorial()
{
	if (running) {
		levelInterface->removeChild(dialogues);
		levelInterface->removeChildByName("hand");
	}
}

void UpgradeTutorial::startDialogues()
{
	for (unsigned int i(0); i < level->getTowers().size(); ++i) {
		if (level->getTowers()[i]->isSameType("bomber") && level->getTowers()[i]->isFixed()) {
			selectedTower = level->getTowers()[i];
			break;
		}
	}
	if (selectedTower != nullptr) {
		levelInterface->pauseLevel();
		levelInterface->setSelectedTower(selectedTower);
		dialogues = Dialogue::createFromConfig(settings->getSettingsMap()["upgrade"]["dialogue"]);
		levelInterface->addChild(dialogues, 1, "dialogue");
		dialogues->launch();
		levelInterface->hideStartMenu();
		levelInterface->lockStartMenu();
		running = true;
	}
}

void UpgradeTutorial::showHand() {
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	levelInterface->addChild(cocos2d::ui::Layout::create(), 2, "invisble_mask");
	cocos2d::ui::Button* mask = cocos2d::ui::Button::create("res/buttons/tranparent_mask.png");
	mask->setScaleX(visibleSize.width / mask->getContentSize().width);
	mask->setScaleY(visibleSize.height / mask->getContentSize().height);
	mask->setPosition(cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2));
	levelInterface->getChildByName("invisble_mask")->addChild(mask);
	cocos2d::Sprite* hand = cocos2d::Sprite::create("res/buttons/hand.png");
	hand->setAnchorPoint(cocos2d::Vec2(0.15f, 0.85f));
	hand->setScale(visibleSize.width / 10 / hand->getContentSize().width);
	hand->setPosition(cocos2d::Vec2(visibleSize.width / 2, 0));
	levelInterface->addChild(hand, 3, "hand");

	auto display_menu = cocos2d::CallFunc::create([this]() {
		levelInterface->showTowerInfo();
	});
	auto validate_tutorial = cocos2d::CallFunc::create([this]() {
		endTutorial();
	});

	hand->runAction(cocos2d::Sequence::create(
		cocos2d::MoveTo::create(0.75f, selectedTower->getPosition()),
		cocos2d::DelayTime::create(0.75f),
		display_menu,
		cocos2d::DelayTime::create(0.75f),
		cocos2d::MoveTo::create(0.75f, selectedTower->getPosition() + cocos2d::Vec2(Cell::getCellWidth() / 2, Cell::getCellHeight())),
		cocos2d::DelayTime::create(0.75f),
		cocos2d::FadeOut::create(0.75f),
		validate_tutorial,
		nullptr)
	);
	levelInterface->unlockStartMenu();
	levelInterface->displayStartMenuIfInTitleState();
}

void UpgradeTutorial::endTutorial()
{
	levelInterface->resumeLevel();
	settings->completeTutorial("upgrade");
	levelInterface->hideTowerInfo();
	levelInterface->setSelectedTower(nullptr);
	levelInterface->removeChildByName("invisble_mask");
	Tutorial::endTutorial();
	running = false;
}

bool UpgradeTutorial::areConditionsMet()
{
	return level->getLevelId() == settings->getSettingsMap()["upgrade"]["level"].asInt() &&
		level->getWorldId() == settings->getSettingsMap()["upgrade"]["world"].asInt();
}
