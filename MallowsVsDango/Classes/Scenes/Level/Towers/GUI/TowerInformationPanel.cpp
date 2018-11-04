#include "TowerInformationPanel.h"
#include "../Tower.h"
#include "../../Mygame.h"
#include "../../../../SceneManager.h"
#include "../../Level.h"
#include "../../Interface/LevelInterface.h"

TowerInformationPanel::~TowerInformationPanel()
{
	delete currentLevelInfos;
	delete nextLevelInfos;
}

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
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	game = cGame;
	level = cGame->getLevel();
	menu = cGame->getMenu();
	tower = cTower;
	configClass = cConfig;
	settings = tower->getTowerSettings();

	createMainPanel(visibleSize);
	createCurrentLevelPanel(visibleSize);
	createNextLevelPanel(visibleSize);
	createLockLayout(visibleSize);
	createDescriptionLayout(visibleSize);
	createNextLevelButton(visibleSize);
	createSellButton(visibleSize);
	createMaxLevelLabel(visibleSize);

	setPanelPosition(visibleSize);
	return true;
}

void TowerInformationPanel::setTower(Tower* newTower) {
	tower = newTower;
	currentLevelInfos->setTower(tower);
	nextLevelInfos->setTower(tower);
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	setPanelPosition(visibleSize);
}

void TowerInformationPanel::createMainPanel(cocos2d::Size &visibleSize) {
	mainPanel = cocos2d::ui::Button::create("res/buttons/centralMenuPanel2.png");
	mainPanel->setScaleX(visibleSize.width / 4 / mainPanel->getContentSize().width);
	mainPanel->setScaleY(visibleSize.width * 0.25 / mainPanel->getContentSize().width);
	mainPanel->setZoomScale(0);
	addChild(mainPanel, 0);
	spriteWidth = mainPanel->getContentSize().width * mainPanel->getScaleX() / 10;
}

void TowerInformationPanel::createCurrentLevelPanel(cocos2d::Size &visibleSize) {
	const auto config = configClass->getConfigValues(Config::ConfigType::BUTTON);
	std::string language = configClass->getSettings()->getLanguage();

	currentLevelInfos = CurrentLevelPanel::create(configClass, spriteWidth, tower);
	currentLevelInfos->setContentSize(cocos2d::Size(mainPanel->getContentSize().width * mainPanel->getScaleX() / 3,
		mainPanel->getContentSize().height * mainPanel->getScaleY() * 3 / 4));
	currentLevelInfos->setPosition(cocos2d::Vec2(-mainPanel->getContentSize().width * mainPanel->getScaleX() * 0.3,
		currentLevelInfos->getContentSize().height / 2));

	addChild(currentLevelInfos, 2, "currentLevelInfos");
}

void TowerInformationPanel::setPanelPosition(cocos2d::Size &visibleSize)
{
	double position_x = tower->getPosition().x;
	double position_y = tower->getPosition().y + mainPanel->getContentSize().height * mainPanel->getScaleY() / 2 +
		Cell::getCellHeight() / 2;
	if (position_x >= visibleSize.width * 3 / 8) {
		position_x = mainPanel->getContentSize().width * mainPanel->getScaleX() / 2;
	}
	else if (position_x < visibleSize.width * 3 / 8) {
		position_x = visibleSize.width * 3 / 4 - mainPanel->getContentSize().width * mainPanel->getScaleX() / 2;
	}
	position_y = mainPanel->getContentSize().height * mainPanel->getScaleY() / 2;
	setPosition(cocos2d::Vec2(position_x, position_y));
}

void TowerInformationPanel::createNextLevelPanel(cocos2d::Size &visibleSize) {
	std::string language = configClass->getSettings()->getLanguage();
	const auto config = configClass->getConfigValues(Config::ConfigType::BUTTON);
	const auto spec_config = tower->getSpecConfig();
	cocos2d::Size nextLevelSize = cocos2d::Size(mainPanel->getContentSize().width * mainPanel->getScaleX() / 3,
		mainPanel->getContentSize().height * mainPanel->getScaleY() * 3 / 4);
	nextLevelInfos = NextLevelPanel::create(configClass, spriteWidth, tower, nextLevelSize);
	nextLevelInfos->setPosition(cocos2d::Vec2(0, nextLevelInfos->getContentSize().height / 2));

	addChild(nextLevelInfos, 2, "next_level_layout");
}

void TowerInformationPanel::createLockLayout(cocos2d::Size &visibleSize) {
	std::string language = configClass->getSettings()->getLanguage();
	const auto config = configClass->getConfigValues(Config::ConfigType::BUTTON);

	lockedLayout = cocos2d::LayerColor::create();
	lockedLayout->setContentSize(cocos2d::Size(mainPanel->getContentSize().width * mainPanel->getScaleX() / 3,
		mainPanel->getContentSize().height * mainPanel->getScaleY() * 3 / 4));
	lockedLayout->setPosition(0, -lockedLayout->getContentSize().height / 2);
	std::string s = Json::Value(settings->getXP(tower->getLevel() + 1)).asString();
	int dot_pos = s.find('.');
	s = s.substr(0, dot_pos);
	cocos2d::Label* locked_label = cocos2d::Label::createWithTTF(config["locked"][language].asString() + "\n" + Json::Value(tower->getCurrentXP()).asString() + "/" +
		s,
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	locked_label->setColor(cocos2d::Color3B::BLACK);
	locked_label->setPosition(nextLevelInfos->getLevelLabelPosition() + cocos2d::Vec2(0, -nextLevelInfos->getLevelLabelSize().width / 2
		- locked_label->getContentSize().height / 2));
	locked_label->setDimensions(lockedLayout->getContentSize().width, lockedLayout->getContentSize().height);
	locked_label->setPosition(lockedLayout->getContentSize().width / 2, lockedLayout->getContentSize().height / 2);
	locked_label->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
	locked_label->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
	lockedLayout->addChild(locked_label, 1, "locked_label");

	addChild(lockedLayout, 2, "lockedLayout");
}

void TowerInformationPanel::createDescriptionLayout(cocos2d::Size &visibleSize) {
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

void TowerInformationPanel::createNextLevelButton(cocos2d::Size &visibleSize) {
	nextLevelButton = cocos2d::ui::Button::create("res/buttons/upgrade.png");
	nextLevelButton->setScaleX(mainPanel->getContentSize().width * mainPanel->getScaleX() * .95f /
		nextLevelButton->getContentSize().width * 0.55f);
	nextLevelButton->setScaleY(mainPanel->getContentSize().height * mainPanel->getScaleY() /
		nextLevelButton->getContentSize().height * 0.85);
	nextLevelButton->setPosition(cocos2d::Vec2(mainPanel->getContentSize().width * mainPanel->getScaleX() / 6,
		0));
	addChild(nextLevelButton, 1, "next_level_button");
	nextLevelButton->addTouchEventListener([&](Ref *sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			std::string language = configClass->getSettings()->getLanguage();
			unsigned int towerLevel = tower->getLevel();
			if ((int)level->getQuantity() >= settings->getCost(towerLevel + 1) &&
				towerLevel < settings->getMaxExistingLevel())
			{
				addTowerActionToTracker("upgrade_tower");
				level->decreaseQuantity(settings->getCost(towerLevel + 1));
				tower->upgradeCallback(sender);

				currentLevelInfos->updateLabel();
				nextLevelInfos->updateLabel();
				nextLevelInfos->updateCost((int)level->getQuantity());
				updateDisplayNextLevel();
				((cocos2d::Label*)getChildByName("sell_label"))->setString(Json::Value(settings->getSell(towerLevel)).asString());
			}
		}
	});
}

void TowerInformationPanel::addTowerActionToTracker(std::string towerAction)
{
	Json::Value action;
	action["tower_name"] = tower->getName();
	action["time"] = (int)time(0);
	cocos2d::Vec2 turret_position = level->getNearestPositionInGrid(getPosition());
	action["position"]["x"] = turret_position.x;
	action["position"]["y"] = turret_position.y;
	action["action"] = towerAction;
	game->addActionToTracker(action);
}

void TowerInformationPanel::createSellButton(cocos2d::Size &visibleSize) {
	std::string language = configClass->getSettings()->getLanguage();
	const auto config = configClass->getConfigValues(Config::ConfigType::BUTTON);

	auto sell = cocos2d::ui::Button::create("res/buttons/sell.png");
	sell->setScaleX(mainPanel->getContentSize().width * mainPanel->getScaleX() / sell->getContentSize().width * 0.3);
	sell->setScaleY(mainPanel->getContentSize().height * mainPanel->getScaleY() / sell->getContentSize().height * 0.3);
	sell->setPosition(cocos2d::Vec2(-mainPanel->getContentSize().width * mainPanel->getScaleX() * 0.3,
		currentLevelInfos->getPosition().y + currentLevelInfos->getRangePosition().y - spriteWidth / 2 -
		sell->getContentSize().height* sell->getScaleY() / 2));
	sell->addTouchEventListener([&](Ref *sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			level->increaseQuantity(settings->getSell(tower->getLevel()));
			tower->destroyCallback(sender);
			menu->removeTower();
			menu->hideTowerInfo();
			addTowerActionToTracker("sell_tower");
		}
	});
	addChild(sell, 1);

	auto sell_label = cocos2d::Label::createWithTTF(config["sell"][language].asString(),
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	sell_label->enableOutline(cocos2d::Color4B::BLACK, 2);
	sell_label->setAlignment(cocos2d::TextHAlignment::CENTER);
	sell_label->setPosition(sell->getPosition().x, sell->getPosition().y + sell_label->getContentSize().height / 2);
	addChild(sell_label, 1);

	auto sell_cost = cocos2d::Label::createWithTTF(Json::Value(settings->getCost(tower->getLevel())).asString(),
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	sell_cost->enableOutline(cocos2d::Color4B::BLACK, 2);
	sell_cost->setAlignment(cocos2d::TextHAlignment::CENTER);
	sell_cost->setPosition(sell->getPosition().x + sell_cost->getContentSize().width / 2, sell_label->getPosition().y -
		sell_label->getContentSize().height / 2 - spriteWidth / 2);
	addChild(sell_cost, 1, "sell_label");

	cocos2d::Sprite* cost_sugar = cocos2d::Sprite::create("res/buttons/sugar.png");
	cost_sugar->setScale(spriteWidth / cost_sugar->getContentSize().width);
	cost_sugar->setPosition(sell_cost->getPosition().x - spriteWidth,
		sell_cost->getPosition().y);
	addChild(cost_sugar, 1);
}

void TowerInformationPanel::createMaxLevelLabel(cocos2d::Size &visibleSize) {
	maxLevelLabel = cocos2d::Label::createWithTTF("Max. Level Reached", "fonts/LICABOLD.ttf", 35 * visibleSize.width / 1280);
	maxLevelLabel->setColor(cocos2d::Color3B::BLACK);
	maxLevelLabel->setAlignment(cocos2d::TextHAlignment::CENTER);
	maxLevelLabel->setDimensions(nextLevelInfos->getContentSize().width, nextLevelInfos->getContentSize().height * 0.6);
	maxLevelLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5));
	maxLevelLabel->setPosition(0, 0);
	addChild(maxLevelLabel, 3, "maxLevelLabel");
}

void TowerInformationPanel::updateDisplay() {
	if (tower->getLevel() < tower->getMaxLevel()) {
		currentLevelInfos->updateLabel();
		nextLevelInfos->updateLabel();
		nextLevelInfos->updateCost((int)level->getQuantity());
		updateDisplayNextLevel();
	}
	else {
		maxLevelLabel->setVisible(false);
		nextLevelInfos->setVisible(false);
		descriptionLayout->setVisible(false);
		lockedLayout->setVisible(true);
		nextLevelButton->setEnabled(false);
	}
}

void TowerInformationPanel::updateDisplayNextLevel()
{
	unsigned int towerLevel = tower->getLevel();
	lockedLayout->setVisible(false);
	if (towerLevel < (int)settings->getMaxExistingLevel()) {
		maxLevelLabel->setVisible(false);
		nextLevelInfos->setVisible(true);
		descriptionLayout->setVisible(false);
	}
	else {
		nextLevelInfos->setVisible(false);
		nextLevelButton->setEnabled(false);
	}
	if (towerLevel == settings->getMaxExistingLevel() - 2) {
		nextLevelInfos->setVisible(false);
		descriptionLayout->setVisible(true);
	}
	if ((int)level->getQuantity() < settings->getCost(towerLevel + 1)) {
		nextLevelButton->setEnabled(false);
	}
	else {
		if (towerLevel + 1 < settings->getMaxExistingLevel() && !nextLevelButton->isEnabled()) {
			nextLevelButton->setEnabled(true);
		}
		else if (towerLevel + 1 >= settings->getMaxExistingLevel() && (nextLevelButton->isEnabled()
			|| nextLevelInfos->isVisible())) {
			nextLevelButton->setEnabled(false);
			nextLevelInfos->setVisible(false);
			maxLevelLabel->setVisible(true);
		}
	}
}

void TowerInformationPanel::update() {
	updateDisplay();
	unsigned int towerLevel = tower->getLevel();
	
	if(towerLevel < tower->getMaxLevel()) {
		std::string language = configClass->getSettings()->getLanguage();
		std::string s = Json::Value(settings->getXP(towerLevel + 1)).asString();
		int dot_pos = s.find('.');
		s = s.substr(0, dot_pos);
		std::string lockedWord = configClass->getConfigValues(Config::ConfigType::BUTTON)["locked"][language].asString();
		std::string towerXP = Json::Value(tower->getCurrentXP()).asString();
		if (((cocos2d::Label*)lockedLayout->getChildByName("locked_label"))->getString() !=
			lockedWord + "\n" + towerXP + "/" + s) {
			((cocos2d::Label*)lockedLayout->getChildByName("locked_label"))->setString(
				lockedWord + "\n" + towerXP + "/" + s);
		}
	}
}