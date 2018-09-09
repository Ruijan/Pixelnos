#include "GameSettings.h"

GameSettings::GameSettings() :
	needSave(false),
	saveFolder(""),
	fileName("settings.json")
{
}

void GameSettings::init(std::string& saveFolder)
{
	this->saveFolder = saveFolder;
	auto fileUtils = cocos2d::FileUtils::getInstance();
	Json::Reader reader;
	std::string saveFile = fileUtils->getStringFromFile(cocos2d::FileUtils::getInstance()->getWritablePath() + fileName);
	bool parsingSaveSuccessful = reader.parse(saveFile, settingsSave, false);
	if (!parsingSaveSuccessful) {
		createNewSettings();
	}
	loadPreviousSettings();
}

void GameSettings::loadPreviousSettings()
{
	alwaysGridEnabled = settingsSave["grid"]["always"].asBool();
	movingGridEnabled = settingsSave["grid"]["moving"].asBool();
	neverGridEnabled = settingsSave["grid"]["never"].asBool();
	limit_enabled = settingsSave["auto_limit"].asBool();
	dialoguesEnabled = settingsSave["dialogues"].asBool();
	language = settingsSave["language"].asString();
}

void GameSettings::createNewSettings()
{
	settingsSave["grid"]["always"] = false;
	settingsSave["grid"]["moving"] = true;
	settingsSave["grid"]["never"] = false;
	settingsSave["auto_limit"] = false;
	settingsSave["dialogues"] = true;
	settingsSave["language"] = cocos2d::Application::getInstance()->getCurrentLanguageCode();
	save();
}

void GameSettings::addAlwaysGridCheckbox(cocos2d::ui::CheckBox* box) {
	box->setSelected(alwaysGridEnabled);
	alwaysGridCheckboxes.push_back(box);
	box->addTouchEventListener([&](cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			enableAlwaysGrid(true);
		}
	});
}

void GameSettings::addMovingGridButton(cocos2d::ui::CheckBox* box) {
	box->setSelected(movingGridEnabled);
	movingGridCheckboxes.push_back(box);
	box->addTouchEventListener([&](cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			enableMovingGrid(true);
		}
	});
}

void GameSettings::addNeverGridButton(cocos2d::ui::CheckBox* box) {
	box->setSelected(neverGridEnabled);
	neverGridCheckboxes.push_back(box);
	box->addTouchEventListener([&](cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			enableNeverGrid(true);
		}
	});
}


void GameSettings::addLimitButton(cocos2d::ui::CheckBox* box) {
	box->setSelected(!limit_enabled);
	limitCheckboxes.push_back(box);
	box->addTouchEventListener([&](cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			enableAutoLimit(((cocos2d::ui::CheckBox*)sender)->isSelected());
		}
	});
}

void GameSettings::addDialogueButton(cocos2d::ui::CheckBox* box) {
	box->setSelected(!dialoguesEnabled);
	dialoguesCheckboxes.push_back(box);
	box->addTouchEventListener([&](cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			enableDialogues(((cocos2d::ui::CheckBox*)sender)->isSelected());
		}
	});
}

void GameSettings::removeCheckbox(cocos2d::ui::CheckBox * box)
{
	alwaysGridCheckboxes.erase(std::remove(alwaysGridCheckboxes.begin(), alwaysGridCheckboxes.end(), box), alwaysGridCheckboxes.end());
	movingGridCheckboxes.erase(std::remove(movingGridCheckboxes.begin(), movingGridCheckboxes.end(), box), movingGridCheckboxes.end());
	neverGridCheckboxes.erase(std::remove(neverGridCheckboxes.begin(), neverGridCheckboxes.end(), box), neverGridCheckboxes.end());
	limitCheckboxes.erase(std::remove(limitCheckboxes.begin(), limitCheckboxes.end(), box), limitCheckboxes.end());
	dialoguesCheckboxes.erase(std::remove(dialoguesCheckboxes.begin(), dialoguesCheckboxes.end(), box), dialoguesCheckboxes.end());
}

void GameSettings::enableAlwaysGrid(bool enable) {
	alwaysGridEnabled = enable;
	movingGridEnabled = false;
	neverGridEnabled = enable ? false : true;
	updateGridCheckBoxes();
	updateGridSettings();
}

void GameSettings::enableMovingGrid(bool enable) {
	movingGridEnabled = enable;
	neverGridEnabled = enable ? false : true;
	alwaysGridEnabled = false;
	updateGridCheckBoxes();
	updateGridSettings();
}

void GameSettings::enableNeverGrid(bool enable) {
	neverGridEnabled = enable;
	alwaysGridEnabled = enable ? false : true;
	movingGridEnabled = false;
	updateGridCheckBoxes();
	updateGridSettings();
}

void GameSettings::updateGridSettings() {
	settingsSave["grid"]["moving"] = movingGridEnabled;
	settingsSave["grid"]["never"] = neverGridEnabled;
	settingsSave["grid"]["always"] = alwaysGridEnabled;
	save();
	needSave = true;
}

void GameSettings::updateGridCheckBoxes() {
	for (cocos2d::ui::CheckBox* checkbox : movingGridCheckboxes) {
		checkbox->setSelected(false);
	}
	for (cocos2d::ui::CheckBox* checkbox : alwaysGridCheckboxes) {
		checkbox->setSelected(false);
	}
	for (cocos2d::ui::CheckBox* checkbox : neverGridCheckboxes) {
		checkbox->setSelected(false);
	}
}

void GameSettings::enableAutoLimit(bool enable) {
	limit_enabled = enable;
	// Change all the checkbox selection to the new value
	for (auto checkbox : limitCheckboxes) {
		checkbox->setSelected(limit_enabled);
	}
	settingsSave["auto_limit"] = limit_enabled;
	save();
	needSave = true;
}

void GameSettings::enableDialogues(bool enable) {
	dialoguesEnabled = enable;
	// Change all the checkbox selection to the new value
	for (auto checkbox : dialoguesCheckboxes) {
		checkbox->setSelected(dialoguesEnabled);
	}
	settingsSave["dialogues"] = dialoguesEnabled;
	save();
	needSave = true;
}

void GameSettings::setLanguage(const std::string& lang) {
	if (lang == "en" || lang == "fr") {
		language = lang;
		settingsSave["language"] = language;
		save();
		needSave = true;
	}
}


bool GameSettings::isAlwaysGridEnabled() {
	return alwaysGridEnabled;
}

bool GameSettings::isNeverGridEnabled() {
	return neverGridEnabled;
}

bool GameSettings::isMovingGridEnabled() {
	return movingGridEnabled;
}

bool GameSettings::isLimitEnabled() {
	return limit_enabled;
}

bool GameSettings::isDialoguesEnabled() {
	return dialoguesEnabled;
}

const std::string& GameSettings::getLanguage() {
	return language;
}

Json::Value GameSettings::getSettingsSave() {
	return settingsSave;
}

bool GameSettings::doesNeedSave()
{
	return needSave;
}

void GameSettings::setNeedSaving(bool value)
{
	needSave = value;
}

void GameSettings::save()
{
	Json::StyledWriter writer;
	std::string outputSave = writer.write(settingsSave);
	std::string path = cocos2d::FileUtils::getInstance()->getWritablePath() + fileName;

	bool succeed = cocos2d::FileUtils::getInstance()->writeStringToFile(outputSave, path);
	/*if (succeed) {
		log("Saved File in %s", path.c_str());
	}
	else {
		log("error saving file %s", path.c_str());
	}*/
}
