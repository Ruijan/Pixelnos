#include "Config.h"
#include "cocos2d.h"

USING_NS_CC;

Config::Config(std::string configfilename, std::string savename) : 
	config_filename(configfilename), save_filename(savename), save_file(false),
	always_grid_enabled(false), never_grid_enabled(false), moving_grid_enabled(false),
	limit_enabled(false), dialogues_enabled(false){
}

Json::Value const Config::getConfigValues() const{
	return root;
}

Json::Value const Config::getSaveValues() const{
	return rootSav;
}

bool Config::isSaveFile() const{
	return save_file;
}

void Config::setSave(Json::Value nroot){
	rootSav = nroot;
}

void Config::save(){
	Json::StyledWriter writer;
	std::string outputSave = writer.write(rootSav);
	std::string path = FileUtils::getInstance()->getWritablePath() + save_filename;

	bool succeed = FileUtils::getInstance()->writeStringToFile(outputSave, path);
	if(succeed){
		log("Saved File in %s",path.c_str());
	}
	else{
		log("error saving file %s", path.c_str());
	}
}

void Config::init(){
	auto fileUtils =  FileUtils::getInstance();

	std::string configFile = fileUtils->getStringFromFile(config_filename);
	std::string saveFile = fileUtils->getStringFromFile(FileUtils::getInstance()->getWritablePath() + save_filename);

	Json::Reader reader;
	Json::Reader readerSav;
	bool parsingConfigSuccessful(false);
	bool parsingSaveSuccessful(false);

	parsingConfigSuccessful = reader.parse(configFile, root, false);
	parsingSaveSuccessful = readerSav.parse(saveFile, rootSav, false);


	if (!parsingConfigSuccessful){
		// report to the user the failure and their locations in the document.
		std::string error = reader.getFormattedErrorMessages();
		return;
	}
	if (!parsingSaveSuccessful){
		// report to the user the failure and their locations in the document.
		std::string error = readerSav.getFormattedErrorMessages();
	}
	else{
		save_file = true;
	}
	always_grid_enabled = rootSav["settings"]["always_grid"].asBool();
	moving_grid_enabled = rootSav["settings"]["moving_grid"].asBool();
	never_grid_enabled = rootSav["settings"]["never_grid"].asBool();
	limit_enabled = rootSav["settings"]["auto_limit"].asBool();
	dialogues_enabled = rootSav["settings"]["dialogues"].asBool();
	save();
	
}

void Config::addGridButton(cocos2d::ui::CheckBox* box) {
	box->setSelected(!always_grid_enabled);
	always_grid_buttons.push_back(box);
}

void Config::addMovingGridButton(cocos2d::ui::CheckBox* box) {
	box->setSelected(!moving_grid_enabled);
	moving_grid_buttons.push_back(box);
}

void Config::addNeverGridButton(cocos2d::ui::CheckBox* box) {
	box->setSelected(!never_grid_enabled);
	never_grid_buttons.push_back(box);
}


void Config::addLimitButton(cocos2d::ui::CheckBox* box) {
	box->setSelected(!limit_enabled);
	limit_buttons.push_back(box);
}

void Config::addDialogueButton(cocos2d::ui::CheckBox* box) {
	box->setSelected(!dialogues_enabled);
	dialogues_buttons.push_back(box);
}

void Config::removeGridButton(cocos2d::ui::CheckBox* box) {
	always_grid_buttons.erase(std::remove(always_grid_buttons.begin(), always_grid_buttons.end(), box),
		always_grid_buttons.end());
}

void Config::removeMovingGridButton(cocos2d::ui::CheckBox* box) {
	moving_grid_buttons.erase(std::remove(moving_grid_buttons.begin(), moving_grid_buttons.end(), box),
		moving_grid_buttons.end());
}

void Config::removeNeverGridButton(cocos2d::ui::CheckBox* box) {
	never_grid_buttons.erase(std::remove(never_grid_buttons.begin(), never_grid_buttons.end(), box),
		never_grid_buttons.end());
}

void Config::removeLimitButton(cocos2d::ui::CheckBox* box) {
	limit_buttons.erase(std::remove(limit_buttons.begin(), limit_buttons.end(), box),
		limit_buttons.end());
}

void Config::removeDialogueButton(cocos2d::ui::CheckBox* box) {
	dialogues_buttons.erase(std::remove(dialogues_buttons.begin(), dialogues_buttons.end(), box),
		dialogues_buttons.end());
}

void Config::enableAlwaysGrid(bool enable) {
	always_grid_enabled = enable;
	// Change all the checkbox selection to the new value
	for (auto checkbox : always_grid_buttons) {
		checkbox->setSelected(always_grid_enabled);
	}
	rootSav["settings"]["always_grid"] = always_grid_enabled;
	save();
}

void Config::enableMovingGrid(bool enable) {
	moving_grid_enabled = enable;
	// Change all the checkbox selection to the new value
	for (auto checkbox : moving_grid_buttons) {
		checkbox->setSelected(moving_grid_enabled);
	}
	rootSav["settings"]["moving_grid"] = moving_grid_enabled;
	save();
}

void Config::enableNeverGrid(bool enable) {
	never_grid_enabled = enable;
	// Change all the checkbox selection to the new value
	for (auto checkbox : never_grid_buttons) {
		checkbox->setSelected(never_grid_enabled);
	}
	rootSav["settings"]["never_grid"] = never_grid_enabled;
	save();
}

void Config::enableAutoLimit(bool enable) {
	limit_enabled = enable;
	// Change all the checkbox selection to the new value
	for (auto checkbox : limit_buttons) {
		checkbox->setSelected(limit_enabled);
	}
	rootSav["settings"]["auto_limit"] = limit_enabled;
	save();
}

void Config::enableDialogues(bool enable) {
	dialogues_enabled = enable;
	// Change all the checkbox selection to the new value
	for (auto checkbox : dialogues_buttons) {
		checkbox->setSelected(dialogues_enabled);
	}
	rootSav["settings"]["dialogues"] = dialogues_enabled;
	save();
}

bool Config::isAlwaysGridEnabled() {
	return always_grid_enabled;
}

bool Config::isNeverGridEnabled() {
	return never_grid_enabled;
}

bool Config::isMovingGridEnabled() {
	return moving_grid_enabled;
}

bool Config::isLimitEnabled() {
	return limit_enabled;
}

bool Config::isDialoguesEnabled() {
	return dialogues_enabled;
}