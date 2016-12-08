#include "Config.h"
#include "cocos2d.h"
#include "AppDelegate.h"
#include "extensions/cocos-ext.h"
#include "NetworkController.h"
#include "../SceneManager.h"
#include "../Level/InterfaceGame.h"
#include <time.h>
#include <stdio.h>

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocos2d::network;

Config::Config(std::string configfilename, std::string savename) : 
	config_filename(configfilename), save_filename(savename), save_file(false),
	always_grid_enabled(false), never_grid_enabled(false), moving_grid_enabled(false),
	limit_enabled(false), dialogues_enabled(false), settings_need_save(false),
	tracking_need_save(false), progression_need_save(false), user_need_creation(false), 
	user_need_save(false), waiting_answer(false), tracking_filename("temp_tracking.json"), c_tracking_index(0), 
	level_tracking_filename("temp_level_tracking.json"), c_level_tracking(-1) {

	//network_controller = new NetworkController("http://127.0.0.1/mvd/");
	network_controller = new NetworkController("http://pixelnos.com/app/");

	scheduler = Director::getInstance()->getScheduler();
	scheduler->retain();
	scheduler->schedule(CC_SCHEDULE_SELECTOR(Config::serverUpdate), this , 5.f, false);

	srand(time(NULL));
}

const Json::Value& Config::getConfigValues() const{
	return root;
}

const Json::Value& Config::getSaveValues() const{
	return rootSav;
}

Json::Value const Config::findSkill(int id) const {
	for (unsigned int i(0); i < rootSav["skill"].size(); ++i) {
		for (unsigned int j(0); j < rootSav["skill"][i].size(); ++j) {
			if (rootSav["skill"][i][j]["id"].asInt() == id) {
				return rootSav["skill"][i][j];
			}
		}
	}
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

void Config::init(){
	auto fileUtils =  FileUtils::getInstance();

	std::string configFile = fileUtils->getStringFromFile(config_filename);
	std::string saveFile = fileUtils->getStringFromFile(FileUtils::getInstance()->getWritablePath() + save_filename);
	std::string trackingFile = fileUtils->getStringFromFile(FileUtils::getInstance()->getWritablePath() + tracking_filename);
	std::string levelTrackingFile = fileUtils->getStringFromFile(FileUtils::getInstance()->getWritablePath() + level_tracking_filename);

	Json::Reader reader;
	Json::Reader readerSav;
	Json::Reader readerTracking;
	Json::Reader readerLevelTracking;
	bool parsingConfigSuccessful(false);
	bool parsingSaveSuccessful(false);
	bool parsingTrackingSuccessful(false);
	bool parsingLevelTrackingSuccessful(false);

	parsingConfigSuccessful = reader.parse(configFile, root, false);
	parsingSaveSuccessful = readerSav.parse(saveFile, rootSav, false);
	parsingTrackingSuccessful = readerTracking.parse(trackingFile, tracking, false);
	parsingLevelTrackingSuccessful = readerTracking.parse(levelTrackingFile, level_tracking, false);

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
		for (unsigned int i(0); i < level_tracking.size(); ++i) {
			saveLevelTrackingIntoDB(level_tracking[i], [&, this, i](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
				waiting_answer = false;
				if (response->isSucceed()) {
					std::vector<char> *buffer = response->getResponseData();
					std::string str(buffer->begin(), buffer->end());

					if (this->level_tracking[i].isMember("tracking_id")) {
						if (str != "") {
							log("error while updating level tracking");
						}
						else {
							this->level_tracking[i]["saved"] = true;
							saveLevelTracking();
							this->tracking_need_save = false;
							log("Updating Level Tracking ok");
						}
					}
					else {
						if (Value(str).asInt() > 0) {
							this->level_tracking[i]["tracking_id"] = Value(str).asInt();
							this->level_tracking[i]["saved"] = true;
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
	}
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

	if (!parsingConfigSuccessful){
		// report to the user the failure and their locations in the document.
		std::string error = reader.getFormattedErrorMessages();
		return;
	}
	if (!parsingSaveSuccessful){
		// report to the user the failure and their locations in the document.
		std::string error = readerSav.getFormattedErrorMessages();
		rootSav["settings"]["always_grid"] = false;
		rootSav["settings"]["moving_grid"] = true;
		rootSav["settings"]["never_grid"] = false;
		rootSav["settings"]["auto_limit"] = false;
		rootSav["settings"]["dialogues"] = true;
		rootSav["settings"]["language"] = Application::getInstance()->getCurrentLanguageCode();
		rootSav["holy_sugar"] = 0;
		rootSav["c_level"] = 0;
		rootSav["c_world"] = 0;
		rootSav["username"] = "";
		//rootSav["tutorials"] = root["tutorials"];
		std::vector<std::string> tuto_names = root["tutorials"].getMemberNames();
		for (unsigned int i(0); i < tuto_names.size(); ++i) {
			rootSav["tutorials"][tuto_names[i]]["state"] = root["tutorials"][tuto_names[i]]["state"];
		}
		for (unsigned int i(0); i < root["towers"].size(); ++i) {
			rootSav["towers"][root["towers"].getMemberNames()[i]]["exp"] = 0;

			rootSav["towers"][root["towers"].getMemberNames()[i]]["max_level"] = 0;
			if (root["towers"][root["towers"].getMemberNames()[i]]["unlock_level"].asInt() == -1 &&
				root["towers"][root["towers"].getMemberNames()[i]]["unlock_skill"].asInt() == -1) {
				rootSav["towers"][root["towers"].getMemberNames()[i]]["unlocked"] = true;
			}
			else {
				rootSav["towers"][root["towers"].getMemberNames()[i]]["unlocked"] = false;
			}
		}
		user_need_creation = true;
		int time_now = (int)time(NULL);
		rootSav["id_player"] = time_now;
		rootSav["exist_in_db"] = false;
		save();
		createUserIntoDB();
	}
	else{
		save_file = true;
		auto tutos = rootSav["tutorials"].getMemberNames();
		for (unsigned int j(0); j < rootSav["tutorials"].getMemberNames().size(); ++j) {
			if (rootSav["tutorials"][tutos[j]]["state"].asString() == "running") {
				rootSav["tutorials"][tutos[j]]["state"] = "uncompleted";
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
	always_grid_enabled = rootSav["settings"]["always_grid"].asBool();
	moving_grid_enabled = rootSav["settings"]["moving_grid"].asBool();
	never_grid_enabled = rootSav["settings"]["never_grid"].asBool();
	limit_enabled = rootSav["settings"]["auto_limit"].asBool();
	dialogues_enabled = rootSav["settings"]["dialogues"].asBool();

	language = rootSav["settings"]["language"].asString();
	loadAllLevels();
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
	setSettingsNeedSave(true);
}

void Config::enableMovingGrid(bool enable) {
	moving_grid_enabled = enable;
	// Change all the checkbox selection to the new value
	for (auto checkbox : moving_grid_buttons) {
		checkbox->setSelected(moving_grid_enabled);
	}
	rootSav["settings"]["moving_grid"] = moving_grid_enabled;
	save();
	setSettingsNeedSave(true);
}

void Config::enableNeverGrid(bool enable) {
	never_grid_enabled = enable;
	// Change all the checkbox selection to the new value
	for (auto checkbox : never_grid_buttons) {
		checkbox->setSelected(never_grid_enabled);
	}
	rootSav["settings"]["never_grid"] = never_grid_enabled;
	save();
	setSettingsNeedSave(true);
}

void Config::enableAutoLimit(bool enable) {
	limit_enabled = enable;
	// Change all the checkbox selection to the new value
	for (auto checkbox : limit_buttons) {
		checkbox->setSelected(limit_enabled);
	}
	rootSav["settings"]["auto_limit"] = limit_enabled;
	save();
	setSettingsNeedSave(true);
}

void Config::enableDialogues(bool enable) {
	dialogues_enabled = enable;
	// Change all the checkbox selection to the new value
	for (auto checkbox : dialogues_buttons) {
		checkbox->setSelected(dialogues_enabled);
	}
	rootSav["settings"]["dialogues"] = dialogues_enabled;
	save();
	setSettingsNeedSave(true);
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
	if (settings_need_save) {
		if (!waiting_answer) {
			std::string request = "action=updateUserSettings&id_game=" + rootSav["id_player"].asString() +
				"&moving_grid=" + rootSav["settings"]["moving_grid"].asString() +
				"&always_grid=" + rootSav["settings"]["always_grid"].asString() +
				"&never_grid=" + rootSav["settings"]["never_grid"].asString() +
				"&dialogues=" + rootSav["settings"]["dialogues"].asString() +
				"&auto_limit=" + rootSav["settings"]["auto_limit"].asString() +
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
		for (unsigned int i(0); i < level_tracking.size(); ++i) {
			if (!level_tracking[i]["saved"].asBool()) {
				saveLevelTrackingIntoDB(level_tracking[i], [&, this, i](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
					waiting_answer = false;
					if (response->isSucceed()) {
						std::vector<char> *buffer = response->getResponseData();
						std::string str(buffer->begin(), buffer->end());
						
						if (this->level_tracking[i].isMember("tracking_id")) {
							if (str != "") {
								log("error while updating level tracking");
							}
							else {
								this->level_tracking[i]["saved"] = true;
								saveLevelTracking();
								this->tracking_need_save = false;
								log("Updating Level Tracking ok");
							}
						}
						else {
							if (Value(str).asInt() > 0) {
								this->level_tracking[i]["tracking_id"] = Value(str).asInt();
								this->level_tracking[i]["saved"] = true;
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
				waiting_answer = true;
			}
		}
	}
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

std::string Config::getStringFromSceneType(SceneManager::SceneType type) {
	switch (type) {
	case SceneManager::SceneType::CREDIT:
		return "credit";
	case SceneManager::SceneType::LEVELS:
		return "levels";
	case SceneManager::SceneType::MENU:
		return "menu";
	case SceneManager::SceneType::GAME:
		return "game";
	case SceneManager::SceneType::SKILLS:
		return "skills";
	case SceneManager::SceneType::SHOP:
		return "shop";
	case SceneManager::SceneType::EDITOR:
		return "editor";
	case SceneManager::SceneType::LOADING:
		return "loading";
	case SceneManager::SceneType::PAUSE:
		return "pause";
	case SceneManager::SceneType::STOP:
		return "stop";
	case SceneManager::SceneType::START:
		return "start";
	default:
		return "error";
	}
}

std::string Config::getStringFromGameState(InterfaceGame::GameState state) {
	switch (state) {
	case InterfaceGame::GameState::INTRO:
		return "intro";
	case InterfaceGame::GameState::TITLE:
		return "title";
	case InterfaceGame::GameState::STARTING:
		return "starting";
	case InterfaceGame::GameState::RUNNING:
		return "running";
	case InterfaceGame::GameState::ENDING:
		return "ending";
	case InterfaceGame::GameState::DONE:
		return "done";
	case InterfaceGame::GameState::NEXT_LEVEL:
		return "next_level";
	default:
		return "error";
	}
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

std::string Config::getLanguage() {
	return language;
}

void Config::setLanguage(std::string lang) {
	if (lang == "en" || lang == "fr") {
		language = lang;
		rootSav["settings"]["language"] = language;
		save();
		setSettingsNeedSave(true);
	}
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
	rootSav["tutorials"][name]["state"] = "complete";
	save();
}

void Config::startTutorial(std::string name) {
	rootSav["tutorials"][name]["state"] = "running";
	save();
}

bool Config::isTutorialComplete(std::string name){
	return rootSav["tutorials"][name]["state"].asString() == "complete";
}

bool Config::isTutorialUncompleted(std::string name) {
	return rootSav["tutorials"][name]["state"].asString() == "uncompleted";
}

bool Config::isTutorialRunning(std::string name) {
	return rootSav["tutorials"][name]["state"].asString() == "running";
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
	std::string level_name = root["worlds"][world_id]["levels"][level_id]["path_level"].asString();
	level_name = level_name.substr(0, level_name.find('.'));
	for (auto& level : rootSav["local_editor_levels"]) {
		if (level["name"].asString() == level_name) {
			return level["id_bdd"].asInt();
		}
	}
	log("Couldn't find the level");
	return 0;
}