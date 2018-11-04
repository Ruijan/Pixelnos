#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../../Config/Config.h"
#include "../../Config/json.h"

class Skills;

class RequirementDescription : public cocos2d::ui::RichText {
public:
	RequirementDescription(Config* config, Skills* skillTree);
	static RequirementDescription* create(Skills* skillTree, cocos2d::Size &visibleSize, Config* config);

	void update(std::string & language, Json::Value & talent, Json::Value & save_root);
	void reset();

	void addNoneRequirementDescription(std::string & language, cocos2d::Size & visibleSize);
	void addHolySugarSpentRequirementDescription(Json::Value & save_root, cocos2d::Size & visibleSize, std::string & language);
	void addPreviousSkillRequirementDescription(std::string & language, cocos2d::Size & visibleSize);
	void addRequirementDescription(const std::string & description, const cocos2d::Color3B & color, const cocos2d::Size & visibleSize);

protected:
	int nbRichTextElements;
	Json::Value currentTalent;
	Config* config;
	Skills* skillTree;
};