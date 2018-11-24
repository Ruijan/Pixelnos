#include "NextLevelPanel.h"

NextLevelPanel * NextLevelPanel::create(float spriteWidth, Tower * tower, cocos2d::Size size, GUISettings* guiSettings)
{
	NextLevelPanel* panel = new NextLevelPanel();
	if (panel->init(spriteWidth, tower, size, guiSettings)) {
		return panel;
	}
	delete panel;
	return nullptr;
}

bool NextLevelPanel::init(float spriteWidth, Tower* cTower, cocos2d::Size size, GUISettings* guiSettings)
{
	setContentSize(size);
	CurrentLevelPanel::init(spriteWidth, cTower, guiSettings);
	settings = cTower->getTowerSettings();

	cocos2d::Size visibleSize = guiSettings->getVisibleSize();

	cocos2d::Sprite* cost = cocos2d::Sprite::create("res/buttons/sugar.png");
	addChild(cost, 1, "cost");
	cost->setScale(spriteWidth / cost->getContentSize().width);

	auto upgrade_label = cocos2d::Label::createWithTTF(guiSettings->getButton("upgrade"),
		"fonts/LICABOLD.ttf", 30 * visibleSize.width / 1280);
	upgrade_label->setColor(cocos2d::Color3B::YELLOW);
	upgrade_label->enableOutline(cocos2d::Color4B::BLACK, 1);
	upgrade_label->setAlignment(cocos2d::TextHAlignment::CENTER);
	upgrade_label->setPosition(getContentSize().width * getScaleX() / 2,
		range->getPosition().y - range->getContentSize().height * range->getScaleY() / 2 -
		upgrade_label->getContentSize().height / 2);
	addChild(upgrade_label, 1, "upgrade");

	cost->setPosition(spriteWidth, upgrade_label->getPosition().y - upgrade_label->getContentSize().height / 2 - spriteWidth / 2);

	costLabel = cocos2d::Label::createWithTTF("", "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	costLabel->setColor(cocos2d::Color3B::YELLOW);
	costLabel->enableOutline(cocos2d::Color4B::BLACK, 1);
	costLabel->setPosition(cocos2d::Vec2(cost->getPosition().x + attack->getContentSize().width * attack->getScale(),
		cost->getPosition().y));
	costLabel->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	addChild(costLabel, 1, "cost_label");

	return true;
}

void NextLevelPanel::setSpritesPositions(float spriteWidth)
{
	levelLabel->setPosition(getContentSize().width * getScaleX() / 2, -levelLabel->getContentSize().height / 2);
	attack->setPosition(spriteWidth, levelLabel->getPosition().y -
		levelLabel->getContentSize().height / 2
		- attack->getContentSize().height*attack->getScaleY() / 2);
	speed->setPosition(spriteWidth, attack->getPosition().y - attack->getContentSize().height*
		attack->getScaleY() / 2 - speed->getContentSize().height * speed->getScaleY() * 3 / 4);
	range->setPosition(spriteWidth, speed->getPosition().y - speed->getContentSize().height*
		speed->getScaleY() / 2 - range->getContentSize().height * range->getScaleY() * 3 / 4);
}

void NextLevelPanel::updateLabel() {
	int towerLevel = tower->getLevel();
	std::string s("");
	levelLabel->setString(guiSettings->getButton("level") + " " +
		Json::Value(towerLevel + 2).asString());

	s = Json::Value(settings->getDamage(towerLevel + 1)).asString();
	s.resize(4);
	attackLabel->setString(s);

	s = Json::Value(settings->getAttackSpeed(towerLevel + 1)).asString();
	s.resize(4);
	speedLabel->setString(s);

	s = Json::Value(tower->getNormalizedRangeFromRange(settings->getRange(towerLevel + 1))).asString();
	s.resize(4);
	rangeLabel->setString(s);
}

void NextLevelPanel::updateCost(int currentSugar) {
	int towerLevel = tower->getLevel();
	std::string s = Json::Value(settings->getCost(towerLevel + 1)).asString();
	s.resize(4);
	costLabel->setString(s);

	if (currentSugar < settings->getCost(towerLevel + 1)) {
		costLabel->setColor(cocos2d::Color3B::RED);
	}
	else {
		costLabel->setColor(cocos2d::Color3B::YELLOW);
	}
}