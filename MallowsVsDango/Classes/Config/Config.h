#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include "json.h"
#include "ui/CocosGUI.h"
#include <random>
#include "extensions/cocos-ext.h"
#include "network/HttpClient.h"
#include "../SceneManager.h"
#include "../Level/InterfaceGame.h"

class NetworkController;

struct TrackingEvent {
	std::string from_scene;
	std::string to_scene;
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
	Json::Value level_tracking;   // will contains the root value after parsing.
	std::string level_tracking_filename;

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
	int c_level_tracking;

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
	const Json::Value& getConfigValues() const;

	/**
	* @brief Return the current save json value.
	*/
	bool isSaveFile() const;

	/**
	* @brief Return the current save json value.
	*/
	const Json::Value&  getSaveValues() const;

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
	void saveLevelTracking();

	void saveTrackingIntoDB(Json::Value tracking, const cocos2d::network::ccHttpRequestCallback& callback);
	void saveLevelTrackingIntoDB(Json::Value tracking_conf, const cocos2d::network::ccHttpRequestCallback& callback);


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

	/**
	* @brief Change the value of the settings regarding the display of the grid.
	* it will save the new value in the save file and ask to the network to save it 
	* online at the next call. It changes also the value of all the grid checkbox
	* @param true of false if you want to show the grid all the time.
	*/
	void enableAlwaysGrid(bool enable);

	/**
	* @brief Change the value of the settings regarding the display of the grid.
	* it will save the new value in the save file and ask to the network to save it
	* online at the next call. It changes also the value of all the grid checkbox
	* @param true of false if you want to show the grid when a turret is about to
	* be placed
	*/
	void enableMovingGrid(bool enable);

	/**
	* @brief Change the value of the settings regarding the display of the grid.
	* it will save the new value in the save file and ask to the network to save it
	* online at the next call. It changes also the value of all the grid checkbox
	* @param true of false if you never want to show the grid
	*/
	void enableNeverGrid(bool enable);

	/**
	* @brief Change the value of the settings regarding the limit of the turrets.
	* it will save the new value in the save file and ask to the network to save it
	* online at the next call. It changes also the value of all the limit checkbox
	* @param true of false if you want to automatically lunch the limits of the turrets
	*/
	void enableAutoLimit(bool enable);

	/**
	* @brief Change the value of the settings regarding the dialogues.
	* it will save the new value in the save file and ask to the network to save it
	* online at the next call. It changes also the value of all the dialogue checkbox
	* @param true of false if you want to show the dialogues
	*/
	void enableDialogues(bool enable);

	/**
	* @brief Tells to save the settings online
	* @param true of false if you want to save the settings of the user.
	*/
	void setSettingsNeedSave(bool nneed_save);

	/**
	* @brief Tells to save the tracker file online
	* @param true of false if you want to save the tracker of the user.
	*/
	void setTrackingNeedSave(bool nneed_save);

	/**
	* @brief Tells to save the progression file online
	* @param true of false if you want to save the progression of the user.
	*/
	void setProgressionNeedSave(bool nneed_save);

	/**
	* @brief Handle the update of the settings/progression/tracking online.
	* @param float timestep. not used in the function.
	*/
	void serverUpdate(float dt);

	/**
	* @brief Send the request to create a new user into the DB. It will also 
	* generate the towers for progression and its settings. It returns an id
	* that will be used later to update its information.
	*/
	void createUserIntoDB();
	/**
	* @brief Update user information into DB: settings, progression, tracking.
	* If the info cannot be updated, it will continue to try next step.
	*/
	void updateUserInfo();

	/**
	* @brief Add a tracking event to the tracker. 
	* @param TrackingEvent is a struct that needs:
	*		std::string from_scene;
	*		std::string to_scene;
	*		time_t time; // time at which it happens
	*/
	void addTrackingEvent(TrackingEvent n_event);
	/**
	* @brief Add a level tracking event to the tracker. Usually called for every 
	* new level.
	* @param LevelTrackingEvent is a struct that needs:
	*		int level_id;
	*		int world_id;
	*		std::string state;
	*		int holy_sugar;
	*		int duration;
	*		time_t time;
	*		Json::Value actions;
	*/
	void addLevelTrackingEvent(LevelTrackingEvent n_event);

	/**
	* @brief Set the current LevelTrackingEvent to the new n_event. 
	* Usually called every time the user is making a new action.
	* @param LevelTrackingEvent is a struct that needs:
	*		int level_id;
	*		int world_id;
	*		std::string state;
	*		int holy_sugar;
	*		int duration;
	*		time_t time;
	*		Json::Value actions;
	*/
	void updateCurrentLevelTrackingEvent(LevelTrackingEvent n_event);

	/**
	* @brief check the state of the grid.
	*/
	bool isNeverGridEnabled();
	/**
	* @brief check the state of the grid.
	*/
	bool isAlwaysGridEnabled();
	/**
	* @brief check the state of the grid.
	*/
	bool isMovingGridEnabled();
	/**
	* @brief check the state of the auto limit.
	*/
	bool isLimitEnabled();
	/**
	* @brief check the state of the dialogues display.
	*/
	bool isDialoguesEnabled();

	/**
	* @brief return the current network controller to send new request.
	*/
	NetworkController* getNetworkController();

	/**
	* @brief return the string equivalent for a specific SceneType.
	*/
	static std::string getStringFromSceneType(SceneManager::SceneType type);
	/**
	* @brief return the string equivalent for a specific GameState.
	*/
	static std::string getStringFromGameState(InterfaceGame::GameState sate);

	/**
	* @brief return the current language:
	*		"en" for english
	*		"fr" for french
	*/
	std::string getLanguage();

	/**
	* @brief set the current language:
	* @param lang should take only two values.
	*		"en" for english
	*		"fr" for french
	* HAS TO BE CHANGED TO ONLY TAKE AN ENUM !!! 
	* RIGHT NOW THERE IS AN STRING CHECK !!!!
	* AAAAAAAAAAAAAAAAH !!!!!
	*/
	void setLanguage(std::string lang);

	/**
	* @brief Load all the levels from the online database to the computer.
	*	Save everithin in the writable folder (AppData/local/MvD)
	*/
	void loadAllLevels();

	/**
	* @brief Set the state value of a tutorial to complete
	*	Save everithin in the writable folder (AppData/local/MvD)
	*/
	void completeTutorial(std::string name);

	/**
	* @brief Set the state value of a tutorial to running
	*	Save everithin in the writable folder (AppData/local/MvD)
	*/
	void startTutorial(std::string name);

	/**
	* @brief Check if a tutorial has been completed
	*/
	bool isTutorialComplete(std::string name);

	/**
	* @brief Check if a tutorial has not been completed
	*/
	bool isTutorialUncompleted(std::string name);

	/**
	* @brief Check if a tutorial is running
	*/
	bool isTutorialRunning(std::string name);

	/**
	* @brief Convert a date in string at format "%Y-%m-%d %H:%M:%S" to time
	*/
	static tm getTimeFromString(std::string date1);

	/**
	* @brief Change save file for tower so it's now available
	*/
	void activateTower(std::string name);

	/**
	* @brief Return the number of challenges done for the specific level
	*/
	int getNbLevelChallenges(int world_id, int level_id) const;

	/**
	* @brief Return the username
	*/
	std::string getUsername() const;

	/**
	* @brief Set and save username
	*/
	void setUsername(std::string username);

	/**
	* @brief Return the equivalent bdd id of a particular level (useful for keeping track of which level is played)
	*/
	int getLevelBDDID(int world_id, int level_id);

};

#endif
