#include "SkillTutorial.h"
#include "../../../AppDelegate.h"
#include "../Skills.h"

SkillTutorial::SkillTutorial(TutorialSettings* settings, Skills* skillTree, GUISettings* guiSettings) :
	running(false),
	settings(settings),
	skillTree(skillTree),
	dialogues(nullptr),
	showingHand(false),
	guiSettings(guiSettings)
{
}

void SkillTutorial::update(float dt)
{
	if (!isDone() && areConditionsMet()) {
		if (!running) {
			startDialogues();
			running = true;
		}
		else {
			dialogues->update();
			if (dialogues->hasFinished() && !showingHand) {
				showingHand = true;
				showHowToBuySkill();
			}
		}
	}
}

bool SkillTutorial::areConditionsMet() {
	auto save = ((AppDelegate*)cocos2d::Application::getInstance())->getSave();
	return settings->isTutorialRunning("skills") &&
		save["c_level"].asInt() >= settings->getSettingsMap()["skills"]["level"].asInt() &&
		save["c_world"].asInt() >= settings->getSettingsMap()["skills"]["world"].asInt();
}

bool SkillTutorial::isDone() {
	return settings->isTutorialComplete("skills");
}

void SkillTutorial::startDialogues() {
	running = true;
	addBlackMask();
	dialogues = Dialogue::createFromConfig(settings->getSettingsMap()["skills"]["dialogue"], guiSettings);
	skillTree->addChild(dialogues, 2, "dialogue");
	dialogues->launch();
}

void SkillTutorial::addBlackMask()
{
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	skillTree->addChild(cocos2d::ui::Layout::create(), 2, "invisble_mask");
	cocos2d::ui::Button* mask = cocos2d::ui::Button::create("res/buttons/tranparent_mask.png");
	mask->setScaleX(visibleSize.width / mask->getContentSize().width);
	mask->setScaleY(visibleSize.height / mask->getContentSize().height);
	mask->setPosition(cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2));
	skillTree->getChildByName("invisble_mask")->addChild(mask, 1);
}

SkillTutorial::~SkillTutorial()
{
}

void SkillTutorial::showHowToBuySkill()
{
	skillTree->removeChild(dialogues);
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	cocos2d::Sprite* hand = createHand(visibleSize);

	auto select_skill = cocos2d::CallFunc::create([this]() {
		skillTree->selectSkill(1);
	});
	auto show_skill_buy = createShowSkillFunction(hand);
	cocos2d::ui::Button* firstTalentButton = skillTree->getTalentButton(1);
	hand->runAction(cocos2d::Sequence::create(
		cocos2d::FadeIn::create(0.5f),
		cocos2d::DelayTime::create(0.5f),
		cocos2d::Spawn::createWithTwoActions(
			cocos2d::MoveBy::create(1.5f, cocos2d::Vec2(skillTree->getChildByName("talent_page")->getPosition().x +
				firstTalentButton->getPosition().x -
				visibleSize.width / 2, 0)),
			cocos2d::EaseBackOut::create(cocos2d::MoveBy::create(1.5f,
				cocos2d::Vec2(0, skillTree->getChildByName("talent_page")->getPosition().y +
					firstTalentButton->getPosition().y -
					firstTalentButton->getContentSize().height *
					firstTalentButton->getScaleY() / 2 -
					visibleSize.height / 2)))),
		cocos2d::DelayTime::create(0.25f),
		select_skill,
		cocos2d::DelayTime::create(0.25f),
		cocos2d::MoveTo::create(1.f, skillTree->getChildByName("buyingLayout")->getPosition()),
		cocos2d::DelayTime::create(0.25f),
		show_skill_buy,
		nullptr));
}

cocos2d::Sprite* SkillTutorial::createHand(cocos2d::Size &visibleSize)
{
	cocos2d::Sprite* hand = cocos2d::Sprite::create("res/buttons/hand.png");
	hand->setAnchorPoint(cocos2d::Vec2(0.15f, 0.90f));
	hand->setScale(visibleSize.width / 10 / hand->getContentSize().width);
	hand->setPosition(visibleSize / 2);
	hand->setOpacity(0.f);
	skillTree->addChild(hand, 4, "hand");
	return hand;
}

cocos2d::CallFunc * SkillTutorial::createShowSkillFunction(cocos2d::Sprite * hand)
{
	return cocos2d::CallFunc::create([this, hand]() {
		skillTree->showValidationPanel();
		auto end_tutorial = cocos2d::CallFunc::create([this, hand]() {
			skillTree->removeChildByName("invisble_mask");
			skillTree->removeChildByName("hand");
			skillTree->hideValidationPanel();
			settings->completeTutorial("skills");
			showingHand = false;
			running = false;
		});
		cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
		cocos2d::Vec2 pos = cocos2d::Vec2(visibleSize.width, visibleSize.height) / 2 + skillTree->getChildByName("validationLayout")->getChildByName("validate_button")->getPosition();
		hand->runAction(cocos2d::Sequence::create(
			cocos2d::DelayTime::create(0.25f),
			cocos2d::MoveTo::create(1.f, pos),
			cocos2d::DelayTime::create(0.25f),
			cocos2d::FadeOut::create(0.5f),
			end_tutorial,
			nullptr
		));
	});
}