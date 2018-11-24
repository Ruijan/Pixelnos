#include "CurrentLevelPanel.h"


CurrentLevelPanel * CurrentLevelPanel::create(float spriteWidth, Tower* tower, GUISettings* settings)
{
	CurrentLevelPanel* panel = new CurrentLevelPanel();
	if (panel->init(spriteWidth, tower, settings)) {
		return panel;
	}
	delete panel;
	return nullptr;
}

bool CurrentLevelPanel::init(float spriteWidth, Tower* cTower, GUISettings* guiSettings)
{
	tower = cTower;
	this->guiSettings = guiSettings;
	addLevelInfo();
	addAttackInfo(spriteWidth);
	addSpeedInfo(spriteWidth);
	addRangeInfo(spriteWidth);

	setSpritesPositions(spriteWidth);
	setLabelsPositions(spriteWidth);
	return true;
}

void CurrentLevelPanel::setSpritesPositions(float spriteWidth) {
	levelLabel->setPosition(cocos2d::Vec2(0, -levelLabel->getContentSize().height / 2));
	attack->setPosition(-spriteWidth / 2, levelLabel->getPosition().y -
		levelLabel->getContentSize().height / 2
		- attack->getContentSize().height*attack->getScaleY() / 2);
	speed->setPosition(-spriteWidth / 2, attack->getPosition().y - attack->getContentSize().height*
		attack->getScaleY() / 2 - speed->getContentSize().height * speed->getScaleY() * 3 / 4);
	range->setPosition(-spriteWidth / 2, speed->getPosition().y - speed->getContentSize().height*
		speed->getScaleY() / 2 - range->getContentSize().height * range->getScaleY() * 3 / 4);
}

void CurrentLevelPanel::setLabelsPositions(float spriteWidth) {
	attackLabel->setPosition(cocos2d::Vec2(attack->getPosition().x + spriteWidth,
		attack->getPosition().y));
	rangeLabel->setPosition(cocos2d::Vec2(range->getPosition().x + spriteWidth,
		range->getPosition().y));
	speedLabel->setPosition(cocos2d::Vec2(speed->getPosition().x + spriteWidth,
		speed->getPosition().y));
}

void CurrentLevelPanel::addLevelInfo() {

	levelLabel = cocos2d::Label::createWithTTF(
		guiSettings->getButton("level") + " " + Json::Value(tower->getLevel() + 1).asString(),
		"fonts/LICABOLD.ttf", 25 * guiSettings->getVisibleSize().width / 1280);
	levelLabel->setColor(cocos2d::Color3B::BLACK);
	
	addChild(levelLabel, 1, "levelLabel");
}

void CurrentLevelPanel::addAttackInfo( float spriteWidth) {
	attack = cocos2d::Sprite::create("res/buttons/attack.png");
	addChild(attack, 1, "attack");
	attack->setScale(spriteWidth / attack->getContentSize().width);

	attackLabel = cocos2d::Label::createWithTTF("", "fonts/LICABOLD.ttf", 25 * guiSettings->getVisibleSize().width / 1280);
	attackLabel->setColor(cocos2d::Color3B::BLACK);
	attackLabel->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	addChild(attackLabel, 1, "attackLabel");
}

void CurrentLevelPanel::addRangeInfo(float spriteWidth) {
	range = cocos2d::Sprite::create("res/buttons/range.png");
	addChild(range, 1, "range");
	range->setScale(spriteWidth / range->getContentSize().width);
	
	rangeLabel = cocos2d::Label::createWithTTF("", "fonts/LICABOLD.ttf", 25 * guiSettings->getVisibleSize().width / 1280);
	rangeLabel->setColor(cocos2d::Color3B::BLACK);
	
	rangeLabel->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	addChild(rangeLabel, 1, "rangeLabel");
}

void CurrentLevelPanel::addSpeedInfo(float spriteWidth) {
	speed = cocos2d::Sprite::create("res/buttons/speed.png");
	addChild(speed, 1, "speed");
	speed->setScale(spriteWidth / speed->getContentSize().width);

	speedLabel = cocos2d::Label::createWithTTF("", "fonts/LICABOLD.ttf", 25 * guiSettings->getVisibleSize().width / 1280);
	speedLabel->setColor(cocos2d::Color3B::BLACK);
	
	speedLabel->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	addChild(speedLabel, 1, "speedLabel");
}

void CurrentLevelPanel::updateLabel() {
	std::string s("");
	levelLabel->setString(guiSettings->getButton("level") + " " +
		Json::Value(tower->getLevel() + 1).asString());

	s = Json::Value(tower->getDamage()).asString();
	s.resize(4);
	attackLabel->setString(s);

	s = Json::Value(tower->getAttackSpeed()).asString();
	s.resize(4);
	speedLabel->setString(s);

	s = Json::Value(tower->getNormalizedRange()).asString();
	s.resize(4);
	rangeLabel->setString(s);
}

const cocos2d::Vec2& CurrentLevelPanel::getRangePosition()
{
	return rangeLabel->getPosition();
}

const cocos2d::Vec2 & CurrentLevelPanel::getLevelLabelPosition()
{
	return levelLabel->getPosition();
}

const cocos2d::Size & CurrentLevelPanel::getLevelLabelSize()
{
	return levelLabel->getContentSize();
}

void CurrentLevelPanel::setTower(Tower * cTower)
{
	tower = cTower;
}
