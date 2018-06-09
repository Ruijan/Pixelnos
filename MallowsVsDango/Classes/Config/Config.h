#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include "json.h"
#include "ui/CocosGUI.h"
#include <random>
#include "extensions/cocos-ext.h"
#include "network/HttpClient.h"
#include "Settings.h"

class NetworkController;
struct LevelTrackingEvent;

struct TrackingEvent {
	std::string from_scene;
	std::string to_scene;
	time_t time;
};



class Config : public cocos2d::Ref {
private:
	Json::Value conf_general;
	Json::Value conf_game_tutorial;
	Json::Value conf_skills_tutorial;
	Json::Value conf_advice;
	Json::Value conf_challenge;
	Json::Value conf_button;
	Json::Value conf_tower;
	Json::Value conf_dango;
	Json::Value conf_level;
	Json::Value conf_talent;

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
	bool settings_need_save;
	bool tracking_need_save;
	bool progression_need_save;
	bool user_need_creation;
	bool user_need_save;
	bool waiting_answer;
	int c_tracking_index;
	int c_level_tracking;

	Settings* settings;

public:
	enum ConfigType {
		GENERAL,
		GAMETUTORIAL,
		SKILLTUTORIAL,
		ADVICE,
		TOWER,
		DANGO,
		BUTTON,
		CHALLENGE,
		TALENT,
		LEVEL
	};
	Config(std::string configfilename, std::string savename);
	void init();

	/**
	* @brief Return the current configuration json value.
	*/
	const Json::Value& getConfigValues(ConfigType type) const;


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

	Json::Value getLastLevelAction();

	/**
	* @brief return the current network controller to send new request.
	*/
	NetworkController* getNetworkController();

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
	bool isGameTutorialComplete(std::string name);

	bool isSkillTutorialComplete(std::string name);

	void completeSkillTutorial(std::string name);

	void startSkillTutorial(std::string name);

	bool isSkillTutorialUncompleted(std::string name);

	bool isSkillTutorialRunning(std::string name);


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
	Settings* getSettings();

};

#endif
