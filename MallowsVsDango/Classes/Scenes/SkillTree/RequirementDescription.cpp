#include "RequirementDescription.h"
#include "Skills.h"

RequirementDescription::RequirementDescription(Config* config, Skills* skillTree):
	nbRichTextElements(0), 
	config(config),
	currentTalent(Json::Value::null),
	skillTree(skillTree)
{
}

RequirementDescription * RequirementDescription::create(Skills* skillTree, cocos2d::Size &visibleSize, Config* config)
{
	RequirementDescription* description = new RequirementDescription(config, skillTree);
	description->setContentSize(cocos2d::Size(visibleSize.width / 5, visibleSize.height / 3));
	description->ignoreContentAdaptWithSize(false);
	description->setAnchorPoint(cocos2d::Vec2(0.5f, 1.f));
	return description;
}

void RequirementDescription::update(std::string &language, Json::Value &talent, Json::Value &save_root)
{
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	currentTalent = talent;
	reset();
	addRequirementDescription(config->getConfigValues(Config::ConfigType::BUTTON)["requirements"][language].asString(), cocos2d::Color3B::BLACK, visibleSize);
	addHolySugarSpentRequirementDescription(save_root, visibleSize, language);
	addPreviousSkillRequirementDescription(language, visibleSize);
	addNoneRequirementDescription(language, visibleSize);
}

void RequirementDescription::reset()
{
	for (int i(nbRichTextElements-1); i >= 0; i--) {
		removeElement(i);
		nbRichTextElements--;
	}
}

void RequirementDescription::addNoneRequirementDescription(std::string & language, cocos2d::Size &visibleSize)
{
	if (nbRichTextElements == 1) {
		addRequirementDescription(config->getConfigValues(Config::ConfigType::BUTTON)["none"][language].asString(), cocos2d::Color3B(1, 69, 0), visibleSize);
	}
}

void RequirementDescription::addHolySugarSpentRequirementDescription(Json::Value & save_root, cocos2d::Size &visibleSize, std::string & language)
{
	if (currentTalent["condition_sugar"].asInt() != -1) {
		cocos2d::Color3B color(1, 69, 0);
		if (save_root["holy_sugar_spent"].asInt() < currentTalent["condition_sugar"].asInt()) {
			color = cocos2d::Color3B::RED;
			skillTree->getBuyButton()->setEnabled(false);
		}

		addRequirementDescription(currentTalent["condition_sugar"].asString() + " ", color, visibleSize);
		addRequirementDescription(config->getConfigValues(Config::ConfigType::BUTTON)["holy_sugar_spent"][language].asString(), cocos2d::Color3B::BLACK, visibleSize);
	}
}

void RequirementDescription::addPreviousSkillRequirementDescription(std::string & language, cocos2d::Size &visibleSize)
{
	Json::Value conditionSkill = skillTree->getSkillFromID(currentTalent["condition_id"].asInt());
	if (currentTalent["condition_id"].asInt() != -1 && conditionSkill != Json::Value::null) {
		cocos2d::Color3B color(1, 69, 0);
		if (!skillTree->getSavedSkillFromID(currentTalent["condition_id"].asInt())["bought"].asBool()) {
			color = cocos2d::Color3B::RED;
			skillTree->getBuyButton()->setEnabled(false);
		}
		addRequirementDescription(conditionSkill["name_" + language].asString(), color, visibleSize);
		addRequirementDescription(" " + config->getConfigValues(Config::ConfigType::BUTTON)["skill_bought"][language].asString(), cocos2d::Color3B::BLACK, visibleSize);
	}
}

void RequirementDescription::addRequirementDescription(const std::string& description, const cocos2d::Color3B& color, const cocos2d::Size& visibleSize) {
	pushBackElement(cocos2d::ui::RichElementText::create(nbRichTextElements, color, 255, description,
			"fonts/LICABOLD.ttf", round(visibleSize.width / 55.f)));
	++nbRichTextElements;
}