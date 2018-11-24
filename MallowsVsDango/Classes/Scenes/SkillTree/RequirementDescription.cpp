#include "RequirementDescription.h"
#include "Skills.h"

RequirementDescription::RequirementDescription(Config* config, Skills* skillTree, GUISettings* settings) :
	nbRichTextElements(0),
	config(config),
	currentTalent(Json::Value::null),
	skillTree(skillTree),
	settings(settings)
{
}

RequirementDescription * RequirementDescription::create(Skills* skillTree, GUISettings* settings, Config* config)
{
	RequirementDescription* description = new RequirementDescription(config, skillTree, settings);
	cocos2d::Size visibleSize = settings->getVisibleSize();
	description->setContentSize(cocos2d::Size(visibleSize.width / 5, visibleSize.height / 3));
	description->ignoreContentAdaptWithSize(false);
	description->setAnchorPoint(cocos2d::Vec2(0.5f, 1.f));
	return description;
}

void RequirementDescription::update(Json::Value &talent, Json::Value &save_root)
{
	currentTalent = talent;
	reset();
	addRequirementDescription(settings->getButton("requirements"), cocos2d::Color3B::BLACK);
	addHolySugarSpentRequirementDescription(save_root);
	addPreviousSkillRequirementDescription();
	addNoneRequirementDescription();
}

void RequirementDescription::reset()
{
	for (int i(nbRichTextElements - 1); i >= 0; i--) {
		removeElement(i);
		nbRichTextElements--;
	}
}

void RequirementDescription::addNoneRequirementDescription()
{
	if (nbRichTextElements == 1) {
		addRequirementDescription(settings->getButton("none"), cocos2d::Color3B(1, 69, 0));
	}
}

void RequirementDescription::addHolySugarSpentRequirementDescription(Json::Value & save_root)
{
	if (currentTalent["condition_sugar"].asInt() != -1) {
		cocos2d::Color3B color(1, 69, 0);
		if (save_root["holy_sugar_spent"].asInt() < currentTalent["condition_sugar"].asInt()) {
			color = cocos2d::Color3B::RED;
			skillTree->getBuyButton()->setEnabled(false);
		}

		addRequirementDescription(currentTalent["condition_sugar"].asString() + " ", color);
		addRequirementDescription(settings->getButton("holy_sugar_spent"), cocos2d::Color3B::BLACK);
	}
}

void RequirementDescription::addPreviousSkillRequirementDescription()
{
	Json::Value conditionSkill = skillTree->getSkillFromID(currentTalent["condition_id"].asInt());
	if (currentTalent["condition_id"].asInt() != -1 && conditionSkill != Json::Value::null) {
		cocos2d::Color3B color(1, 69, 0);
		if (!skillTree->getSavedSkillFromID(currentTalent["condition_id"].asInt())["bought"].asBool()) {
			color = cocos2d::Color3B::RED;
			skillTree->getBuyButton()->setEnabled(false);
		}
		addRequirementDescription(conditionSkill["name_" + settings->getLanguage()].asString(), color);
		addRequirementDescription(" " + settings->getButton("skill_bought"), cocos2d::Color3B::BLACK);
	}
}

void RequirementDescription::addRequirementDescription(const std::string& description, const cocos2d::Color3B& color) {
	pushBackElement(cocos2d::ui::RichElementText::create(nbRichTextElements, color, 255, description,
		"fonts/LICABOLD.ttf", round(settings->getVisibleSize().width / 55.f)));
	++nbRichTextElements;
}