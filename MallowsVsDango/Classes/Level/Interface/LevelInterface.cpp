#include "LevelInterface.h"
#include "../../SceneManager.h"
#include "../../AppDelegate.h"
#include "../../Scenes/MyGame.h"
#include "../../Towers/TowerFactory.h"
#include "../../Lib/Functions.h"
#include "../Rewards/RewardTower.h"
#include "../Rewards/RewardSugar.h"
#include <iostream>
#include <string>
#include <sstream>
#include "../../GUI/ParametersMenu.h"
#include "../../Config/Config.h"
#include "../../Dangos/Monkey.h"
#include "../Level.h"


USING_NS_CC;

LevelInterface::LevelInterface() : sizeButton(cocos2d::Director::getInstance()->getVisibleSize().width / 15), towerPanel(nullptr)
{}

LevelInterface::~LevelInterface() {
	if (dialogues != nullptr) {
		delete dialogues;
		log("deleted dialogue");
	}
	cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
}

LevelInterface* LevelInterface::create(MyGame* ngame, Config* config) {

	LevelInterface* interface_game = new LevelInterface();

	if (interface_game->init(ngame, config))
	{
		interface_game->autorelease();
		return interface_game;
	}
	else
	{
		delete interface_game;
		interface_game = NULL;
		return NULL;
	}
}

bool LevelInterface::init(MyGame* ngame, Config* nConfig) {
	game = ngame;
	level = game->getLevel();
	if (!Layer::init()) { return false; }
	Size visibleSize = Director::getInstance()->getVisibleSize();

	state = State::IDLE;
	game_state = INTRO;
	configClass = nConfig; // ((AppDelegate*)Application::getInstance())->getConfigClass();
	Json::Value config = configClass->getConfigValues(Config::ConfigType::GENERAL);

	initParametersMenu(config);
	initLoseMenu(configClass->getSettings()->getLanguage(), configClass->getConfigValues(Config::ConfigType::BUTTON), configClass->getConfigValues(Config::ConfigType::ADVICE));
	initWinMenu(config);
	initRightPanel();
	initLabels(config);
	initStartMenu(config);
	initDialoguesFromLevel(config);

	addRewardLayout();

	addBlackMask(visibleSize);
	selected_turret = nullptr;

	addEvents();

	return true;
}

void LevelInterface::addRewardLayout()
{
	auto reward_layout = ui::Layout::create();
	addChild(reward_layout, 1, "reward_layout");
}

void LevelInterface::addBlackMask(cocos2d::Size &visibleSize)
{
	addChild(ui::Layout::create(), 3, "black_mask");
	ui::Button* mask = ui::Button::create("res/buttons/mask.png");
	mask->setScaleX(visibleSize.width / mask->getContentSize().width);
	mask->setScaleY(visibleSize.height / mask->getContentSize().height);
	mask->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	getChildByName("black_mask")->addChild(mask);
	getChildByName("black_mask")->setVisible(false);
}

bool LevelInterface::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) {
	cocos2d::Vec2 p = touch->getLocation();
	cocos2d::Rect rect = this->getBoundingBox();
	cocos2d::Rect rectrightpanel = rightPanel->getChildByName("panel")->getBoundingBox();
	rectrightpanel.origin += rightPanel->getBoundingBox().origin;
	challenges->hideDescription();
	if (rect.containsPoint(p)) {
		if (rectrightpanel.containsPoint(p)) {
			if (state == TURRET_CHOSEN) {
				if (selected_turret != nullptr) {
					selected_turret->destroyCallback(this);
					removeTower();
				}
			}
			auto item = rightPanel->getTowerFromPoint(touch->getStartLocation());
			if (item.first != "nullptr") {
				if (state == TURRET_SELECTED) {
					selected_turret->displayRange(false);
					hideTowerInfo();
					selected_turret = nullptr;
				}
				state = State::TOUCHED;
				rightPanel->displayTowerInfos(item.first, configClass->getSettings()->getLanguage());
				if (game_state == TITLE) {
					hideStartMenu();
				}
			}
			else {
				if (selected_turret != nullptr && state == TURRET_CHOSEN) {
					removeTower();
				}
				else if (selected_turret != nullptr && state == TURRET_SELECTED) {
					selected_turret->displayRange(false);
					hideTowerInfo();
					selected_turret = nullptr;
				}
				rightPanel->getChildByName("informations")->setVisible(false);
				state = State::IDLE;
			}
		}
		else {
			if (game_state == TITLE) {
				hideStartMenu();
			}
			Tower* tower = level->touchingTower(p);
			if (tower != nullptr) {
				if (state == TURRET_SELECTED) {
					selected_turret->displayRange(false);
					if (tower != selected_turret) {
						selected_turret = tower;
					}
					else {
						hideTowerInfo();
						selected_turret = nullptr;
						state = IDLE;
					}
				}
				else if (state == IDLE) {
					selected_turret = tower;
					state = TURRET_SELECTED;
				}
			}
			else {
				if (state == TURRET_SELECTED) {
					selected_turret->displayRange(false);
					hideTowerInfo();
					if (game_state == TITLE) {
						hideStartMenu();
					}
					selected_turret = nullptr;
					state = IDLE;
				}
				else if (state == TOUCHED) {
					state = IDLE;
					rightPanel->getChildByName("informations")->setVisible(false);
					//log("Problem, you shouldn't be in this state: TOUCHED when touched began in the background.");
				}
				else if (state == TURRET_CHOSEN) {
					Vec2 pos = touch->getLocation();
					moveSelectedTurret(pos);
					selected_turret->setVisible(true);
				}
			}
			Dango* dango = level->touchingDango(p);
			if (dango != nullptr) {
				if (state == TURRET_SELECTED) {
					state = IDLE;
					rightPanel->getChildByName("informations")->setVisible(false);
					selected_turret->displayRange(false);
					hideTowerInfo();
					selected_turret = nullptr;
				}
				if (state == IDLE || state == DANGO_SELECTED || state == TOUCHED) {
					rightPanel->getChildByName("informations")->setVisible(false);
					selectedDango = dango;
					state = DANGO_SELECTED;
				}
			}
			else {
				if (state == DANGO_SELECTED) {
					hideDangoInfo();
					selectedDango = nullptr;
					state = IDLE;
				}
			}
		}
		return true; // to indicate that we have consumed it.
	}
	return false; // we did not consume this event, pass thru.
}

void LevelInterface::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) {
	cocos2d::Vec2 p = touch->getLocation();
	cocos2d::Rect rectrightpanel = rightPanel->getChildByName("panel")->getBoundingBox();
	rectrightpanel.origin += rightPanel->getBoundingBox().origin;
	if (configClass->getSettings()->isMovingGridEnabled()) {
		level->showGrid(false);
	}
	if (state == TURRET_SELECTED) {
		Tower* tower = level->touchingTower(p);
		if (tower != nullptr) {
			selected_turret = tower;
			rightPanel->displayTowerInfos(tower->getSpecConfig()["name"].asString(), configClass->getSettings()->getLanguage());
			selected_turret->displayRange(true);
			showTowerInfo();
		}
		else {
			rightPanel->getChildByName("informations")->setVisible(false);
			selected_turret = nullptr;
			state = IDLE;
			displayStartMenuIfInTitleState();
		}
	}
	else if (state == DANGO_SELECTED) {
		Dango* dango = level->touchingDango(p);
		if (dango != nullptr) {
			showDangoInfo();
		}
		else {
			selectedDango = nullptr;
			state = IDLE;
			displayStartMenuIfInTitleState();
		}
	}
	else if (state == TURRET_CHOSEN) {
		if (!rectrightpanel.containsPoint(p)) {
			if (selected_turret->getPosition() != Vec2(0, 0)) {
				builtCallback(nullptr);
				state = State::IDLE;
				selected_turret->displayRange(false);
				rightPanel->getChildByName("informations")->setVisible(false);
				selected_turret = nullptr;
			}
			else {
				selected_turret->destroyCallback(this);
				removeTower();
			}
		}
		else {
			selected_turret->destroyCallback(this);
			removeTower();
		}
		displayStartMenuIfInTitleState();
	}
	else if (state == TOUCHED) {
		displayStartMenuIfInTitleState();
		//state = IDLE;
	}
	if (state == IDLE) {
		displayStartMenuIfInTitleState();
		rightPanel->getChildByName("informations")->setVisible(false);
	}
}

void LevelInterface::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) {
	if (state == State::TOUCHED) {
		Size visibleSize = Director::getInstance()->getVisibleSize();

		if (touch->getLocation().x - visibleSize.width * 3 / 4 < 0) {
			if (configClass->getSettings()->isMovingGridEnabled()) {
				level->showGrid(true);
			}
			auto item = rightPanel->getTowerFromPoint(touch->getStartLocation());
			if (item.first != "nullptr") {
				if (level->getQuantity() >= Tower::getConfig()[item.first]["cost"][0].asDouble()) {
					state = TURRET_CHOSEN;
					menuTurretTouchCallback(TowerFactory::getTowerTypeFromString(item.first));
					moveSelectedTurret(touch->getLocation());
					selected_turret->displayRange(true);
					selected_turret->setVisible(true);
				}
				else {
					resetSugarLabel();
					shakeScaleElement((Label*)levelInfo->getChildByName("sugar"), false);
				}
			}
		}
	}
	else if (state == State::TURRET_CHOSEN) {
		Vec2 pos = touch->getLocation();
		cocos2d::Rect rectrightpanel = rightPanel->getChildByName("panel")->getBoundingBox();
		rectrightpanel.origin += rightPanel->getBoundingBox().origin;
		if (rectrightpanel.containsPoint(pos)) {
			selected_turret->destroyCallback(nullptr);
			selected_turret = nullptr;
			state = TOUCHED;
			if (configClass->getSettings()->isMovingGridEnabled()) {
				level->showGrid(false);
			}
		}
		else {
			moveSelectedTurret(pos);
		}
	}
}

void LevelInterface::addMonkey(Monkey* monkey) {
	monkeys.push_back(monkey);
	addChild(monkey);
}

unsigned int LevelInterface::getNbOfMonkeys()
{
	return monkeys.size();
}

void LevelInterface::makeAllMonkeysGoAway()
{
	for (auto& monkey : monkeys) {
		if (monkey != nullptr) {
			monkey->goAway();
		}
	}
}

void LevelInterface::resetSugarLabel() {
	levelInfo->resetAnimations();
}

void LevelInterface::resetTowerMenu() {
	rightPanel->resetAnimations();
}

void LevelInterface::addEvents()
{
	listener = cocos2d::EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = CC_CALLBACK_2(LevelInterface::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(LevelInterface::onTouchEnded, this);
	listener->onTouchMoved = CC_CALLBACK_2(LevelInterface::onTouchMoved, this);

	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 30);
}

void LevelInterface::update(float dt) {
	levelInfo->update(level->getQuantity(), level->getLife(), level->getProgress());
	for (auto& monkey : monkeys) {
		if (monkey->hasToBeRemoved()) {
			removeChild(monkey);
			delete monkey;
			monkey = nullptr;
		}
	}
	monkeys.erase(std::remove(monkeys.begin(), monkeys.end(), nullptr), monkeys.end());

	if (towerPanel != nullptr && selected_turret != nullptr) {
		towerPanel->update();
	}

	switch (game_state) {
	case INTRO:
		dialogues->update();
		if (dialogues->hasFinished()) {
			Size visibleSize = Director::getInstance()->getVisibleSize();

			removeChild(dialogues);
			dialogues = nullptr;
			game_state = TITLE;
			startMenu->displayWithAnimation();
		}
		rightPanel->update();
		challenges->update();
		break;
	case TITLE:
		rightPanel->update();
		challenges->update();
		if (selected_turret != nullptr) {
			selected_turret->updateDisplay(dt);
		}
		break;
	case RUNNING:
		rightPanel->update();
		challenges->update();
		if (selected_turret != nullptr) {
			selected_turret->updateDisplay(dt);
		}
		break;
	}
}

void LevelInterface::menuTurretTouchCallback(Tower::TowerType turret) {
	if (selected_turret == nullptr && !game->isPaused()) {
		Tower* createdTower = TowerFactory::createTower(turret, configClass, level);
		level->addTurret(createdTower);
		selected_turret = createdTower;
	}
}

void LevelInterface::moveSelectedTurret(Vec2 pos) {
	if (selected_turret != nullptr) {
		Cell* nearestCell = level->getNearestCell(selected_turret->getPosition());
		Cell* nearestCell2 = level->getNearestCell(pos / level->getScale());
		if (nearestCell2 != nullptr && nearestCell != nullptr) {
			if (nearestCell2->isFree() && !nearestCell2->isPath() && !nearestCell2->isOffLimit()) {
				nearestCell->setObject(nullptr);
				nearestCell2->setObject(selected_turret);
				selected_turret->setPosition(nearestCell2->getPosition());
			}
		}
	}
}

bool LevelInterface::isOnTower(Vec2 pos) {
	Cell* nearestCell = level->getNearestCell(pos / level->getScale());
	if (nearestCell != nullptr) {
		if ((nearestCell->isFree() || nearestCell->getObject() == selected_turret) && !nearestCell->isPath() && !nearestCell->isOffLimit()) {
			return false;
		}
	}
	return true;
}

void LevelInterface::showLose() {
	loseMenu->showLose();
	setGameState(GameState::ENDING);
	getChildByName("black_mask")->setVisible(true);
}

void LevelInterface::showWin() {
	winMenu->showWin();
	getChildByName("black_mask")->setVisible(true);
}

void LevelInterface::reset() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Json::Value config = configClass->getConfigValues(Config::ConfigType::GENERAL);

	state = IDLE;
	game_state = TITLE;
	selected_turret = nullptr;
	selectedDango = nullptr;

	loseMenu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 1.5));
	winMenu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 1.5));
	//startMenu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 1.5));
	getChildByName("black_mask")->setVisible(false);

	startMenu->reset(level->getLevelId());
	getChildByName("reward_layout")->removeAllChildren();
	removeChild(towerPanel);
	delete towerPanel;
	towerPanel = nullptr;
	if (dialogues != nullptr) {
		removeChild(dialogues, 1);
		delete dialogues;
	}
	initDialoguesFromLevel(config);
	rightPanel->reset();
	removeChildByName("menu_win");
	initWinMenu(config);
	challenges = ChallengeHandler::create(configClass->getConfigValues(Config::ConfigType::LEVEL)["worlds"]
		[level->getWorldId()]["levels"][level->getLevelId()]["challenges"]);
	levelInfo->reset(challenges);
}

void LevelInterface::initParametersMenu(const Json::Value& config) {
	pauseMenu = ParametersMenu::create(game, configClass);
	addChild(pauseMenu, 4, "menu_pause");
}

void LevelInterface::initStartMenu(const Json::Value& config) {
	startMenu = StartMenu::create(this, level->getLevelId());
	addChild(startMenu, 2, "start");
}

void LevelInterface::showLabelInformation() {
	levelInfo->showProgress();
}

void LevelInterface::initLabels(const Json::Value& config) {
	challenges = ChallengeHandler::create(configClass->getConfigValues(Config::ConfigType::LEVEL)["worlds"]
		[level->getWorldId()]["levels"][level->getLevelId()]["challenges"]);

	levelInfo = LevelInfo::create(challenges);
	addChild(levelInfo, 2, "label_information");
}

void LevelInterface::initLoseMenu(const std::string& language, const Json::Value& buttons, const Json::Value& advice) {
	loseMenu = LoseMenu::create(game, language, buttons, advice);
	addChild(loseMenu, 4, "menu_lose");
}

void LevelInterface::initWinMenu(const Json::Value& config) {
	winMenu = WinMenu::create(game);
	addChild(winMenu, 4, "menu_win");
}

void LevelInterface::initRightPanel() {
	rightPanel = RightPanel::create(game, configClass);
	addChild(rightPanel, 2, "menup_panel");
}

void LevelInterface::removeTower() {
	selected_turret = nullptr;
	state = State::IDLE;
}

void LevelInterface::builtCallback(Ref* sender) {
	Json::Value action;
	action["tower_name"] = selected_turret->getName();
	action["time"] = (unsigned int)time(0);
	Vec2 turret_position = level->getNearestPositionInGrid(selected_turret->getPosition());
	action["position"]["x"] = turret_position.x;
	action["position"]["y"] = turret_position.y;
	action["action"] = "create_tower";
	game->addActionToTracker(action);
	challenges->addTower(TowerFactory::getTowerTypeFromString(selected_turret->getName()), selected_turret->getPosition());

	selected_turret->builtCallback(sender);
	selected_turret->setFixed(true);
	level->decreaseQuantity(selected_turret->getCost());
	rightPanel->displayTowerInfos("", configClass->getSettings()->getLanguage());
}

void LevelInterface::displayStartMenuIfInTitleState() {
	if (game_state == TITLE) {
		startMenu->displayWithAnimation();
	}
}

void LevelInterface::showDangoInfo() {
	if (selectedDango != nullptr) {
		selectedDango->showLifeBar();
	}
}

void LevelInterface::hideDangoInfo() {
	if (selectedDango != nullptr) {
		selectedDango->hideLifeBar();
	}
	displayStartMenuIfInTitleState();
}

void LevelInterface::showTowerInfo() {
	if (towerPanel != nullptr) {
		auto scale_to = ScaleTo::create(0.125f, 0.f);
		auto removeAndCreateLayout = CallFunc::create([&]() {
			if (selected_turret != nullptr) {
				towerPanel->setTower(selected_turret);
				towerPanel->update();
				towerPanel->setScale(0);
				auto scale_to = ScaleTo::create(0.125f, 1.f);
				towerPanel->runAction(scale_to);
			}
		});
		towerPanel->runAction(Sequence::create(scale_to, removeAndCreateLayout, nullptr));
	}
	else {
		towerPanel = TowerInformationPanel::create(game, selected_turret, configClass);
		addChild(towerPanel, 1, "information_tower");
		towerPanel->setScale(0);
		auto scale_to = ScaleTo::create(0.125f, 1.f);
		towerPanel->runAction(scale_to);
	}
}

void LevelInterface::hideTowerInfo() {
	if (towerPanel != nullptr) {
		auto scale_to = ScaleTo::create(0.125f, 0.f);
		towerPanel->runAction(scale_to);
	}
	displayStartMenuIfInTitleState();
}

void LevelInterface::hideStartMenu()
{
	startMenu->hide();
}

void LevelInterface::lockStartMenu()
{
	startMenu->lock(true);
}

void LevelInterface::unlockStartMenu()
{
	startMenu->lock(false);
}

LevelInterface::GameState LevelInterface::getGameState() const {
	return game_state;
}
cocos2d::Vec2 LevelInterface::getAbsoluteMenuTowerPosition(std::string towerName)
{
	return rightPanel->getAbsoluteMenuTowerPosition(towerName);
}
Sprite * LevelInterface::getTowerFromMenu(std::string towerName)
{
	return rightPanel->getTower(towerName);
}

void LevelInterface::setGameState(GameState g_state) {
	game_state = g_state;
}

void LevelInterface::setSelectedTower(Tower * tower)
{
	selected_turret = tower;
}

void LevelInterface::setListening(bool listening) {
	listener->setEnabled(listening);
}

void LevelInterface::generateHolySugar(Vec2 pos) {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	Node* node = Node::create();
	ui::Button* sugar = ui::Button::create("res/buttons/holy_sugar.png", "res/buttons/holy_sugar.png", "res/buttons/holy_sugar.png");
	Sprite* shining = Sprite::create("res/buttons/holy_sugar_shining.png");

	node->addChild(shining);
	node->addChild(sugar);

	sugar->setScale(Cell::getCellWidth() / sugar->getContentSize().width);
	shining->setScale(1.1*Cell::getCellWidth() / (shining->getContentSize().width));

	auto* move1 = EaseOut::create(MoveBy::create(0.25f, Vec2(0, 75)), 2);
	auto* move2 = EaseBounceOut::create(MoveBy::create(1.0f, Vec2(0, -75)));

	node->setPosition(pos);

	ScaleBy* scale_to1 = ScaleBy::create(1.0f, 1.2f);
	ScaleBy* scale_to2 = ScaleBy::create(1.0f, 1 / 1.2f);
	shining->runAction(RepeatForever::create(Sequence::create(scale_to1, scale_to2, nullptr)));

	MoveBy* move_h1 = nullptr;
	MoveBy* move_h2 = nullptr;
	if (pos.x - 60 < 0) {
		move_h1 = MoveBy::create(0.25f, Vec2(25, 0));
		move_h2 = MoveBy::create(1.0f, Vec2(75, 0));
	}
	else {
		move_h1 = MoveBy::create(0.25f, Vec2(-15, 0));
		move_h2 = MoveBy::create(1.0f, Vec2(-45, 0));
	}

	node->runAction(Sequence::create(Spawn::createWithTwoActions(move1, move_h1),
		Spawn::createWithTwoActions(move2, move_h2),
		nullptr));
	sugar->addTouchEventListener([&, node, sugar](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Size visibleSize = Director::getInstance()->getVisibleSize();

			EaseIn* move = EaseIn::create(MoveTo::create(1.5f, Vec2(visibleSize.width / 2, visibleSize.height)), 2);
			EaseIn* scale = EaseIn::create(ScaleTo::create(1.5f, 0.01f), 2);
			sugar->setEnabled(false);
			node->runAction(Sequence::create(Spawn::createWithTwoActions(move, scale), nullptr));
		}
	});

	addChild(node);
}

void LevelInterface::startRewarding(Vec2 pos) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	std::string language = configClass->getSettings()->getLanguage();
	Json::Value rootSave = configClass->getSaveValues();
	Json::Value level_config = configClass->getConfigValues(Config::ConfigType::LEVEL)["worlds"]
		[level->getWorldId()]["levels"][level->getLevelId()];

	challenges->endChallengeHandler();
	int successfull_challenges = challenges->countSucceedChallenges();
	std::vector<std::string> starNames = { "star_left", "star_middle", "star_right" };
	for (int i(0); i < successfull_challenges; ++i) {
		Sprite* star = Sprite::create("res/levels/rewards/star_full.png");
		star->setPosition(Vec2(star->getContentSize().width / 2, star->getContentSize().height / 2));
		((Sprite*)getChildByName("menu_win")->getChildByName(starNames[i]))->addChild(star);
	}

	if ((int)level->getWorldId() >= rootSave["c_world"].asInt() &&
		(int)level->getLevelId() >= rootSave["c_level"].asInt()) {
		ui::Layout* reward_layout = (ui::Layout*)getChildByName("reward_layout");

		// Creating Holy Sugar reward animation
		auto reward_holy_sugar = RewardSugar::create(pos);
		reward_layout->addChild(reward_holy_sugar, 2);
		reward_holy_sugar->animate();
		game_state = DONE;

		// If another reward can be displayed, then display it and animate it
		if (level_config["reward"]["type"].asString() != "none") {
			auto black_mask = ui::Button::create("res/buttons/mask.png");
			black_mask->setScaleX(visibleSize.width / black_mask->getContentSize().width);
			black_mask->setScaleY(visibleSize.height / black_mask->getContentSize().height);
			black_mask->setPosition(visibleSize / 2);
			black_mask->addTouchEventListener([&](Ref *sender, cocos2d::ui::Widget::TouchEventType type) {
				if (type == cocos2d::ui::Widget::TouchEventType::ENDED) { endGame(); }
			});
			reward_layout->addChild(black_mask, 1, "black_mask");

			if (level_config["reward"]["type"].asString() == "tower") {
				auto reward = RewardTower::create(level_config["reward"]["file"].asString(), level_config["reward"]["tower_name"].asString(),
					pos,
					[&](Ref *sender, cocos2d::ui::Widget::TouchEventType type) {
					if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
						endGame();
					}
				}
				);
				reward_layout->addChild(reward, 2);
				reward->animate();
			}

			// Add a Tap to continue to inform the user what to do.
			auto tapToContinue = Label::createWithTTF(
				configClass->getConfigValues(Config::ConfigType::BUTTON)
				["tap_continue"][language].asString(), "fonts/LICABOLD.ttf", 35.f * visibleSize.width / 1280);
			tapToContinue->setPosition(Vec2(visibleSize.width * 3 / 8, visibleSize.height / 15 + tapToContinue->getContentSize().height / 2));
			tapToContinue->setColor(Color3B::WHITE);
			tapToContinue->setVisible(true);
			FadeIn* fadeIn = FadeIn::create(0.75f);
			FadeOut* fadeout = FadeOut::create(0.75f);
			tapToContinue->runAction(RepeatForever::create(Sequence::create(fadeIn, fadeout, NULL)));
			reward_layout->addChild(tapToContinue, 2, "taptocontinue");

			game_state = ENDING;
		}
	}
	else {
		game_state = DONE;
	}

}

void LevelInterface::initDialoguesFromLevel(const Json::Value& config) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	if (configClass->getSettings()->isDialoguesEnabled()) {
		Settings* dialgouesSettings = Settings::create(configClass->getConfigValues(Config::ConfigType::LEVEL)["worlds"]
		[level->getWorldId()]["levels"][level->getLevelId()]["dialogues_file"].asString());
		if (dialgouesSettings->getSettingsMap()["introDialogue"].size() != 0) {
			dialogues = Dialogue::createFromConfig(dialgouesSettings->getSettingsMap()["introDialogue"]);
			addChild(dialogues, 1, "dialogue");
			dialogues->launch();
			game_state = INTRO;
		}
	}
	else {
		game_state = TITLE;
		startMenu->displayWithAnimation();
		dialogues = nullptr;
	}
}

Dango* LevelInterface::getCurrentDango() {
	return selectedDango;
}

int LevelInterface::getSugarQuantity()
{
	return level->getQuantity();
}

int LevelInterface::getLifeQuantity()
{
	return level->getLife();
}

void LevelInterface::handleDeadDango() {
	hideDangoInfo();
	selectedDango = nullptr;
	state = IDLE;
}

void LevelInterface::pauseLevel()
{
	level->pause();
}

void LevelInterface::resumeLevel()
{
	level->resume();
}

void LevelInterface::shakeScaleElement(Node* element, bool loop) {
	RotateTo* left = RotateTo::create(0.1f, 15);
	RotateTo* right = RotateTo::create(0.1f, -15);
	RotateTo* center = RotateTo::create(0.2f, 0);
	Spawn* spawn = Spawn::createWithTwoActions(
		Sequence::create(
			left, right,
			left->clone(), right->clone(),
			left->clone(), right->clone(),
			center, DelayTime::create(1.f),
			nullptr),
		Sequence::create(
			ScaleTo::create(0.4f, 1.5f),
			ScaleTo::create(0.4f, 1.f),
			nullptr)
	);

	if (loop) {
		element->runAction(RepeatForever::create(spawn));
	}
	else {
		element->runAction(spawn);
	}
}

void LevelInterface::endGame() {
	setGameState(DONE);
	setListening(false);
}