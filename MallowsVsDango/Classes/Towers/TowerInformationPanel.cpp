#include "TowerInformationPanel.h"
#include "Tower.h"
#include "../Scenes/Mygame.h"
#include "../AppDelegate.h"

TowerInformationPanel * TowerInformationPanel::create(MyGame* game, Tower * tower, Config* config)
{
	TowerInformationPanel* panel = new TowerInformationPanel();
	if (panel->init(game, tower, config)) {
		return panel;
	}
	delete panel;
	return nullptr;
}

bool TowerInformationPanel::init(MyGame* cGame, Tower * cTower, Config* cConfig)
{
	game = cGame;
	tower = cTower;
	configClass = cConfig;
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	std::string language = configClass->getSettings()->getLanguage();
	const auto config = configClass->getConfigValues(Config::ConfigType::BUTTON);
	const auto spec_config = tower->getSpecConfig();
	mainPanel = cocos2d::ui::Button::create("res/buttons/centralMenuPanel2.png");
	mainPanel->setScaleX(visibleSize.width / 4 / mainPanel->getContentSize().width);
	mainPanel->setScaleY(visibleSize.width * 0.25 / mainPanel->getContentSize().width);
	mainPanel->setZoomScale(0);
	addChild(mainPanel, 0);
	spriteSize = mainPanel->getContentSize().width * mainPanel->getScaleX() / 10;

	createCurrentLevelPanel();
	createNextLevelPanel();
	createLockLayout();
	createDescriptionLayout();
	

	

	auto nextlevel_button = cocos2d::ui::Button::create("res/buttons/upgrade.png");
	nextlevel_button->setScaleX(mainPanel->getContentSize().width * mainPanel->getScaleX() * .95f /
		nextlevel_button->getContentSize().width * 0.55f);
	nextlevel_button->setScaleY(mainPanel->getContentSize().height * mainPanel->getScaleY() /
		nextlevel_button->getContentSize().height * 0.85);
	nextlevel_button->setPosition(cocos2d::Vec2(mainPanel->getContentSize().width * mainPanel->getScaleX() / 6,
		0));
	addChild(nextlevel_button, 1, "next_level_button");
	nextlevel_button->addTouchEventListener([&](Ref *sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			std::string language = configClass->getSettings()->getLanguage();

			auto cost_size = tower->getCosts().size();
			if ((int)game->getLevel()->getQuantity() >= tower->getCosts()[tower->getLevel() + 1] &&
				tower->getLevel() < (int)cost_size)
			{
				Json::Value action;
				action["tower_name"] = tower->getName();
				action["time"] = (int)time(0);
				cocos2d::Vec2 turret_position = game->getLevel()->getNearestPositionInGrid(getPosition());
				action["position"]["x"] = turret_position.x;
				action["position"]["y"] = turret_position.y;
				action["action"] = "upgrade_tower";
				game->addActionToTracker(action);
				game->getLevel()->decreaseQuantity(tower->getCosts()[tower->getLevel() + 1]);
				tower->upgradeCallback(sender);

				if ((int)game->getLevel()->getQuantity() < tower->getCosts()[tower->getLevel() + 1]) {
					((cocos2d::ui::Button*)getChildByName("next_level_button"))->setEnabled(false);
					nextCostLabel->setColor(cocos2d::Color3B::RED);
				}
				else {
					((cocos2d::ui::Button*)getChildByName("next_level_button"))->setEnabled(true);
					nextCostLabel->setColor(cocos2d::Color3B::YELLOW);
				}
				nextLevelLabel->setString(configClass->getConfigValues(Config::ConfigType::BUTTON)
						["level"][language].asString() + " " + Json::Value(tower->getLevel() + 2).asString());
				if (tower->getLevel() < (int)cost_size - 2) {
					std::string s("");
					s = Json::Value(tower->getDamages()[tower->getLevel() + 1]).asString();
					s.resize(4);
					nextAttackLabel->setString(s);
					s = Json::Value(tower->getAttackSpeeds()[tower->getLevel() + 1]).asString();
					s.resize(4);
					nextSpeedLabel->setString(s);
					s = Json::Value(round(tower->getRanges()[tower->getLevel() + 1] / Cell::getCellWidth() * 100) / 100).asString();
					s.resize(4);
					nextRangeLabel->setString(s);
					nextCostLabel->setString(Json::Value(tower->getCosts()[tower->getLevel() + 1]).asString());
				}
				else if (tower->getLevel() == (int)cost_size - 2) {
					nextCostLabel->setString(Json::Value(tower->getCosts()[tower->getLevel() + 1]).asString());
					nextLevelInfos->setVisible(false);
					descriptionLayout->setVisible(true);
				}
				else {
					getChildByName("next_level_layout")->setVisible(false);
					((cocos2d::ui::Button*)getChildByName("next_level_button"))->setEnabled(false);
					getChildByName("max_level_label")->setVisible(true);
				}
				if (tower->getLevel() >= tower->getMaxLevel() && tower->getLevel() < (int)cost_size - 1) {
					nextLevelInfos->setVisible(false);
					descriptionLayout->setVisible(false);
					lockedLayout->setVisible(false);
					((cocos2d::ui::Button*)getChildByName("next_level_button"))->setEnabled(false);
				}
				std::string s("");
				currentLevelLabel->setString(configClass->getConfigValues(Config::ConfigType::BUTTON)
						["level"][language].asString() + " " + Json::Value(tower->getLevel() + 1).asString());
				s = Json::Value(tower->getDamages()[tower->getLevel()]).asString();
				s.resize(4);
				currentAttackLabel->setString(s);
				s = Json::Value(tower->getAttackSpeeds()[tower->getLevel()]).asString();
				s.resize(4);
				currentSpeedLabel->setString(s);
				s = Json::Value(round(tower->getRanges()[tower->getLevel()] / Cell::getCellWidth() * 100) / 100).asString();
				s.resize(4);
				currentRangeLabel->setString(s);
				((cocos2d::Label*)getChildByName("sell_label"))->setString(Json::Value(tower->getSells()[tower->getLevel()]).asString());
			}
		}
	});

	cocos2d::Label* max_level_label = cocos2d::Label::createWithTTF("Max. Level Reached", "fonts/LICABOLD.ttf", 35 * visibleSize.width / 1280);
	max_level_label->setColor(cocos2d::Color3B::BLACK);
	max_level_label->setAlignment(cocos2d::TextHAlignment::CENTER);
	max_level_label->setDimensions(nextLevelInfos->getContentSize().width, nextLevelInfos->getContentSize().height * 0.6);
	max_level_label->setAnchorPoint(cocos2d::Vec2(0, 0.5));
	max_level_label->setPosition(0, 0);
	addChild(max_level_label, 3, "max_level_label");

	if (tower->getLevel() < tower->getMaxLevel()) {
		lockedLayout->setVisible(false);
		if (tower->getLevel() < (int)tower->getDamages().size()) {
			max_level_label->setVisible(false);
			nextLevelInfos->setVisible(true);
			descriptionLayout->setVisible(false);
		}
		else {
			nextLevelInfos->setVisible(false);
			nextlevel_button->setEnabled(false);
		}
		if (tower->getLevel() == tower->getDamages().size() - 2) {
			nextLevelInfos->setVisible(false);
			descriptionLayout->setVisible(true);
		}
		MyGame* game = SceneManager::getInstance()->getGame();
		if ((int)game->getLevel()->getQuantity() < tower->getCosts()[tower->getLevel() + 1]) {
			nextlevel_button->setEnabled(false);
		}
	}
	else {
		max_level_label->setVisible(false);
		nextLevelInfos->setVisible(false);
		descriptionLayout->setVisible(false);
		lockedLayout->setVisible(true);
		nextlevel_button->setEnabled(false);
	}
	auto sell = cocos2d::ui::Button::create("res/buttons/sell.png");
	sell->setScaleX(mainPanel->getContentSize().width * mainPanel->getScaleX() / sell->getContentSize().width * 0.3);
	sell->setScaleY(mainPanel->getContentSize().height * mainPanel->getScaleY() / sell->getContentSize().height * 0.3);
	sell->setPosition(cocos2d::Vec2(-mainPanel->getContentSize().width * mainPanel->getScaleX() * 0.3,
		currentLevelInfos->getPosition().y + currentRangeLabel->getPosition().y - spriteSize / 2 -
		sell->getContentSize().height* sell->getScaleY() / 2));
	sell->addTouchEventListener([&](Ref *sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			MyGame* game = SceneManager::getInstance()->getGame();
			game->getLevel()->increaseQuantity(tower->getSells()[tower->getLevel()]);
			tower->destroyCallback(sender);
			SceneManager::getInstance()->getGame()->getMenu()->hideTowerInfo();
			Json::Value action;
			action["tower_name"] = tower->getName();
			action["time"] = (int)time(0);
			cocos2d::Vec2 turret_position = game->getLevel()->getNearestPositionInGrid(getPosition());
			action["position"]["x"] = turret_position.x;
			action["position"]["y"] = turret_position.y;
			action["action"] = "sell_tower";
			game->addActionToTracker(action);
		}
	});
	addChild(sell, 1);

	auto sell_label = cocos2d::Label::createWithTTF(config["sell"][language].asString(),
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	sell_label->enableOutline(cocos2d::Color4B::BLACK, 2);
	sell_label->setAlignment(cocos2d::TextHAlignment::CENTER);
	sell_label->setPosition(sell->getPosition().x, sell->getPosition().y + sell_label->getContentSize().height / 2);
	addChild(sell_label, 1);

	auto sell_cost = cocos2d::Label::createWithTTF(Json::Value(tower->getCosts()[tower->getLevel()]).asString(),
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	sell_cost->enableOutline(cocos2d::Color4B::BLACK, 2);
	sell_cost->setAlignment(cocos2d::TextHAlignment::CENTER);
	sell_cost->setPosition(sell->getPosition().x + sell_cost->getContentSize().width / 2, sell_label->getPosition().y -
		sell_label->getContentSize().height / 2 - spriteSize / 2);
	addChild(sell_cost, 1, "sell_label");

	cocos2d::Sprite* cost_sugar = cocos2d::Sprite::create("res/buttons/sugar.png");
	cost_sugar->setScale(spriteSize / cost_sugar->getContentSize().width);
	cost_sugar->setPosition(sell_cost->getPosition().x - spriteSize,
		sell_cost->getPosition().y);
	addChild(cost_sugar, 1);
	return true;
}

void TowerInformationPanel::createCurrentLevelPanel() {
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	const auto config = configClass->getConfigValues(Config::ConfigType::BUTTON);
	std::string language = configClass->getSettings()->getLanguage();

	currentLevelInfos = cocos2d::ui::Layout::create();
	currentLevelInfos->setContentSize(cocos2d::Size(mainPanel->getContentSize().width * mainPanel->getScaleX() / 3,
		mainPanel->getContentSize().height * mainPanel->getScaleY() * 3 / 4));
	currentLevelInfos->setPosition(cocos2d::Vec2(-mainPanel->getContentSize().width * mainPanel->getScaleX() * 0.3,
		currentLevelInfos->getContentSize().height / 2));

	currentLevelLabel = cocos2d::Label::createWithTTF(config["level"][language].asString() + " " + Json::Value(tower->getLevel() + 1).asString(),
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	currentLevelLabel->setColor(cocos2d::Color3B::BLACK);
	currentLevelLabel->setPosition(cocos2d::Vec2(0,
		-currentLevelLabel->getContentSize().height / 2));
	currentLevelInfos->addChild(currentLevelLabel, 1, "currentLevelLabel");

	cocos2d::Sprite* attack = cocos2d::Sprite::create("res/buttons/attack.png");
	cocos2d::Sprite* speed = cocos2d::Sprite::create("res/buttons/speed.png");
	cocos2d::Sprite* range_i = cocos2d::Sprite::create("res/buttons/range.png");
	currentLevelInfos->addChild(attack, 1, "attack");
	currentLevelInfos->addChild(speed, 1, "speed");
	currentLevelInfos->addChild(range_i, 1, "range");

	attack->setScale(spriteSize / attack->getContentSize().width);
	speed->setScale(spriteSize / speed->getContentSize().width);
	range_i->setScale(spriteSize / range_i->getContentSize().width);

	attack->setPosition(-spriteSize / 2, currentLevelLabel->getPosition().y -
		currentLevelLabel->getContentSize().height / 2
		- attack->getContentSize().height*attack->getScaleY() / 2);
	speed->setPosition(-spriteSize / 2, attack->getPosition().y - attack->getContentSize().height*
		attack->getScaleY() / 2 - speed->getContentSize().height * speed->getScaleY() * 3 / 4);
	range_i->setPosition(-spriteSize / 2, speed->getPosition().y - speed->getContentSize().height*
		speed->getScaleY() / 2 - range_i->getContentSize().height * range_i->getScaleY() * 3 / 4);

	std::string s = Json::Value(tower->getDamage()).asString();
	s.resize(4);
	currentAttackLabel = cocos2d::Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	currentAttackLabel->setColor(cocos2d::Color3B::BLACK);
	currentAttackLabel->setPosition(cocos2d::Vec2(attack->getPosition().x + spriteSize,
		attack->getPosition().y));
	currentAttackLabel->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	currentLevelInfos->addChild(currentAttackLabel, 1, "currentAttackLabel");

	s = Json::Value(round(tower->getRange() / Cell::getCellWidth() * 100) / 100).asString();
	s.resize(4);
	currentRangeLabel = cocos2d::Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	currentRangeLabel->setColor(cocos2d::Color3B::BLACK);
	currentRangeLabel->setPosition(cocos2d::Vec2(range_i->getPosition().x + spriteSize,
		range_i->getPosition().y));
	currentRangeLabel->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	currentLevelInfos->addChild(currentRangeLabel, 1, "currentRangeLabel");

	s = Json::Value(tower->getAttackSpeed()).asString();
	s.resize(4);
	currentSpeedLabel = cocos2d::Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	currentSpeedLabel->setColor(cocos2d::Color3B::BLACK);
	currentSpeedLabel->setPosition(cocos2d::Vec2(speed->getPosition().x + spriteSize,
		speed->getPosition().y));
	currentSpeedLabel->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	currentLevelInfos->addChild(currentSpeedLabel, 1, "currentSpeedLabel");

	double position_x = getPosition().x;
	double position_y = getPosition().y + mainPanel->getContentSize().height * mainPanel->getScaleY() / 2 +
		Cell::getCellHeight() / 2;
	if (position_x >= visibleSize.width * 3 / 8) {
		position_x = mainPanel->getContentSize().width * mainPanel->getScaleX() / 2;
	}
	else if (position_x < visibleSize.width * 3 / 8) {
		position_x = visibleSize.width * 3 / 4 - mainPanel->getContentSize().width * mainPanel->getScaleX() / 2;
	}
	position_y = mainPanel->getContentSize().height * mainPanel->getScaleY() / 2;
	setPosition(cocos2d::Vec2(position_x, position_y));
	addChild(currentLevelInfos, 2, "currentLevelInfos");
}

void TowerInformationPanel::createNextLevelPanel() {
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	std::string language = configClass->getSettings()->getLanguage();
	const auto config = configClass->getConfigValues(Config::ConfigType::BUTTON);
	const auto spec_config = tower->getSpecConfig();

	nextLevelInfos = cocos2d::ui::Layout::create();
	nextLevelInfos->setContentSize(cocos2d::Size(mainPanel->getContentSize().width * mainPanel->getScaleX() / 3,
		mainPanel->getContentSize().height * mainPanel->getScaleY() * 3 / 4));
	nextLevelInfos->setPosition(cocos2d::Vec2(0, nextLevelInfos->getContentSize().height / 2));

	nextLevelLabel = cocos2d::Label::createWithTTF(config["level"][language].asString() + " " + Json::Value(tower->getLevel() + 2).asString(),
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	nextLevelLabel->setColor(cocos2d::Color3B::BLACK);
	nextLevelLabel->setPosition(cocos2d::Vec2(nextLevelInfos->getContentSize().width * nextLevelInfos->getScaleX() / 2,
		-nextLevelLabel->getContentSize().height / 2));
	nextLevelInfos->addChild(nextLevelLabel, 1, "currentLevelLabel");

	cocos2d::Sprite* attack_n = cocos2d::Sprite::create("res/buttons/attack.png");
	cocos2d::Sprite* speed_n = cocos2d::Sprite::create("res/buttons/speed.png");
	cocos2d::Sprite* range_ni = cocos2d::Sprite::create("res/buttons/range.png");
	cocos2d::Sprite* cost = cocos2d::Sprite::create("res/buttons/sugar.png");

	nextLevelInfos->addChild(attack_n, 1, "attack");
	nextLevelInfos->addChild(speed_n, 1, "speed");
	nextLevelInfos->addChild(range_ni, 1, "range");
	nextLevelInfos->addChild(cost, 1, "cost");

	attack_n->setScale(spriteSize / attack_n->getContentSize().width);
	speed_n->setScale(spriteSize / speed_n->getContentSize().width);
	range_ni->setScale(spriteSize / range_ni->getContentSize().width);
	cost->setScale(spriteSize / cost->getContentSize().width);

	attack_n->setPosition(spriteSize, nextLevelLabel->getPosition().y -
		nextLevelLabel->getContentSize().height / 2 - spriteSize / 2);
	speed_n->setPosition(spriteSize, attack_n->getPosition().y - spriteSize / 2 - spriteSize / 2);
	range_ni->setPosition(spriteSize, speed_n->getPosition().y - spriteSize / 2 - spriteSize / 2);



	auto upgrade_label = cocos2d::Label::createWithTTF(config["upgrade"][language].asString(),
		"fonts/LICABOLD.ttf", 30 * visibleSize.width / 1280);
	upgrade_label->setColor(cocos2d::Color3B::YELLOW);
	upgrade_label->enableOutline(cocos2d::Color4B::BLACK, 1);
	upgrade_label->setAlignment(cocos2d::TextHAlignment::CENTER);
	upgrade_label->setPosition(nextLevelInfos->getContentSize().width * nextLevelInfos->getScaleX() / 2,
		range_ni->getPosition().y - range_ni->getContentSize().height * range_ni->getScaleY() / 2 -
		upgrade_label->getContentSize().height / 2);
	nextLevelInfos->addChild(upgrade_label, 1, "upgrade");

	cost->setPosition(spriteSize, upgrade_label->getPosition().y - upgrade_label->getContentSize().height / 2 - spriteSize / 2);

	std::string s = Json::Value(tower->getDamages()[tower->getLevel() + 1]).asString();
	s.resize(4);
	nextAttackLabel = cocos2d::Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	nextAttackLabel->setColor(cocos2d::Color3B::BLACK);
	nextAttackLabel->setPosition(cocos2d::Vec2(attack_n->getPosition().x + attack_n->getContentSize().width * attack_n->getScale(),
		attack_n->getPosition().y));
	nextAttackLabel->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	nextLevelInfos->addChild(nextAttackLabel, 1, "currentAttackLabel");

	s = Json::Value(round(tower->getRanges()[tower->getLevel() + 1] / Cell::getCellWidth() * 100) / 100).asString();
	s.resize(4);
	nextRangeLabel = cocos2d::Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	nextRangeLabel->setColor(cocos2d::Color3B::BLACK);
	nextRangeLabel->setPosition(cocos2d::Vec2(range_ni->getPosition().x + attack_n->getContentSize().width * attack_n->getScale(),
		range_ni->getPosition().y));
	nextRangeLabel->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	nextLevelInfos->addChild(nextRangeLabel, 1, "currentRangeLabel");

	s = Json::Value(tower->getAttackSpeeds()[tower->getLevel() + 1]).asString();
	s.resize(4);
	nextSpeedLabel = cocos2d::Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	nextSpeedLabel->setColor(cocos2d::Color3B::BLACK);
	nextSpeedLabel->setPosition(cocos2d::Vec2(speed_n->getPosition().x + attack_n->getContentSize().width * attack_n->getScale(),
		speed_n->getPosition().y));
	nextSpeedLabel->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	nextLevelInfos->addChild(nextSpeedLabel, 1, "currentSpeedLabel");

	s = Json::Value(tower->getCosts()[tower->getLevel() + 1]).asString();
	s.resize(4);
	nextCostLabel = cocos2d::Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	nextCostLabel->setColor(cocos2d::Color3B::YELLOW);
	nextCostLabel->enableOutline(cocos2d::Color4B::BLACK, 1);
	nextCostLabel->setPosition(cocos2d::Vec2(cost->getPosition().x + attack_n->getContentSize().width * attack_n->getScale(),
		cost->getPosition().y));
	nextCostLabel->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	nextLevelInfos->addChild(nextCostLabel, 1, "cost_label");

	addChild(nextLevelInfos, 2, "next_level_layout");
}

void TowerInformationPanel::createLockLayout() {
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize(); 
	std::string language = configClass->getSettings()->getLanguage();
	const auto config = configClass->getConfigValues(Config::ConfigType::BUTTON);

	//lockedLayout = cocos2d::LayerColor::create(cocos2d::Color4B(255, 255, 255, 255));
	lockedLayout = cocos2d::LayerColor::create();
	lockedLayout->setContentSize(cocos2d::Size(mainPanel->getContentSize().width * mainPanel->getScaleX() / 3,
		mainPanel->getContentSize().height * mainPanel->getScaleY() * 3 / 4));
	lockedLayout->setPosition(0, -lockedLayout->getContentSize().height / 2);
	std::string s = Json::Value(tower->getXPLevels()[tower->getLevel() + 1]).asString();
	int dot_pos = s.find('.');
	s = s.substr(0, dot_pos);
	cocos2d::Label* locked_label = cocos2d::Label::createWithTTF(config["locked"][language].asString() + "\n" + Json::Value(tower->getCurrentXP()).asString() + "/" +
		s,
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	locked_label->setColor(cocos2d::Color3B::BLACK);
	locked_label->setPosition(nextLevelLabel->getPosition() + cocos2d::Vec2(0, -nextLevelLabel->getContentSize().width / 2
		- locked_label->getContentSize().height / 2));
	locked_label->setDimensions(lockedLayout->getContentSize().width, lockedLayout->getContentSize().height);
	locked_label->setPosition(lockedLayout->getContentSize().width / 2, lockedLayout->getContentSize().height / 2);
	locked_label->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
	locked_label->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
	lockedLayout->addChild(locked_label, 1, "locked_label");

	addChild(lockedLayout, 2, "lockedLayout");
}

void TowerInformationPanel::createDescriptionLayout() {
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	std::string language = configClass->getSettings()->getLanguage();
	const auto config = configClass->getConfigValues(Config::ConfigType::BUTTON);
	const auto spec_config = tower->getSpecConfig();

	descriptionLayout = cocos2d::LayerColor::create();
	descriptionLayout->setContentSize(cocos2d::Size(mainPanel->getContentSize().width * mainPanel->getScaleX() / 3,
		mainPanel->getContentSize().height * mainPanel->getScaleY() * 3 / 4));
	descriptionLayout->setPosition(cocos2d::Vec2(0, descriptionLayout->getContentSize().height / 2));
	auto description_label = cocos2d::Label::createWithTTF(spec_config["last_level_description_" +
		configClass->getSettings()->getLanguage()].asString(),
		"fonts/LICABOLD.ttf", 20 * visibleSize.width / 1280);
	description_label->setColor(cocos2d::Color3B::BLACK);
	description_label->setAlignment(cocos2d::TextHAlignment::CENTER);
	description_label->setDimensions(nextLevelInfos->getContentSize().width * 2 * 0.85,
		nextLevelInfos->getContentSize().height * 3 / 4);
	description_label->setPosition(nextLevelInfos->getContentSize().width * nextLevelInfos->getScaleX() / 2,
		descriptionLayout->getPosition().y - descriptionLayout->getContentSize().height / 2 -
		description_label->getContentSize().height / 2);
	descriptionLayout->addChild(description_label, 1, "description_label");
	addChild(descriptionLayout, 2, "descriptionLayout");
}

void TowerInformationPanel::update() {
	const auto config = tower->getSpecConfig();
	if (tower->getLevel() < tower->getMaxLevel()) {
		if (lockedLayout->isVisible()) {
			lockedLayout->setVisible(false);

			if (tower->getLevel() < (int)config["damages"].size()) {

				nextLevelInfos->setVisible(true);
				descriptionLayout->setVisible(false);
			}
			else {
				nextLevelInfos->setVisible(false);
				((cocos2d::ui::Button*)getChildByName("next_level_button"))->setEnabled(false);
			}
			if (tower->getLevel() == (int)config["damages"].size() - 2) {
				nextLevelInfos->setVisible(false);
				descriptionLayout->setVisible(true);
			}
		}
		if ((int)game->getLevel()->getQuantity() < config["cost"][tower->getLevel() + 1].asInt()) {
			((cocos2d::ui::Button*)getChildByName("next_level_button"))->setEnabled(false);
			nextCostLabel->setColor(cocos2d::Color3B::RED);
		}
		else {
			if (tower->getLevel() + 1 < (int)config["cost"].size() && !((cocos2d::ui::Button*)getChildByName("next_level_button"))->isEnabled()) {
				((cocos2d::ui::Button*)getChildByName("next_level_button"))->setEnabled(true);
			}
			else if (tower->getLevel() + 1 >= (int)config["cost"].size() && (((cocos2d::ui::Button*)getChildByName("next_level_button"))->isEnabled()
				|| nextLevelInfos->isVisible())) {
				((cocos2d::ui::Button*)getChildByName("next_level_button"))->setEnabled(false);
				nextLevelInfos->setVisible(false);
				getChildByName("max_level_label")->setVisible(true);
			}
			nextCostLabel->setColor(cocos2d::Color3B::YELLOW);
		}
	}
	else {
		if (!lockedLayout->isVisible()) {
			lockedLayout->setVisible(true);
			((cocos2d::ui::Button*)getChildByName("next_level_button"))->setEnabled(false);
		}
		std::string language = configClass->getSettings()->getLanguage();
		std::string s = Json::Value(tower->getXPLevels()[tower->getLevel() + 1]).asString();
		int dot_pos = s.find('.');
		s = s.substr(0, dot_pos);
		std::string lockedWord = configClass->getConfigValues(Config::ConfigType::BUTTON)["locked"][language].asString();
		if (((cocos2d::Label*)lockedLayout->getChildByName("locked_label"))->getString() !=
			lockedWord + "\n" + Json::Value(tower->getCurrentXP()).asString() + "/" + s) {
			((cocos2d::Label*)lockedLayout->getChildByName("locked_label"))->setString(
				lockedWord + "\n" + Json::Value(tower->getCurrentXP()).asString() + "/" + s);
		}
	}
}