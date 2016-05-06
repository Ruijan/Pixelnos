#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include "json.h"
#include "ui/CocosGUI.h"

class Config{
private:
	Json::Value root;   // will contains the root value after parsing.
	std::string config_filename;
	Json::Value rootSav;   // will contains the root value after parsing.
	std::string save_filename;

	bool save_file;
	bool always_grid_enabled;
	bool moving_grid_enabled;
	bool never_grid_enabled;
	bool limit_enabled;
	bool dialogues_enabled;
	std::string language; 

	std::vector<cocos2d::ui::CheckBox*> always_grid_buttons;
	std::vector<cocos2d::ui::CheckBox*> never_grid_buttons;
	std::vector<cocos2d::ui::CheckBox*> moving_grid_buttons;
	std::vector<cocos2d::ui::CheckBox*> limit_buttons;
	std::vector<cocos2d::ui::CheckBox*> dialogues_buttons;

public:
	Config(std::string configfilename, std::string savename);
	void init();

	/**
	* @brief Return the current configuration json value.
	*/
	const Json::Value&  getConfigValues() const;

	/**
	* @brief Return the current save json value.
	*/
	bool isSaveFile() const;

	/**
	* @brief Return the current save json value.
	*/
	Json::Value const getSaveValues() const;

	/**
	* @brief Save the configuration, the skills and so on.
	* @param nroot. Set the json map of the saving file.
	*/
	void setSave(Json::Value nroot);

	/**
	* @brief Save the configuration, the skills and so on.
	*/
	void save();


	/// SETTINGS PART OF THE CONFIGURATION
	/**
	* @brief Add a checkbox to the vector of checkbox. It will add it to the update
	* loop. The checkbox can be attributed to music or to effects. When the checkbox
	* is selected (disabled), it will disable the sound for the sound type.
	* @param CheckBox to add.
	*/
	void addGridButton(cocos2d::ui::CheckBox* box);
	/**
	* @brief Add a checkbox to the vector of checkbox. It will add it to the update
	* loop. The checkbox can be attributed to music or to effects. When the checkbox
	* is selected (disabled), it will disable the sound for the sound type.
	* @param CheckBox to add.
	*/
	void addMovingGridButton(cocos2d::ui::CheckBox* box);
	/**
	* @brief Add a checkbox to the vector of checkbox. It will add it to the update
	* loop. The checkbox can be attributed to music or to effects. When the checkbox
	* is selected (disabled), it will disable the sound for the sound type.
	* @param CheckBox to add.
	*/
	void addNeverGridButton(cocos2d::ui::CheckBox* box);
	/**
	* @brief Add a checkbox to the vector of checkbox. It will add it to the update
	* loop. When the checkbox is selected (disabled), it will disable the automatic limit.
	* @param CheckBox to add.
	*/
	void addLimitButton(cocos2d::ui::CheckBox* box);
	/**
	* @brief Add a checkbox to the vector of checkbox. It will add it to the update
	* loop. When the checkbox is selected (disabled), it will disable the automatic limit.
	* @param CheckBox to add.
	*/
	void addDialogueButton(cocos2d::ui::CheckBox* box);
	/**
	* @brief Add a checkbox to the vector of checkbox. It will add it to the update
	* loop. The checkbox can be attributed to music or to effects. When the checkbox
	* is selected (disabled), it will disable the sound for the sound type.
	* @param CheckBox to add.
	*/
	void removeGridButton(cocos2d::ui::CheckBox* box);
	/**
	* @brief Add a checkbox to the vector of checkbox. It will add it to the update
	* loop. The checkbox can be attributed to music or to effects. When the checkbox
	* is selected (disabled), it will disable the sound for the sound type.
	* @param CheckBox to add.
	*/
	void removeMovingGridButton(cocos2d::ui::CheckBox* box);
	/**
	* @brief Add a checkbox to the vector of checkbox. It will add it to the update
	* loop. The checkbox can be attributed to music or to effects. When the checkbox
	* is selected (disabled), it will disable the sound for the sound type.
	* @param CheckBox to add.
	*/
	void removeNeverGridButton(cocos2d::ui::CheckBox* box);
	/**
	* @brief Add a checkbox to the vector of checkbox. It will add it to the update
	* loop. When the checkbox is selected (disabled), it will disable the automatic limit.
	* @param CheckBox to add.
	*/
	void removeLimitButton(cocos2d::ui::CheckBox* box);
	/**
	* @brief Add a checkbox to the vector of checkbox. It will add it to the update
	* loop. When the checkbox is selected (disabled), it will disable the automatic limit.
	* @param CheckBox to add.
	*/
	void removeDialogueButton(cocos2d::ui::CheckBox* box);

	void enableAlwaysGrid(bool enable);
	void enableMovingGrid(bool enable);
	void enableNeverGrid(bool enable);
	void enableAutoLimit(bool enable);
	void enableDialogues(bool enable);

	bool isNeverGridEnabled();
	bool isAlwaysGridEnabled();
	bool isMovingGridEnabled();
	bool isLimitEnabled();
	bool isDialoguesEnabled();
};

#endif
