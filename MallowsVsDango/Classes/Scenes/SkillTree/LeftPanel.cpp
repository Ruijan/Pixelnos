#include "LeftPanel.h"

LeftPanel* LeftPanel::create(Json::Value &savedSkills, GUISettings* settings) {
	LeftPanel* leftPanel = new LeftPanel();
	leftPanel->init(savedSkills, settings);
	
	//addChild(leftPanel);
	return leftPanel;
}

void LeftPanel::init(Json::Value &savedSkills, GUISettings * settings) {
	this->settings = settings;
	Node* xLastElement = createSkillInformationPanel();
	Node* yLastElement = createTitle();
	yLastElement = createSugarSprite(xLastElement, yLastElement);
	createSugarAmountLabel(savedSkills, xLastElement, yLastElement);
	yLastElement = createSkillNameLabel(yLastElement);
	createSkillDescription(yLastElement);
}

void LeftPanel::setSugarAmount(int amount)
{
	sugarAmount->setString("X " + Json::Value(amount).asString());
}

cocos2d::Sprite* LeftPanel::createSkillInformationPanel() {
	cocos2d::Sprite* skill_info = cocos2d::Sprite::create("res/buttons/menupanel5.png");
	addChild(skill_info, 1);
	skill_info->setScaleX(settings->getVisibleSize().width / skill_info->getContentSize().width / 4);
	skill_info->setScaleY(settings->getVisibleSize().height / skill_info->getContentSize().height);
	skill_info->setPosition(cocos2d::Vec2(0.f, 0.f));
	skill_info->setAnchorPoint(cocos2d::Vec2(0.f, 0.f));
	return skill_info;
}

void LeftPanel::createSkillDescription(cocos2d::Node * yLastElement)
{
	skillDescription = cocos2d::Label::createWithTTF(settings->getButton("waiting_talent"),
		"fonts/LICABOLD.ttf", round(settings->getVisibleSize().width / 55.f));
	skillDescription->setColor(cocos2d::Color3B::BLACK);
	skillDescription->setWidth(settings->getVisibleSize().width / 5);
	skillDescription->setPosition(cocos2d::Vec2(settings->getVisibleSize().width / 8, yLastElement->getPosition().y - yLastElement->getContentSize().height / 2 -
		settings->getVisibleSize().height / 25));
	skillDescription->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
	skillDescription->setAnchorPoint(cocos2d::Vec2(0.5f, 1.f));
	skillDescription->runAction(cocos2d::RepeatForever::create(cocos2d::Sequence::createWithTwoActions(cocos2d::FadeIn::create(1.f), cocos2d::FadeOut::create(1.f))));
	addChild(skillDescription, 1, "skillDescription");
}

void LeftPanel::createSugarAmountLabel(Json::Value & savedSkills, cocos2d::Node * xLastElement, cocos2d::Node * yLastElement)
{
	sugarAmount = cocos2d::Label::createWithTTF("X " + savedSkills["holy_sugar"].asString(), "fonts/LICABOLD.ttf", round(settings->getVisibleSize().width / 25));
	sugarAmount->setColor(cocos2d::Color3B::YELLOW);
	sugarAmount->enableOutline(cocos2d::Color4B::BLACK, 2);
	sugarAmount->setPosition(cocos2d::Vec2(xLastElement->getContentSize().width * xLastElement->getScaleX() / 3, 0) + yLastElement->getPosition());
	addChild(sugarAmount, 1, "sugarAmount");
}

cocos2d::Label* LeftPanel::createTitle() {
	cocos2d::Label* title = cocos2d::Label::createWithTTF(settings->getButtons()["talents"][settings->getLanguage()].asString(),
		"fonts/LICABOLD.ttf", round(settings->getVisibleSize().width / 20));
	title->setColor(cocos2d::Color3B::YELLOW);
	title->enableOutline(cocos2d::Color4B::BLACK, 2);
	title->setPosition(cocos2d::Vec2(settings->getVisibleSize().width / 8, settings->getVisibleSize().height - title->getContentSize().height * title->getScaleY() / 2 -
		settings->getVisibleSize().height / 25
	));
	addChild(title, 1, "title");
	return title;
}

cocos2d::Sprite* LeftPanel::createSugarSprite(cocos2d::Node* xLastElement, cocos2d::Node* yLastElement) {
	cocos2d::Sprite* sugar_sprite1 = cocos2d::Sprite::create("res/buttons/holy_sugar.png");
	sugar_sprite1->setScale(settings->getVisibleSize().width / sugar_sprite1->getContentSize().width / 12);
	sugar_sprite1->setPosition(cocos2d::Vec2(xLastElement->getContentSize().width * xLastElement->getScaleX() / 3, yLastElement->getPosition().y -
		yLastElement->getContentSize().height * yLastElement->getScaleY() / 2 -
		sugar_sprite1->getContentSize().height * sugar_sprite1->getScaleY() / 2 - settings->getVisibleSize().height / 40));
	addChild(sugar_sprite1, 1);
	return sugar_sprite1;
}

cocos2d::Label* LeftPanel::createSkillNameLabel(cocos2d::Node* yLastElement) {
	cocos2d::Label* skill_name = cocos2d::Label::createWithTTF(" ",
		"fonts/LICABOLD.ttf", round(settings->getVisibleSize().width / 40));
	skill_name->setColor(cocos2d::Color3B::YELLOW);
	skill_name->enableOutline(cocos2d::Color4B::BLACK, 2);
	skill_name->setWidth(settings->getVisibleSize().width / 5);
	skill_name->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
	skill_name->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
	skill_name->setPosition(cocos2d::Vec2(settings->getVisibleSize().width / 8, yLastElement->getPosition().y -
		yLastElement->getContentSize().height * yLastElement->getScaleY() / 2 -
		skill_name->getContentSize().height / 2 -
		settings->getVisibleSize().height / 40
	));
	addChild(skill_name, 1, "skill_name");
	return skill_name;
}

void LeftPanel::updateSkillDescription(Json::Value &talent)
{
	skillDescription->stopAllActions();
	skillDescription->setOpacity(255);
	skillDescription->setString(talent["description_" + settings->getLanguage()].asString());
}

cocos2d::Label * LeftPanel::getSkillDescription()
{
	return skillDescription;
}
