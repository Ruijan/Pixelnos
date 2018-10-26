#ifndef SKILLS_HPP
#define SKILLS_HPP

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../Config/json.h"
#include "../Config/Config.h"

class Skills : public cocos2d::Scene
{
public:
	virtual bool init(Config* config);
	void initSkillTreeProperties(Json::Value &savedSkills, Json::Value &talents);
	void createLeftPanelForSkillDescrition(cocos2d::Size &visibleSize, Json::Value &buttons, std::string &language, Json::Value &savedSkills);
	void createBuyingButtonLayout(cocos2d::Size &visibleSize, Json::Value &buttons, std::string &language);
	void createValidationLayout(cocos2d::Size &visibleSize, Json::Value &buttons, std::string &language);
	void addBlackMask(cocos2d::Size &visibleSize);
	void createBackToMainMenuButton(cocos2d::Size &visibleSize);
	void addBackground(cocos2d::Size &visibleSize);
	void updateSkillTreeWithSaveFile(Json::Value &save_root, Json::Value &talents, Json::Value &root2);
	void initializeDefaultSkillTree(Json::Value &root2, Json::Value &talents);
	static Skills* create(Config* config);
	static Json::Value getSkillFromID(int id);
	static Json::Value getSavedSkillFromID(int id);
	virtual void update(float dt);
	void updateTutorial(float dt);

protected:
	void hideValidationPanel();
	void showValidationPanel();
	int getSavedSkillPosFromID(int id);
	void setTalentBtnBought(cocos2d::ui::Button* btn);
	void selectSkill(int id);
	void removeSelectedEffectFromSkillButton();
	void addSelectedEffectToSkillButton(cocos2d::ui::Button * talent_btn);
	void updateBuyingLayoutVisibility(Json::Value &talent);
	void updateSkillDescription(Json::Value &talent, std::string &language);
	void createRequirementDescription(Json::Value &buttons, std::string &language, Json::Value &talent, Json::Value &save_root);
	void updateBuyingCapacity(Json::Value &talent, Json::Value &save_root);
	bool hasEnoughHolySugar(Json::Value &talent, Json::Value &save_root);
	void initSkills();
	cocos2d::ui::Button* createSkillButton(Json::Value &save_root, Json::Value & talent, float buttonScale);
	bool shouldEnableSkillButton(Json::Value & save_root, Json::Value & talent);
	std::string getSkillSpriteName(Json::Value &save_root, Json::Value & talent);
	virtual void onEnterTransitionDidFinish();

protected:
	bool tutorial_running;
	cocos2d::ui::Button* c_button;
	Json::Value c_talent;
	std::map<int, cocos2d::ui::Button*> talents_button;
	Config* configClass;
	cocos2d::ui::Layout* buyingLayout;
	cocos2d::Label* skillDescription;
};
#endif