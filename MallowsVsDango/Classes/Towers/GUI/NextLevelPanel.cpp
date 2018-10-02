#include "NextLevelPanel.h"

NextLevelPanel * NextLevelPanel::create(Config * config, float spriteWidth, Tower * tower, cocos2d::Size size)
{
	NextLevelPanel* panel = new NextLevelPanel();
	if (panel->init(config, spriteWidth, tower, size)) {
		return panel;
	}
	delete panel;
	return nullptr;
}

bool NextLevelPanel::init(Config* config, float spriteWidth, Tower* cTower, cocos2d::Size size)
{
	setContentSize(size);
	CurrentLevelPanel::init(config, spriteWidth, cTower);
	settings = cTower->getTowerSettings();

	const auto buttonsConfig = configClass->getConfigValues(Config::ConfigType::BUTTON);
	std::string language = configClass->getSettings()->getLanguage();
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	cocos2d::Sprite* cost = cocos2d::Sprite::create("res/buttons/sugar.png");
	addChild(cost, 1, "cost");
	cost->setScale(spriteWidth / cost->getContentSize().width);

	auto upgrade_label = cocos2d::Label::createWithTTF(buttonsConfig["upgrade"][language].asString(),
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
	std::string language = configClass->getSettings()->getLanguage();
	std::string s("");
	levelLabel->setString(configClass->getConfigValues(Config::ConfigType::BUTTON)
		["level"][language].asString() + " " +
		Json::Value(tower->getLevel() + 2).asString());

	s = Json::Value(settings->getDamage(tower->getLevel() + 1)).asString();
	s.resize(4);
	attackLabel->setString(s);

	s = Json::Value(settings->getAttackSpeed(tower->getLevel() + 1)).asString();
	s.resize(4);
	speedLabel->setString(s);

	s = Json::Value(tower->getNormalizedRangeFromRange(settings->getRange(tower->getLevel() + 1))).asString();
	s.resize(4);
	rangeLabel->setString(s);
}

void NextLevelPanel::updateCost(int currentSugar) {
	std::string s = Json::Value(settings->getCost(tower->getLevel() + 1)).asString();
	s.resize(4);
	costLabel->setString(s);

	if (currentSugar < settings->getCost(tower->getLevel() + 1)) {
		costLabel->setColor(cocos2d::Color3B::RED);
	}
	else {
		costLabel->setColor(cocos2d::Color3B::YELLOW);
	}
}