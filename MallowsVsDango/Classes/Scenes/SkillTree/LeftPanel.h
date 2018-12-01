#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../../GUI/GUISettings.h"

class LeftPanel : public cocos2d::ui::Layout {
public:
	static LeftPanel* create(Json::Value &savedSkills, GUISettings * settings);
	void init(Json::Value & savedSkills, GUISettings * settings);
	void setSugarAmount(int amount);
	void updateSkillDescription(Json::Value & talent);
	cocos2d::Label* getSkillDescription();

protected:
	cocos2d::Sprite * createSkillInformationPanel();
	void createSkillDescription(cocos2d::Node * yLastElement);
	void createSugarAmountLabel(Json::Value & savedSkills, cocos2d::Node * xLastElement, cocos2d::Node * yLastElement);
	cocos2d::Label * createTitle();
	cocos2d::Sprite * createSugarSprite(cocos2d::Node * xLastElement, cocos2d::Node * yLastElement);
	cocos2d::Label * createSkillNameLabel(cocos2d::Node * yLastElement);

protected:
	GUISettings * settings;
	cocos2d::Label* skillDescription;
	cocos2d::Label* sugarAmount;
	cocos2d::Size visibleSize;
};