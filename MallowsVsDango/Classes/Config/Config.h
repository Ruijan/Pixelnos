#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include "json.h"
#include "ui/CocosGUI.h"
#include <random>
#include "extensions/cocos-ext.h"
#include "network/HttpClient.h"
#include "../SceneManager.h"

class NetworkController;

std::string getStringFromSceneType(SceneManager::SceneType type);

struct TrackingEvent {
	SceneManager::SceneType from_scene;
	SceneManager::SceneType to_scene;
	time_t time;
};

class Config: public cocos2d::Ref{
private:
	Json::Value root;   // will contains the root value after parsing.
	std::string config_filename;
	Json::Value rootSav;   // will contains the root value after parsing.
	std::string save_filename;
	Json::Value tracking;   // will contains the root value after parsing.
	std::string tracking_filename;
	NetworkController* network_controller;
	cocos2d::Scheduler* scheduler;

	bool save_file;
	bool always_grid_enabled;
	bool moving_grid_enabled;
	bool never_grid_enabled;
	bool limit_enabled;
	bool dialogues_enabled;
	bool settings_need_save;
	bool tracking_need_save;
	bool progression_need_save;
	bool user_need_creation;
	bool user_need_save;
	bool waiting_answer;
	std::string language; 
	int c_tracking_index;

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
	* @brief Find a skill and return its Json entry.
	*/
	Json::Value const findSkill(int id) const;

	/**
	* @brief Save the configuration, the skills and so on.
	* @param nroot. Set the json map of the saving file.
	*/
	void setSave(Json::Value nroot);

	/**
	* @brief Save the configuration, the skills and so on.
	*/
	void save();

	/**
	* @brief Save the tracking json file.
	*/
	void saveTracking();

	void saveTrackingIntoDB(Json::Value tracking, const cocos2d::network::ccHttpRequestCallback& callback);


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

	void setSettingsNeedSave(bool nneed_save);
	void setTrackingNeedSave(bool nneed_save);
	void setProgressionNeedSave(bool nneed_save);

	void serverUpdate(float dt);
	void createUserIntoDB();
	void updateUserInfo();
	
	void addTrackingEvent(TrackingEvent n_event);

	bool isNeverGridEnabled();
	bool isAlwaysGridEnabled();
	bool isMovingGridEnabled();
	bool isLimitEnabled();
	bool isDialoguesEnabled();

	NetworkController* getNetworkController();
};

#endif
