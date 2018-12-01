#ifndef SKILLS_HPP
#define SKILLS_HPP

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../../Config/json.h"
#include "../../Config/Config.h"
#include "Tutorials/SkillTutorial.h"
#include "RequirementDescription.h"
#include "../AdvancedScene.h"
#include "BuyLayout.h"
#include "ValidationLayout.h"
#include "LeftPanel.h"

class Skills : public AdvancedScene
{
public:
	static Skills* create(Config* config, GUISettings* settings);
	virtual bool init(Config* config, GUISettings* settings);
	void createRequirementDescription(Config* configClass);
	void updateRequirementDescriptionPosition();
	void initSkillTreeProperties(Json::Value &savedSkills, Json::Value &talents);
	void createLeftPanelForSkillDescrition(Json::Value &savedSkills);
	void createBuyingButtonLayout();
	void createValidationLayout();
	void validationCallBack(cocos2d::Ref * pSender, cocos2d::ui::Widget::TouchEventType type);
	void buyButtonCallBack(cocos2d::Ref * pSender, cocos2d::ui::Widget::TouchEventType type);
	void addBlackMask();
	void createBackToMainMenuButton();
	void addBackground();
	void updateSkillTreeWithSaveFile(Json::Value &save_root, Json::Value &talents, Json::Value &root2);
	void initializeDefaultSkillTree(Json::Value &root2, Json::Value &talents);

	static Json::Value getSkillFromID(int id);
	static Json::Value getSavedSkillFromID(int id);
	virtual void update(float dt);
	void hideValidationPanel();
	void showValidationPanel();
	void selectSkill(int id);
	cocos2d::ui::Button* getTalentButton(int index);
	cocos2d::ui::Button* getBuyButton();

protected:
	int getSavedSkillPosFromID(int id);
	void setTalentBtnBought(cocos2d::ui::Button* btn);

	void removeSelectedEffectFromSkillButton();
	void addSelectedEffectToSkillButton(cocos2d::ui::Button * talent_btn);
	void updateBuyingLayoutVisibility(Json::Value &talent);
	void updateBuyingCapacity(Json::Value &talent, Json::Value &save_root);
	bool hasEnoughHolySugar(Json::Value &talent, Json::Value &save_root);
	void initSkillTree();
	cocos2d::ui::Button* createSkillButton(Json::Value &save_root, Json::Value & talent, float buttonScale);
	bool shouldEnableSkillButton(Json::Value & save_root, Json::Value & talent);
	std::string getSkillSpriteName(Json::Value &save_root, Json::Value & talent);
	virtual void onEnterTransitionDidFinish();

protected:
	bool tutorial_running;
	cocos2d::ui::Button* c_button;
	Json::Value currentTalent;
	std::map<int, cocos2d::ui::Button*> talentButtons;
	Config* configClass;
	BuyLayout* buyingLayout;
	ValidationLayout* validationLayout;
	SkillTutorial* tutorial;
	LeftPanel* leftPanel;
	RequirementDescription* requirementDescription;
};
#endif