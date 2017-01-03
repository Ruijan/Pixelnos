#include "ChallengeHandler.h"
#include "../SceneManager.h"
#include "../AppDelegate.h"
#include "../Config/json.h"

USING_NS_CC;

ChallengeHandler* ChallengeHandler::create(const Json::Value& value) {

	ChallengeHandler* challenges = new ChallengeHandler(value);
	if (challenges->init()) {
		return challenges;
	}
	CC_SAFE_DELETE(challenges);
	return NULL;
}

ChallengeHandler::ChallengeHandler(const Json::Value& value) {
	Json::Value j_challenges = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::CHALLENGE);
	Size visibleSize = Director::getInstance()->getVisibleSize();
	float size_button = visibleSize.width / 20;

	// GUI
	ui::Layout* text_layout = ui::Layout::create();
	text_layout->setVisible(false);
	addChild(text_layout, 0, "text_layout");

	Sprite* background = Sprite::create("res/buttons/Challenges/text_holder.png");
	background->setScale(visibleSize.width * 0.20 / background->getContentSize().width);
	text_layout->addChild(background, 0, "background");
	ui::Text* title_text = ui::Text::create("", "fonts/LICABOLD.ttf", visibleSize.width / 50);
	title_text->setTextAreaSize(Size(background->getContentSize().width * background->getScale() * 0.85,
		background->getContentSize().height * background->getScale() * 0.2));
	title_text->setPosition(Vec2(0, background->getContentSize().height *background->getScale() * 0.4 - title_text->getTextAreaSize().height / 2));
	title_text->setColor(Color3B(252, 211, 139));
	//title_text->setAnchorPoint(Vec2(0, 0.5f));
	text_layout->addChild(title_text, 0, "title_text");
	ui::Text* description_text = ui::Text::create("", "fonts/LICABOLD.ttf", visibleSize.width / 64);
	//description_text->setAnchorPoint(Vec2(0, 0.5f));
	description_text->setTextAreaSize(Size(background->getContentSize().width * background->getScale() * 0.85,
		background->getContentSize().height * background->getScale() * 0.5));
	description_text->setPosition(Vec2(title_text->getPosition().x,
		title_text->getPosition().y - title_text->getTextAreaSize().height / 2 - description_text->getTextAreaSize().height / 2));
	description_text->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
	text_layout->addChild(description_text, 0, "description_text");
	ui::Text* progress_text = ui::Text::create("","fonts/LICABOLD.ttf", visibleSize.width / 64);
	//progress_text->setAnchorPoint(Vec2(0, 0.5f));
	progress_text->setTextAreaSize(Size(background->getContentSize().width * background->getScale() * 0.85,
		background->getContentSize().height * background->getScale() * 0.2));
	progress_text->setPosition(Vec2(description_text->getPosition().x,
		description_text->getPosition().y - description_text->getTextAreaSize().height / 2 - progress_text->getTextAreaSize().height / 2));
	text_layout->addChild(progress_text, 0, "progress_text");
	std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();

	// Challenges
	for (unsigned int i(0); i < value.size(); ++i) {	
		Challenge::ChallengeType type = Challenge::getChallengeTypeFromString(value[i]["name"].asString());
		
		challenges.push_back(Challenge::create(
			type,
			value[i]["int_value"].asInt(),
			value[i]["str_value"].asString()));

		// GUI part
		std::string image_name = j_challenges[value[i]["name"].asString()]["image"].asString();
		if (type == Challenge::ChallengeType::Discriminant || type == Challenge::ChallengeType::Narrow) {
			int dot_pos = image_name.find('.');
			std::string extension = image_name.substr(dot_pos + 1,image_name.length());
			image_name = image_name.substr(0, dot_pos) + "_" + value[i]["str_value"].asString() + "." + extension;
		}
		Challenge* c_challenge = challenges.back();

		std::string description = j_challenges[value[i]["name"].asString()]["description_" + language].asString();
		std::string name		= j_challenges[value[i]["name"].asString()]["name_" + language].asString();
		ui::Button* challenge = ui::Button::create(image_name);
		challenge->setScale(size_button / challenge->getContentSize().width);
		challenge->setPosition(Vec2(0,
			-(challenges.size() - 1 / 2.0) * (challenge->getContentSize().height * challenge->getScale() + size_button / 10)));
		challenge->addTouchEventListener([&, challenge, text_layout, name, description, c_challenge](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
			if (type == ui::Widget::TouchEventType::ENDED) {
				Size visibleSize = Director::getInstance()->getVisibleSize();
				ActionInterval* action = DelayTime::create(0.0f);
				if (text_layout->isVisible()) {
					auto action = ScaleTo::create(0.125f, 0.f);
				}
				auto updateText = CallFunc::create([&, challenge, text_layout, name, description, c_challenge]() {
					std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();

					std::string str_progress("");
					if (c_challenge->getState() == Challenge::State::Running) {
						str_progress = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::BUTTON)["progress_running"][language].asString();
						((ui::Text*)text_layout->getChildByName("progress_text"))->setColor(Color3B::WHITE);
					}
					else if (c_challenge->getState() == Challenge::State::Failed) {
						str_progress = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::BUTTON)["progress_failed"][language].asString();
						((ui::Text*)text_layout->getChildByName("progress_text"))->setColor(Color3B::ORANGE);
					}
					else if (c_challenge->getState() == Challenge::State::Succeed) {
						str_progress = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::BUTTON)["progress_succeed"][language].asString();
						((ui::Text*)text_layout->getChildByName("progress_text"))->setColor(Color3B::GREEN);
					}
					std::string n_description = description;
					text_layout->setScale(0);
					if (name != ((ui::Text*)text_layout->getChildByName("title_text"))->getString()) {
						int value_pos = description.find("%i");
						int string_pos = description.find("%s");

						if (value_pos != std::string::npos) {
							char buffer[50];
							sprintf(buffer, n_description.c_str(), c_challenge->getIntValue());
							n_description = std::string(buffer);
						}
						if (string_pos != std::string::npos) {
							char buffer[50];
							sprintf(buffer, n_description.c_str(), c_challenge->getStringValue().c_str());
							n_description = std::string(buffer);
						}
						((ui::Text*)text_layout->getChildByName("title_text"))->setString(name);
						((ui::Text*)text_layout->getChildByName("description_text"))->setString(n_description);
						((ui::Text*)text_layout->getChildByName("progress_text"))->setString(str_progress);
						text_layout->setPosition(Vec2(
							challenge->getPosition().x + challenge->getContentSize().width * challenge->getScale() * 2 / 3 + 
							text_layout->getChildByName("background")->getContentSize().width * text_layout->getChildByName("background")->getScale() / 2,
							challenge->getPosition().y ));

						text_layout->setVisible(true);

						auto scale_to = ScaleTo::create(0.125f, 1.f);
						text_layout->runAction(scale_to);
					}
					else {
						((ui::Text*)text_layout->getChildByName("title_text"))->setString("");
						((ui::Text*)text_layout->getChildByName("description_text"))->setString("");
						((ui::Text*)text_layout->getChildByName("progress_text"))->setString("");
					}
				});
				text_layout->runAction(Sequence::create(action, updateText, nullptr));
			}
		});
		addChild(challenge, 1, Value(type).asString());
	}
}

ChallengeHandler::~ChallengeHandler() {
	for (auto& challenge : challenges) {
		delete challenge;
		challenge = nullptr;
	}
	challenges.empty();
}

void ChallengeHandler::update() {
	for (auto& challenge : challenges) {
		int previous_state = challenge->getState();

		switch (challenge->getChallengeType()) {
		case Challenge::ChallengeType::Untouchable:
			if (challenge->getIntValue() > (int)SceneManager::getInstance()->getGame()->getLevel()->getLife()) {
				challenge->setState(Challenge::State::Failed);
			}
			else {
				challenge->setState(Challenge::State::Running);
			}
			break;
		case Challenge::ChallengeType::Perfectionist:
			for (auto& wall : SceneManager::getInstance()->getGame()->getLevel()->getWalls()) {
				if (wall->isDamaged()) {
					challenge->setState(Challenge::State::Failed);
				}
			}
			break;
		case Challenge::ChallengeType::Stingy:
			if (challenge->getIntValue() < (int)SceneManager::getInstance()->getGame()->getLevel()->getUsedQuantity()) {
				challenge->setState(Challenge::State::Failed);
			}
			else {
				challenge->setState(Challenge::State::Running);
			}
			break;
		case Challenge::ChallengeType::Believer:
			if (challenge->getIntValue() < (int)SceneManager::getInstance()->getGame()->getLevel()->getTowers().size()) {
				challenge->setState(Challenge::State::Failed);
			}
			else {
				challenge->setState(Challenge::State::Running);
			}
			break;
		}
		if (previous_state != challenge->getState()) {
			Sprite* failed = Sprite::create("res/buttons/Challenges/failed.png");
			failed->setScale(getChildByName(Value(challenge->getChallengeType()).asString())->getContentSize().width *
				getChildByName(Value(challenge->getChallengeType()).asString())->getScale() * 0.6 / failed->getContentSize().width);
			failed->setPosition(getChildByName(Value(challenge->getChallengeType()).asString())->getPosition());
			addChild(failed, 2);
		}
	}

	// GUI
}

void ChallengeHandler::addTower(Tower::TowerType type, cocos2d::Vec2 position) {
	for (auto& challenge : challenges) {
		int previous_state = challenge->getState();
		bool touching = false;

		switch (challenge->getChallengeType()) {
		case Challenge::ChallengeType::Misanthrope:
			for (auto& tower : SceneManager::getInstance()->getGame()->getLevel()->getTowers()) {
				if (tower->getPosition().getDistance(position) <= Cell::getCellWidth() && position != tower->getPosition()) {
					challenge->setState(Challenge::State::Failed);
				}
			}
			break;
		case Challenge::ChallengeType::Philanthrope:
			if (SceneManager::getInstance()->getGame()->getLevel()->getTowers().size() <= 1) {
				touching = true;
			}
			for (auto& tower : SceneManager::getInstance()->getGame()->getLevel()->getTowers()) {
				if (tower->getPosition() != position && tower->getPosition().getDistance(position) <= Cell::getCellWidth()) {
					touching = true;
				}
			}
			if (!touching) {
				challenge->setState(Challenge::State::Failed);
			}
			break;
		case Challenge::ChallengeType::Discriminant:
			if (Tower::getTowerTypeFromString(challenge->getStringValue()) == type) {
				challenge->setState(Challenge::State::Failed);
			}
			break;
		case Challenge::ChallengeType::Narrow:
			if (Tower::getTowerTypeFromString(challenge->getStringValue()) != type) {
				challenge->setState(Challenge::State::Failed);
			}
			break;
		case Challenge::ChallengeType::Reckless:
			for (auto& path : SceneManager::getInstance()->getGame()->getLevel()->getPaths()) {
				for (auto& cell : path) {
					double distance = cell->getPosition().getDistance(position);
					if (cell->getPosition().getDistance(position) <= Cell::getCellWidth()) {
						touching = true;
					}
				}
			}
			if (!touching) {
				challenge->setState(Challenge::State::Failed);
			}
			break;
		}
		if (previous_state != challenge->getState()) {
			Sprite* failed = Sprite::create("res/buttons/Challenges/failed.png");
			failed->setScale(getChildByName(Value(challenge->getChallengeType()).asString())->getContentSize().width *
				getChildByName(Value(challenge->getChallengeType()).asString())->getScale() * 0.6 / failed->getContentSize().width);
			failed->setPosition(getChildByName(Value(challenge->getChallengeType()).asString())->getPosition());
			addChild(failed, 2);
		}
	}
}

void ChallengeHandler::upgradeTower(Tower::TowerType type, Vec2 position) {
	for (auto& challenge : challenges) {
		switch (challenge->getChallengeType()) {
		case Challenge::ChallengeType::Barbarian:
			challenge->setState(Challenge::State::Failed);
			break;
		}
	}
}

void ChallengeHandler::endChallengeHandler() {
	update();
	for (auto& challenge : challenges) {
		if (!challenge->hasFailed()) {
			challenge->setState(Challenge::State::Succeed);
			Sprite* succeed = Sprite::create("res/buttons/Challenges/succeed.png");
			succeed->setScale(getChildByName(Value(challenge->getChallengeType()).asString())->getContentSize().width *
				getChildByName(Value(challenge->getChallengeType()).asString())->getScale() * 0.6 / succeed->getContentSize().width);
			succeed->setPosition(getChildByName(Value(challenge->getChallengeType()).asString())->getPosition());
			addChild(succeed, 2);
		}
	}
	Json::Value save = ((AppDelegate*)Application::getInstance())->getSave();
	int worldID = SceneManager::getInstance()->getGame()->getLevel()->getWorldId();
	int levelID = SceneManager::getInstance()->getGame()->getLevel()->getLevelId();
	Json::Value level_challenge = save["levels"][worldID][levelID]["challenges"];
	if (level_challenge.isNull()) {
		save["levels"][worldID][levelID]["challenges"] = countSucceedChallenges();
	}
	else if(!level_challenge.isNull() && level_challenge.asInt() < countSucceedChallenges()){
		save["levels"][worldID][levelID]["challenges"] = countSucceedChallenges();
	}
	((AppDelegate*)Application::getInstance())->getConfigClass()->setSave(save);
	((AppDelegate*)Application::getInstance())->getConfigClass()->save();
}

int ChallengeHandler::countSucceedChallenges() {
	int count(0);
	for (auto& challenge : challenges) {
		if (challenge->getState() == Challenge::State::Succeed) {
			++count;
		}
	}
	return count;
}

void ChallengeHandler::hideDescription() {
	getChildByName("text_layout")->setVisible(false);
	((ui::Text*)getChildByName("text_layout")->getChildByName("title_text"))->setString("");
}


/*
* CHALLENGE CLASS
*/

Challenge* Challenge::create(Challenge::ChallengeType type, int value, std::string string_value) {
	return new Challenge(type, value, string_value);
}
Challenge::Challenge(Challenge::ChallengeType n_type, int n_value, std::string string_value): 
	type(n_type), int_value(n_value), str_value(string_value), state(Challenge::State::Running) {}

Challenge::ChallengeType Challenge::getChallengeTypeFromString(std::string type) {
	if (type == "Untouchable") {
		return Challenge::ChallengeType::Untouchable;
	}
	else if (type == "Perfectionist") {
		return Challenge::ChallengeType::Perfectionist;
	}
	else if (type == "Misanthrope") {
		return Challenge::ChallengeType::Misanthrope;
	}
	else if (type == "Philanthrope") {
		return Challenge::ChallengeType::Philanthrope;
	}
	else if (type == "Stingy") {
		return Challenge::ChallengeType::Stingy;
	}
	else if (type == "Believer") {
		return Challenge::ChallengeType::Believer;
	}
	else if (type == "Discriminant") {
		return Challenge::ChallengeType::Discriminant;
	}
	else if (type == "Narrow") {
		return Challenge::ChallengeType::Narrow;
	}
	else if (type == "Reckless") {
		return Challenge::ChallengeType::Reckless;
	}
	else if (type == "Barbarian") {
		return Challenge::ChallengeType::Barbarian;
	}
	else {
		log("error: wrong challenge type %s", type.c_str());
		return Challenge::ChallengeType::Barbarian;
	}
}
Challenge::ChallengeType Challenge::getChallengeType() {
	return type;
}

int Challenge::getIntValue() {
	return int_value;
}

std::string Challenge::getStringValue() {
	return str_value;
}

bool Challenge::hasFailed() {
	return state == Challenge::State::Failed;
}

void Challenge::setState(Challenge::State n_state) {
	state = n_state;
}

Challenge::State Challenge::getState() {
	return state;
}

