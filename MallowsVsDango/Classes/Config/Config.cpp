#include "Config.h"
#include "cocos2d.h"
#include "AppDelegate.h"
#include "extensions/cocos-ext.h"
#include "NetworkController.h"


USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocos2d::network;

Config::Config(std::string configfilename, std::string savename) : 
	config_filename(configfilename), save_filename(savename), save_file(false),
	always_grid_enabled(false), never_grid_enabled(false), moving_grid_enabled(false),
	limit_enabled(false), dialogues_enabled(false), settings_need_save(false),
	tracking_need_save(false), progression_need_save(false), user_need_creation(false), 
	user_need_save(false), waiting_answer(false), 
	tracking_filename("temp_tracking.json"), c_tracking_index(0){

	network_controller = new NetworkController("http://127.0.0.1");
	scheduler = Director::getInstance()->getScheduler();
	scheduler->retain();
	scheduler->schedule(CC_SCHEDULE_SELECTOR(Config::serverUpdate), this , 5.f, false);

}

const Json::Value& Config::getConfigValues() const{
	return root;
}

Json::Value const Config::getSaveValues() const{
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

void Config::init(){
	auto fileUtils =  FileUtils::getInstance();

	std::string configFile = fileUtils->getStringFromFile(config_filename);
	std::string saveFile = fileUtils->getStringFromFile(FileUtils::getInstance()->getWritablePath() + save_filename);
	std::string trackingFile = fileUtils->getStringFromFile(FileUtils::getInstance()->getWritablePath() + tracking_filename);

	Json::Reader reader;
	Json::Reader readerSav;
	Json::Reader readerTracking;
	bool parsingConfigSuccessful(false);
	bool parsingSaveSuccessful(false);
	bool parsingTrackingSuccessful(false);

	parsingConfigSuccessful = reader.parse(configFile, root, false);
	parsingSaveSuccessful = readerSav.parse(saveFile, rootSav, false);
	parsingTrackingSuccessful = readerTracking.parse(trackingFile, tracking, false);

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
		rootSav["holy_sugar"] = 0;
		rootSav["c_level"] = 0;
		rootSav["c_world"] = 0;
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

	language = Application::getInstance()->getCurrentLanguageCode();
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
	}
}

void Config::addTrackingEvent(TrackingEvent n_event) {
	Json::Value c_event;
	c_event["from_scene"] = getStringFromSceneType(n_event.from_scene);
	c_event["to_scene"] = getStringFromSceneType(n_event.to_scene);
	tm *ltm = gmtime(&n_event.time);
	c_event["time"] = Json::Value(1900 + ltm->tm_year).asString() +
		"-" + Json::Value(ltm->tm_mon + 1).asString() +
		"-" + Json::Value(ltm->tm_mday).asString() +
		" " + Json::Value(ltm->tm_hour).asString() +
		":" + Json::Value(ltm->tm_min).asString() +
		":" + Json::Value(ltm->tm_sec).asString();
	tracking[c_tracking_index]["path"].append(c_event);
	tracking[c_tracking_index]["saved"] = false;
	saveTracking();
	tracking_need_save = true;
}

std::string getStringFromSceneType(SceneManager::SceneType type) {
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
	default:
		return "error";
	}
}

void Config::saveTrackingIntoDB(Json::Value tracking_conf, const cocos2d::network::ccHttpRequestCallback& callback) {
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

void Config::createUserIntoDB() {
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
	std::string postData = "action=updateUser&id_game=" + rootSav["id_player"].asString() +
		"&time=" + Value((int)time(NULL)).asString() + "&level=" + rootSav["c_level"].asString() +
		"&world=" + rootSav["c_world"].asString() + "&holy_sugar=" + rootSav["holy_sugar"].asString();
	network_controller->sendNewRequest(NetworkController::Request::USER_AND_SETTINGS, postData,
		[&, this](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
		waiting_answer = false;
		if (response->isSucceed()) {
			std::vector<char> *buffer = response->getResponseData();
			std::string str(buffer->begin(), buffer->end());
			log("Updating user %s", str == "" ? "ok" : str.c_str());
			if (str == "") {
				this->setProgressionNeedSave(false);
			}
		}
		else {
			log("request updateUser error");
		}
	}, "POST updateUser");

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
			if (str == "") {
				this->setProgressionNeedSave(false);
			}
		}
		else {
			log("request updateTowers error");
		}
	}, "POST updateTowers");
}