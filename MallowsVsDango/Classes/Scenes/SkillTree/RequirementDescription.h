#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../../Config/Config.h"
#include "../../Config/json.h"
#include "../../GUI/GUISettings.h"

class Skills;

class RequirementDescription : public cocos2d::ui::RichText {
public:
	RequirementDescription(Config* config, Skills* skillTree, GUISettings* settings);
	static RequirementDescription* create(Skills* skillTree, GUISettings* settings, Config* config);

	void update(Json::Value & talent, Json::Value & save_root);
	void reset();

	void addNoneRequirementDescription();
	void addHolySugarSpentRequirementDescription(Json::Value & save_root);
	void addPreviousSkillRequirementDescription();
	void addRequirementDescription(const std::string & description, const cocos2d::Color3B & color);

protected:
	int nbRichTextElements;
	Json::Value currentTalent;
	Config* config;
	Skills* skillTree;
	GUISettings* settings;
};