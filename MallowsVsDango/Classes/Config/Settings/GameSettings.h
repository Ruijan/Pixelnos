#pragma once
#include <iostream>
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../json.h"

class GameSettings {
public:
	GameSettings();
	void init(std::string saveFolder);

	void addAlwaysGridCheckbox(cocos2d::ui::CheckBox* box);
	void addMovingGridButton(cocos2d::ui::CheckBox* box);
	void addNeverGridButton(cocos2d::ui::CheckBox* box);
	void addLimitButton(cocos2d::ui::CheckBox* box);
	void addDialogueButton(cocos2d::ui::CheckBox* box);

	void removeCheckbox(cocos2d::ui::CheckBox* box);

	void enableAlwaysGrid(bool enable);
	void enableMovingGrid(bool enable);
	void enableNeverGrid(bool enable);
	void enableAutoLimit(bool enable);
	void enableDialogues(bool enable);

	void updateGridSettings();
	void updateGridCheckBoxes();

	bool isNeverGridEnabled();
	bool isAlwaysGridEnabled();
	bool isMovingGridEnabled();
	bool isLimitEnabled();
	bool isDialoguesEnabled();
	const std::string& getLanguage();

	void setLanguage(std::string lang);

	Json::Value getSettingsSave();
	bool doesNeedSave();
	void setNeedSaving(bool value);
protected:
	void save();

protected:
	std::vector<cocos2d::ui::CheckBox*> alwaysGridCheckboxes;
	std::vector<cocos2d::ui::CheckBox*> neverGridCheckboxes;
	std::vector<cocos2d::ui::CheckBox*> movingGridCheckboxes;
	std::vector<cocos2d::ui::CheckBox*> limitCheckboxes;
	std::vector<cocos2d::ui::CheckBox*> dialoguesCheckboxes;

	bool alwaysGridEnabled;
	bool movingGridEnabled;
	bool neverGridEnabled;
	bool limit_enabled;
	bool dialoguesEnabled;
	std::string language;

	bool needSave;
	Json::Value settingsSave;
	std::string saveFolder;
	std::string fileName;
};