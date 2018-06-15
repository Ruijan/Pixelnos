#include "Config.h"
#include "cocos2d.h"
#include "AppDelegate.h"
#include "extensions/cocos-ext.h"
#include "NetworkController.h"
#include "../Scenes/MyGame.h"
#include <time.h>
#include <stdio.h>

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocos2d::network;

Config::Config(std::string configfilename, std::string savename) :
	config_filename(configfilename), 
	save_filename(savename), 
	save_file(false),
	settings_need_save(false),
	tracking_need_save(false), progression_need_save(false), user_need_creation(false),
	user_need_save(false), waiting_answer(false), tracking_filename("temp_tracking.json"), c_tracking_index(0),
	level_tracking_filename("temp_level_tracking.json"), c_level_tracking(-1) {

	//network_controller = new NetworkController("http://127.0.0.1/mvd/");
	network_controller = new NetworkController("http://pixelnos.com/app/");

	settings = new Settings();

	scheduler = Director::getInstance()->getScheduler();
	scheduler->retain();
	scheduler->schedule(CC_SCHEDULE_SELECTOR(Config::serverUpdate), this, 5.f, false);

	srand(time(NULL));
}

const Json::Value& Config::getConfigValues(ConfigType type) const {
	switch (type) {
	case GENERAL:
		return conf_general;
		break;
	case GAMETUTORIAL:
		return conf_game_tutorial;
		break;
	case SKILLTUTORIAL:
		return conf_skills_tutorial;
		break;
	case ADVICE:
		return conf_advice;
		break;
	case TOWER:
		return conf_tower;
		break;
	case DANGO:
		return conf_dango;
		break;
	case BUTTON:
		return conf_button;
		break;
	case CHALLENGE:
		return conf_challenge;
		break;
	case TALENT:
		return conf_talent;
		break;
	case LEVEL:
		return conf_level;
		break;
	default:
		return conf_general;
		break;
	}

}

const Json::Value& Config::getSaveValues() const {
	return rootSav;
}

bool Config::isSaveFile() const {
	return save_file;
}

void Config::setSave(Json::Value nroot) {
	rootSav = nroot;
}

void Config::save() {
	Json::StyledWriter writer;
	std::string outputSave = writer.write(rootSav);
	std::string path = FileUtils::getInstance()->getWritablePath() + save_filename;

	bool succeed = FileUtils::getInstance()->writeStringToFile(outputSave, path);
	if (succeed) {
		log("Saved File in %s", path.c_str());
	}
	else {
		log("error saving file %s", path.c_str());
	}
}

void Config::saveTracking() {
	Json::StyledWriter writer;
	std::string outputSave = writer.write(tracking);
	std::string path = FileUtils::getInstance()->getWritablePath() + tracking_filename;

	bool succeed = FileUtils::getInstance()->writeStringToFile(outputSave, path);
	if (succeed) {
		log("Saved File in %s", path.c_str());
	}
	else {
		log("error saving file %s", path.c_str());
	}
}

void Config::saveLevelTracking() {
	Json::StyledWriter writer;
	std::string outputSave = writer.write(level_tracking);
	std::string path = FileUtils::getInstance()->getWritablePath() + level_tracking_filename;

	bool succeed = FileUtils::getInstance()->writeStringToFile(outputSave, path);
	if (succeed) {
		log("Saved File in %s", path.c_str());
	}
	else {
		log("error saving file %s", path.c_str());
	}
}

void Config::init() {
	auto fileUtils = FileUtils::getInstance();

	std::string configFile = fileUtils->getStringFromFile(config_filename);
	std::string saveFile = fileUtils->getStringFromFile(FileUtils::getInstance()->getWritablePath() + save_filename);
	std::string trackingFile = fileUtils->getStringFromFile(FileUtils::getInstance()->getWritablePath() + tracking_filename);
	std::string levelTrackingFile = fileUtils->getStringFromFile(FileUtils::getInstance()->getWritablePath() + level_tracking_filename);

	Json::Reader reader;
	bool parsingConfigSuccessful(false);
	bool parsingSaveSuccessful(false);
	bool parsingTrackingSuccessful(false);
	bool parsingLevelTrackingSuccessful(false);

	parsingConfigSuccessful = reader.parse(configFile, conf_general, false);
	if (parsingConfigSuccessful) {
		bool parsing_conf_towers = reader.parse(fileUtils->getStringFromFile(conf_general["configuration_files"]["tower"].asString()), conf_tower, false);
		bool parsing_conf_advice = reader.parse(fileUtils->getStringFromFile(conf_general["configuration_files"]["advice"].asString()), conf_advice, false);
		bool parsing_conf_dangos = reader.parse(fileUtils->getStringFromFile(conf_general["configuration_files"]["dango"].asString()), conf_dango, false);
		bool parsing_conf_challenges = reader.parse(fileUtils->getStringFromFile(conf_general["configuration_files"]["challenge"].asString()), conf_challenge, false);
		bool parsing_conf_game_tutorials = reader.parse(fileUtils->getStringFromFile(conf_general["configuration_files"]["gameTutorial"].asString()), conf_game_tutorial, false);
		bool parsing_conf_skills_tutorials = reader.parse(fileUtils->getStringFromFile(conf_general["configuration_files"]["skillsTutorial"].asString()), conf_skills_tutorial, false);
		bool parsing_conf_talents = reader.parse(fileUtils->getStringFromFile(conf_general["configuration_files"]["talent"].asString()), conf_talent, false);
		bool parsing_conf_levels = reader.parse(fileUtils->getStringFromFile(conf_general["configuration_files"]["level"].asString()), conf_level, false);
		std::string buttons = fileUtils->getStringFromFile(conf_general["configuration_files"]["button"].asString());
		bool parsing_conf_buttons = reader.parse(buttons, conf_button, false);
		if (!parsing_conf_towers || !parsing_conf_advice || !parsing_conf_dangos ||
			!parsing_conf_challenges || !parsing_conf_game_tutorials || !parsing_conf_skills_tutorials || !parsing_conf_talents ||
			!parsing_conf_levels || !parsing_conf_buttons) {
			std::string error = reader.getFormattedErrorMessages();
			throw std::invalid_argument("ERROR : loading configuration files. " + error);
			return;
		}
	}
	else {
		std::string error = reader.getFormattedErrorMessages();
		return;
	}

	parsingLevelTrackingSuccessful = reader.parse(levelTrackingFile, level_tracking, false);
	if (parsingLevelTrackingSuccessful) {
		progression_need_save = true;
		Json::Value tracking_to_remove;
		std::vector<int> index_to_remove;
		for (unsigned int i(0); i < level_tracking.size(); ++i) {
			if (level_tracking[i]["saved"].asBool()) {
				index_to_remove.push_back(i);
			}
		}
		for (unsigned int i(0); i < index_to_remove.size(); ++i) {
			level_tracking.removeIndex(index_to_remove[i], &tracking_to_remove);
			for (unsigned int j(0); j < index_to_remove.size(); ++j) {
				--index_to_remove[j];
			}
		}
		c_level_tracking = level_tracking.size() - 1;
		for (unsigned int levelIndex(0); levelIndex < level_tracking.size(); ++levelIndex) {
			saveLevel(levelIndex);
			waiting_answer = true;
		}
	}

	parsingTrackingSuccessful = reader.parse(trackingFile, tracking, false);
	if (parsingTrackingSuccessful) {
		tracking_need_save = true;
		Json::Value tracking_to_remove;
		std::vector<int> index_to_remove;
		for (unsigned int i(0); i < tracking.size(); ++i) {
			if (tracking[i]["saved"].asBool()) {
				index_to_remove.push_back(i);
			}
		}
		for (unsigned int i(0); i < index_to_remove.size(); ++i) {
			tracking.removeIndex(index_to_remove[i], &tracking_to_remove);
			for (unsigned int j(0); j < index_to_remove.size(); ++j) {
				--index_to_remove[j];
			}
		}
		c_tracking_index = tracking.size();
		for (unsigned int i(0); i < tracking.size(); ++i) {
			saveTrackingIntoDB(tracking[i], [&, this, i](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
				waiting_answer = false;
				if (response->isSucceed()) {
					std::vector<char> *buffer = response->getResponseData();
					std::string str(buffer->begin(), buffer->end());
					log("Updating Tracking ok");
					this->tracking[i]["saved"] = true;
					saveTracking();
				}
			});
		}
	}

	parsingSaveSuccessful = reader.parse(saveFile, rootSav, false);
	settings->init(FileUtils::getInstance()->getWritablePath());
	if (!parsingSaveSuccessful) {
		// report to the user the failure and their locations in the document.
		std::string error = reader.getFormattedErrorMessages();
		rootSav["holy_sugar"] = 0;
		rootSav["c_level"] = 0;
		rootSav["c_world"] = 0;
		rootSav["username"] = "";
		//rootSav["gameTutorials"] = conf_game_tutorial;
		std::vector<std::string> tuto_names = conf_game_tutorial.getMemberNames();
		for (unsigned int i(0); i < tuto_names.size(); ++i) {
			rootSav["gameTutorials"][tuto_names[i]]["state"] = conf_game_tutorial[tuto_names[i]]["state"];
		}
		tuto_names = conf_skills_tutorial.getMemberNames();
		for (unsigned int i(0); i < tuto_names.size(); ++i) {
			rootSav["skillsTutorials"][tuto_names[i]]["state"] = conf_skills_tutorial[tuto_names[i]]["state"];
		}
		for (unsigned int i(0); i < conf_tower.size(); ++i) {
			rootSav["towers"][conf_tower.getMemberNames()[i]]["exp"] = 0;

			rootSav["towers"][conf_tower.getMemberNames()[i]]["max_level"] = 0;
			if (conf_tower[conf_tower.getMemberNames()[i]]["unlock_level"].asInt() == -1 &&
				conf_tower[conf_tower.getMemberNames()[i]]["unlock_skill"].asInt() == -1) {
				rootSav["towers"][conf_tower.getMemberNames()[i]]["unlocked"] = true;
			}
			else {
				rootSav["towers"][conf_tower.getMemberNames()[i]]["unlocked"] = false;
			}
		}
		user_need_creation = true;
		int time_now = (int)time(NULL);
		rootSav["id_player"] = time_now;
		rootSav["exist_in_db"] = false;
		save();
		createUserIntoDB();
	}
	else {
		save_file = true;
		auto tutos = rootSav["gameTutorials"].getMemberNames();
		for (unsigned int j(0); j < rootSav["gameTutorials"].getMemberNames().size(); ++j) {
			if (rootSav["gameTutorials"][tutos[j]]["state"].asString() == "running") {
				rootSav["gameTutorials"][tutos[j]]["state"] = "uncompleted";
			}
		}
		save();
		if (!rootSav.isMember("id_player") || !rootSav.isMember("exist_in_db")) {
			user_need_creation = true;
			int time_now = (int)time(NULL);
			rootSav["id_player"] = time_now;
			rootSav["exist_in_db"] = false;
			save();
			createUserIntoDB();
		}
		else if (!rootSav["exist_in_db"].asBool()) {
			createUserIntoDB();
		}
		else {
			updateUserInfo();
		}
	}
	loadAllLevels();
}

NetworkController* Config::getNetworkController() {
	return network_controller;
}

void Config::setSettingsNeedSave(bool nneed_save) {
	settings_need_save = nneed_save;
}

void Config::setTrackingNeedSave(bool nneed_save) {
	tracking_need_save = nneed_save;
}

void Config::setProgressionNeedSave(bool nneed_save) {
	progression_need_save = nneed_save;
}

void Config::serverUpdate(float dt) {
	if (user_need_creation) {
		createUserIntoDB();
	}
	if (user_need_save) {
		updateUserInfo();
	}
	if (settings->doesNeedSave()) {
		if (!waiting_answer) {
			Json::Value settingsValue = settings->getSettingsSave();
			std::string request = "action=updateUserSettings&id_game=" + rootSav["id_player"].asString() +
				"&moving_grid=" + settingsValue["moving_grid"].asString() +
				"&always_grid=" + settingsValue["always_grid"].asString() +
				"&never_grid=" + settingsValue["never_grid"].asString() +
				"&dialogues=" + settingsValue["dialogues"].asString() +
				"&auto_limit=" + settingsValue["auto_limit"].asString() +
				"&loopMusic=" + rootSav["sound"]["loopMusic"].asString() +
				"&maxVolumeEffects=" + rootSav["sound"]["maxVolumeEffects"].asString() +
				"&maxVolumeMusic=" + rootSav["sound"]["maxVolumeMusic"].asString() +
				"&playEffects=" + rootSav["sound"]["playEffects"].asString() +
				"&playMusic=" + rootSav["sound"]["playMusic"].asString();
			network_controller->sendNewRequest(NetworkController::Request::USER_AND_SETTINGS, request,
				[&, this](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
				waiting_answer = false;
				if (response->isSucceed()) {
					std::vector<char> *buffer = response->getResponseData();
					std::string str(buffer->begin(), buffer->end());
					log("Updating user settings %s", str == "" ? "ok" : str.c_str());
					if (str == "") {
						this->setSettingsNeedSave(false);
						this->settings->setNeedSaving(false);
					}
				}
				else {
					log("request updateSettings error");
				}
			}, "POST updateSettings");
			waiting_answer = true;
		}
	}
	if (tracking_need_save) {
		if (!waiting_answer) {
			for (unsigned int i(0); i < tracking.size(); ++i) {
				if (!tracking[i]["saved"].asBool()) {
					saveTrackingIntoDB(tracking[i], [&, this, i](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
						waiting_answer = false;
						if (response->isSucceed()) {
							std::vector<char> *buffer = response->getResponseData();
							std::string str(buffer->begin(), buffer->end());
							log("Updating Tracking ok");
							if (Value(str).asInt() > 0) {
								this->tracking[i]["tracking_id"] = Value(str).asInt();
							}
							this->tracking[i]["saved"] = true;
							saveTracking();
							this->tracking_need_save = false;
						}
						else {
							log("request updateTracking error");
						}
					});
					waiting_answer = true;
				}
			}
		}
	}
	if (progression_need_save) {
		updateUserInfo();
		for (unsigned int levelIndex(0); levelIndex < level_tracking.size(); ++levelIndex) {
			if (!level_tracking[levelIndex]["saved"].asBool()) {
				saveLevel(levelIndex);
				waiting_answer = true;
			}
		}
	}
}

void Config::saveLevel(unsigned int levelIndex)
{
	saveLevelTrackingIntoDB(level_tracking[levelIndex], [&, this, levelIndex](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
		waiting_answer = false;
		if (response->isSucceed()) {
			std::vector<char> *buffer = response->getResponseData();
			std::string str(buffer->begin(), buffer->end());

			if (this->level_tracking[levelIndex].isMember("tracking_id")) {
				if (str != "") {
					log("error while updating level tracking");
				}
				else {
					this->level_tracking[levelIndex]["saved"] = true;
					saveLevelTracking();
					this->tracking_need_save = false;
					log("Updating Level Tracking ok");
				}
			}
			else {
				if (Value(str).asInt() > 0) {
					this->level_tracking[levelIndex]["tracking_id"] = Value(str).asInt();
					this->level_tracking[levelIndex]["saved"] = true;
					saveLevelTracking();
					this->tracking_need_save = false;
					log("Creating Level Tracking ok");
				}
				else {
					log("error while creating level tracking");
				}
			}
		}
		else {
			log("request handlingLevelTracking error");
		}
	});
}

void Config::addTrackingEvent(TrackingEvent n_event) {
	int events = tracking[c_tracking_index]["path"].size(); //tracking[c_tracking_index]["path"][tracking[c_tracking_index]["path"].size() - 1]["from_scene"].asInt();
	std::string previous_scene = "start";
	if (events > 0) {
		previous_scene = tracking[c_tracking_index]["path"][events - 1]["to_scene"].asString();
	}
	if (previous_scene == n_event.from_scene) {
		// create a new tracking entry
		Json::Value c_event;
		c_event["from_scene"] = n_event.from_scene;
		c_event["to_scene"] = n_event.to_scene;
		// write the current time following the SQL format date.
		tm *ltm = gmtime(&n_event.time);
		c_event["time"] = Json::Value(1900 + ltm->tm_year).asString() +
			"-" + Json::Value(ltm->tm_mon + 1).asString() +
			"-" + Json::Value(ltm->tm_mday).asString() +
			" " + Json::Value(ltm->tm_hour).asString() +
			":" + Json::Value(ltm->tm_min).asString() +
			":" + Json::Value(ltm->tm_sec).asString();
		tracking[c_tracking_index]["path"].append(c_event);
		tracking[c_tracking_index]["saved"] = false;
		// save into the local text file.
		saveTracking();
		// since the value changed, we have to save it online
		tracking_need_save = true;
	}
}

void Config::addLevelTrackingEvent(LevelTrackingEvent n_event) {
	// create a new level tracking with the new progression of the user
	Json::Value c_event;
	c_event["level_id_bdd"] = n_event.level_id_bdd;
	c_event["level_id"] = n_event.level_id;
	c_event["world_id"] = n_event.world_id;
	c_event["state"] = n_event.state;
	c_event["holy_sugar"] = n_event.holy_sugar;
	c_event["duration"] = n_event.duration;
	// write the current time following the SQL date.
	tm *ltm = gmtime(&n_event.time);
	c_event["time"] = Json::Value(1900 + ltm->tm_year).asString() +
		"-" + Json::Value(ltm->tm_mon + 1).asString() +
		"-" + Json::Value(ltm->tm_mday).asString() +
		" " + Json::Value(ltm->tm_hour).asString() +
		":" + Json::Value(ltm->tm_min).asString() +
		":" + Json::Value(ltm->tm_sec).asString();
	c_event["saved"] = false;
	c_event["actions"] = n_event.actions;
	++c_level_tracking;
	level_tracking.append(c_event);
	// save into the local text file.
	saveLevelTracking();
	// since the value changed, we have to save it online
	progression_need_save = true;
}

void Config::updateCurrentLevelTrackingEvent(LevelTrackingEvent n_event) {
	// update the level tracking file with the new progression of the user
	level_tracking[c_level_tracking]["level_id"] = n_event.level_id;
	level_tracking[c_level_tracking]["world_id"] = n_event.world_id;
	level_tracking[c_level_tracking]["state"] = n_event.state;
	level_tracking[c_level_tracking]["holy_sugar"] = n_event.holy_sugar;
	level_tracking[c_level_tracking]["duration"] = n_event.duration;
	// write the current time following the SQL date.
	tm *ltm = gmtime(&n_event.time);
	level_tracking[c_level_tracking]["time"] = Json::Value(1900 + ltm->tm_year).asString() +
		"-" + Json::Value(ltm->tm_mon + 1).asString() +
		"-" + Json::Value(ltm->tm_mday).asString() +
		" " + Json::Value(ltm->tm_hour).asString() +
		":" + Json::Value(ltm->tm_min).asString() +
		":" + Json::Value(ltm->tm_sec).asString();
	level_tracking[c_level_tracking]["saved"] = false;
	level_tracking[c_level_tracking]["actions"] = n_event.actions;
	// save into the local text file.
	saveLevelTracking();
	// since the value changed, we have to save it online
	progression_need_save = true;
}

Json::Value Config::getLastLevelAction() {
	return level_tracking[c_level_tracking]["actions"][level_tracking[c_level_tracking]["actions"].size() - 1];
}

void Config::saveTrackingIntoDB(Json::Value tracking_conf, const cocos2d::network::ccHttpRequestCallback& callback) {
	// Save the tracking information into the DB. It creates a new one 
	// or update the current
	std::string request = "action=";
	Json::StyledWriter writer;
	std::string outputSave = writer.write(tracking_conf["path"]);
	if (tracking_conf.isMember("tracking_id")) {
		request += "update_tracking&id=" + tracking_conf["tracking_id"].asString() +
			"&path=" + outputSave;
		network_controller->sendNewRequest(NetworkController::Request::TRACKING, request, callback,
			"POST update tracking");
	}
	else {
		request += "create_tracking&id_user=" + rootSav["id_player"].asString() +
			"&path=" + outputSave;
		network_controller->sendNewRequest(NetworkController::Request::TRACKING, request, callback,
			"POST createTracking");
	}
}

void Config::saveLevelTrackingIntoDB(Json::Value tracking_conf, const cocos2d::network::ccHttpRequestCallback& callback) {
	// Save the level tracking information into the DB. It creates a new one 
	// or update the current
	std::string request = "action=";
	Json::StyledWriter writer;
	std::string actions = writer.write(tracking_conf["actions"]);
	if (tracking_conf.isMember("tracking_id")) {
		request += "update_level_tracking&id_trial=" + tracking_conf["tracking_id"].asString() +
			"&state=" + tracking_conf["state"].asString() + "&holy_sugar=" + tracking_conf["holy_sugar"].asString() +
			"&duration=" + tracking_conf["duration"].asString() + "&actions=" + actions;
		network_controller->sendNewRequest(NetworkController::Request::LEVEL_TRACKING, request, callback,
			"POST update tracking");
	}
	else {
		request += "create_level_tracking&id_user=" + rootSav["id_player"].asString() + "&level_id_bdd=" + tracking_conf["level_id_bdd"].asString() +
			"&level_id=" + tracking_conf["level_id"].asString() + "&world_id=" + tracking_conf["world_id"].asString() +
			"&state=" + tracking_conf["state"].asString() + "&holy_sugar=" + tracking_conf["holy_sugar"].asString() +
			"&duration=" + tracking_conf["duration"].asString() + "&date_time=" + tracking_conf["time"].asString() +
			"&actions=" + actions;
		network_controller->sendNewRequest(NetworkController::Request::LEVEL_TRACKING, request, callback,
			"POST createTracking");
	}
}

void Config::createUserIntoDB() {
	/* There are two ids, one for the game, one for the database.
	Since the database starts from 1 to +infinite, it would be too easy to
	find the id of the player. It allows also to generate a new profile without
	having an internet connection.*/
	network_controller->sendNewRequest(NetworkController::Request::USER_AND_SETTINGS, "action=create_user&name=ruijan&id_game=" + rootSav["id_player"].asString(),
		[&, this](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
		waiting_answer = false;
		if (response->isSucceed()) {
			std::vector<char> *buffer = response->getResponseData();
			std::string str(buffer->begin(), buffer->end());
			log("Creating user %s", str == "" ? "ok" : str.c_str());
			if (str == "") {
				rootSav["exist_in_db"] = true;
				this->save();
				this->user_need_creation = false;
			}
			else {
				log("error creating user : %s", str.c_str());
			}
		}
		else {
			log("request creatingUser error");
			this->user_need_creation = true;
		}
	}, "POST createUser");
}

void Config::updateUserInfo() {
	/* There are two ids, one for the game, one for the database.
	Since the database starts from 1 to +infinite, it would be too easy to
	find the id of the player. It allows also to generate a new profile without
	having an internet connection.*/
	// save the main user infos
	time_t rawtime;
	time(&rawtime);
	std::string postData = "action=updateUser&id_game=" + rootSav["id_player"].asString() +
		"&time=" + Value((int)rawtime).asString() + "&level=" + rootSav["c_level"].asString() +
		"&world=" + rootSav["c_world"].asString() + "&holy_sugar=" + rootSav["holy_sugar"].asString() +
		"&username=" + rootSav["username"].asString();
	network_controller->sendNewRequest(NetworkController::Request::USER_AND_SETTINGS, postData,
		[&, this](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
		waiting_answer = false;
		if (response->isSucceed()) {
			std::vector<char> *buffer = response->getResponseData();
			std::string str(buffer->begin(), buffer->end());
			log("Updating user %s", str == "" ? "ok" : str.c_str());
			user_need_save = false;
			// If everything went well, we set that we don't need to save again.
			if (str == "") {
				this->setProgressionNeedSave(false);

			}
		}
		else {
			log("request updateUser error");
		}
	}, "POST updateUser");

	// save the user's towers infos
	Json::StyledWriter writer;
	std::string towers = writer.write(rootSav["towers"]);
	postData = "action=updateUserTowers&id_game=" + rootSav["id_player"].asString() +
		"&towers=" + towers;
	network_controller->sendNewRequest(NetworkController::Request::USER_AND_SETTINGS, postData,
		[&, this](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
		waiting_answer = false;
		if (response->isSucceed()) {
			std::vector<char> *buffer = response->getResponseData();
			std::string str(buffer->begin(), buffer->end());
			log("Updating towers %s", str == "" ? "ok" : str.c_str());

			// If everything went well, we set that we don't need to save again.
			if (str == "") {
				this->setProgressionNeedSave(false);
			}
		}
		else {
			log("request updateTowers error");
		}
	}, "POST updateTowers");
}

void Config::loadAllLevels() {
	NetworkController* network = ((AppDelegate*)Application::getInstance())->getConfigClass()->getNetworkController();
	network->sendNewRequest(NetworkController::Request::LEVEL_EDITOR, "action=get_list_level",
		[&](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
		if (!response || !response->isSucceed()) {
			log("response failed");
			log("error buffer: %s", response->getErrorBuffer());
			return;
		}
		else {
			std::vector<char> *buffer = response->getResponseData();
			std::string str(buffer->begin(), buffer->end());
			Json::Reader reader;
			Json::Value root;
			bool parsingConfigSuccessful = reader.parse(str, root, false);
			if (!parsingConfigSuccessful) {
				// report to the user the failure and their locations in the document.
				std::string error = reader.getFormattedErrorMessages();
			}
			else {
				root = root["levels"];
				for (unsigned int i(0); i < root.size(); ++i) {
					Json::Value save_root = ((AppDelegate*)Application::getInstance())->getSave();
					int pos = -1;
					bool should_download = false;
					for (unsigned int j(0); j < save_root["local_editor_levels"].size(); ++j) {
						if (save_root["local_editor_levels"][j]["id_bdd"].asInt() == root[i]["id"].asInt()) {
							pos = j;
							break;
						}
					}
					if (pos != -1) {
						struct tm t1, t2;
						std::string date1 = save_root["local_editor_levels"][pos]["last_update"].asString();
						std::string date2 = root[i]["last_update"].asString();
						t1 = getTimeFromString(date1);
						t2 = getTimeFromString(date2);
						if (difftime(mktime(&t2), mktime(&t1)) > 0) {
							should_download = true;
						}
						if (difftime(mktime(&t2), mktime(&t1)) < 0) {

						}
					}
					else {
						should_download = true;
					}
					if (should_download) {
						NetworkController* network = ((AppDelegate*)Application::getInstance())->getConfigClass()->getNetworkController();

						network->sendNewRequest(NetworkController::Request::LEVEL_EDITOR, "action=get_level&id=" + root[i]["id"].asString(),
							[&, i, root](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
							if (!response || !response->isSucceed()) {
								log("response failed");
								log("error buffer: %s", response->getErrorBuffer());
								return;
							}
							else {
								std::vector<char> *buffer = response->getResponseData();
								std::string str(buffer->begin(), buffer->end());
								Json::Reader reader;
								Json::Value root_level;
								Json::Value level_settings;
								level_settings["id_bdd"] = root[i]["id"];
								level_settings["name"] = root[i]["name"];
								level_settings["last_update"] = root[i]["last_update"];

								bool parsingConfigSuccessful = reader.parse(str, root_level, false);
								if (!parsingConfigSuccessful) {
									// report to the user the failure and their locations in the document.
									std::string error = reader.getFormattedErrorMessages();
								}
								else {
									bool level_folder_exist = FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "Levels/");
									if (!level_folder_exist) {
										FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "Levels/");
									}
									std::string path = FileUtils::getInstance()->getWritablePath() + "Levels/" + root[i]["name"].asString() + ".json";
									bool succeed = FileUtils::getInstance()->writeStringToFile(str, path);
									if (succeed) {
										Json::Value save_root = ((AppDelegate*)Application::getInstance())->getSave();
										if (save_root.isMember("local_editor_levels")) {
											int pos = -1;
											for (unsigned int j(0); j < save_root["local_editor_levels"].size(); ++j) {
												if (save_root["local_editor_levels"][j]["id_bdd"].asInt() == root[i]["id"].asInt()) {
													pos = j;
													break;
												}
											}
											if (pos != -1) {
												level_settings["id"] = save_root["local_editor_levels"][pos]["id"];
												save_root["local_editor_levels"][pos] = level_settings;
											}
											else {
												level_settings["id"] = save_root["local_editor_levels"][save_root["local_editor_levels"].size() - 1]["id"].asInt() - 1;
												save_root["local_editor_levels"][save_root["local_editor_levels"].size()] = level_settings;
											}
										}
										else {
											level_settings["id"] = -2;
											save_root["local_editor_levels"][0] = level_settings;
										}
										((AppDelegate*)Application::getInstance())->getConfigClass()->setSave(save_root);
										((AppDelegate*)Application::getInstance())->getConfigClass()->save();

										log("Saved File in %s", path.c_str());
									}
									else {
										log("error saving file %s", path.c_str());
									}
								}
							}
						}, "POST getLevel");
					}
				}
			}
		}
	}, "POST getListLevels");
}

void Config::completeTutorial(std::string name) {
	rootSav["gameTutorials"][name]["state"] = "complete";
	save();
}

void Config::startTutorial(std::string name) {
	rootSav["gameTutorials"][name]["state"] = "running";
	save();
}

bool Config::isGameTutorialComplete(std::string name) {
	return rootSav["gameTutorials"][name]["state"].asString() == "complete";
}


bool Config::isTutorialUncompleted(std::string name) {
	return rootSav["gameTutorials"][name]["state"].asString() == "uncompleted";
}

bool Config::isTutorialRunning(std::string name) {
	return rootSav["gameTutorials"][name]["state"].asString() == "running";
}

void Config::completeSkillTutorial(std::string name) {
	rootSav["skillsTutorials"][name]["state"] = "complete";
	save();
}

void Config::startSkillTutorial(std::string name) {
	rootSav["skillsTutorials"][name]["state"] = "running";
	save();
}

bool Config::isSkillTutorialComplete(std::string name) {
	return rootSav["skillsTutorials"][name]["state"].asString() == "complete";
}

bool Config::isSkillTutorialUncompleted(std::string name) {
	return rootSav["gameTutorials"][name]["state"].asString() == "uncompleted";
}

bool Config::isSkillTutorialRunning(std::string name) {
	return rootSav["gameTutorials"][name]["state"].asString() == "running";
}

tm Config::getTimeFromString(std::string date1) {
	struct tm t1;
	t1.tm_year = Value(date1.substr(0, 4)).asInt();
	t1.tm_mon = Value(date1.substr(5, 2)).asInt();
	t1.tm_mday = Value(date1.substr(8, 2)).asInt();
	t1.tm_hour = Value(date1.substr(11, 2)).asInt();
	t1.tm_min = Value(date1.substr(14, 2)).asInt();
	t1.tm_sec = Value(date1.substr(17, 2)).asInt();
	return t1;
}

void Config::activateTower(std::string name) {
	rootSav["towers"][name]["unlocked"] = true;
	save();
}

int Config::getNbLevelChallenges(int world_id, int level_id) const {
	Json::Value level_challenge = rootSav["levels"][world_id][level_id]["challenges"];
	if (!level_challenge.isNull()) {
		return level_challenge.asInt();
	}
	return 0;
}

std::string Config::getUsername() const {
	return rootSav.isMember("username") ? rootSav["username"].asString() : "";
}

void Config::setUsername(std::string username) {
	rootSav["username"] = username;
	save();
	user_need_save = true;
}

int Config::getLevelBDDID(int world_id, int level_id) {
	std::string level_name = conf_level["worlds"][world_id]["levels"][level_id]["path_level"].asString();
	level_name = level_name.substr(0, level_name.find('.'));
	for (auto& level : rootSav["local_editor_levels"]) {
		if (level["name"].asString() == level_name) {
			return level["id_bdd"].asInt();
		}
	}
	log("Couldn't find the level");
	return 0;
}

Settings * Config::getSettings()
{
	return settings;
}
