#include "InterfaceGame.h"
#include "../SceneManager.h"
#include "../AppDelegate.h"
#include "../Scenes/MyGame.h"
#include "../Towers/TowerFactory.h"
#include "../Lib/Functions.h"
#include "Rewards/RewardTower.h"
#include "Rewards/RewardSugar.h"
#include <iostream>
#include <string>
#include <sstream>
#include "../GUI/ParametersMenu.h"
#include "../GUI/StartMenu.h"


USING_NS_CC;

InterfaceGame::InterfaceGame() : sizeButton(cocos2d::Director::getInstance()->getVisibleSize().width / 15),
sizeTower(cocos2d::Director::getInstance()->getVisibleSize().width / 10), tutorial_running(false)
{}
InterfaceGame::~InterfaceGame() {
	if (dialogues != nullptr) {
		delete dialogues;
		log("deleted dialogue");
	}
}

bool InterfaceGame::init() {

	if (!Layer::init()) { return false; }
	Size visibleSize = Director::getInstance()->getVisibleSize();

	state = State::IDLE;
	game_state = INTRO;
	Json::Value config = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GENERAL);

	initParametersMenu(config);
	initLoseMenu(config);
	initWinMenu(config);
	initRightPanel(config);
	initLabels(config);
	initStartMenu(config);
	initDialoguesFromLevel(config);

	addRewardLayout();

	addBlackMask(visibleSize);
	selected_turret = nullptr;

	addEvents();

	return true;
}

void InterfaceGame::addRewardLayout()
{
	auto reward_layout = ui::Layout::create();
	addChild(reward_layout, 1, "reward_layout");
}

void InterfaceGame::addBlackMask(cocos2d::Size &visibleSize)
{
	addChild(ui::Layout::create(), 3, "black_mask");
	ui::Button* mask = ui::Button::create("res/buttons/mask.png");
	mask->setScaleX(visibleSize.width / mask->getContentSize().width);
	mask->setScaleY(visibleSize.height / mask->getContentSize().height);
	mask->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	getChildByName("black_mask")->addChild(mask);
	getChildByName("black_mask")->setVisible(false);
}

InterfaceGame* InterfaceGame::create(MyGame* ngame) {

	InterfaceGame* interface_game = new InterfaceGame();
	interface_game->setGame(ngame);

	if (interface_game->init())
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


bool InterfaceGame::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) {
	cocos2d::Vec2 p = touch->getLocation();
	cocos2d::Rect rect = this->getBoundingBox();
	cocos2d::Rect rectrightpanel = getChildByName("menu_panel")->getChildByName("panel")->getBoundingBox();
	rectrightpanel.origin += getChildByName("menu_panel")->getBoundingBox().origin;
	challenges->hideDescription();
	if (rect.containsPoint(p)) {
		if (rectrightpanel.containsPoint(p)) {
			if (state == TURRET_CHOSEN) {
				if (selected_turret != nullptr) {
					selected_turret->destroyCallback(this);
					removeTower();
				}
			}
			auto item = getTowerFromPoint(touch->getStartLocation());
			if (item.first != "nullptr") {
				if (state == TURRET_SELECTED) {
					selected_turret->displayRange(false);
					hideTowerInfo();
					selected_turret = nullptr;
				}
				state = State::TOUCHED;
				displayTowerInfos(item.first);
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
				getChildByName("menu_panel")->getChildByName("informations")->setVisible(false);
				state = State::IDLE;
			}
		}
		else {
			if (game_state == TITLE) {
				hideStartMenu();
			}
			Tower* tower = game->getLevel()->touchingTower(p);
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
					getChildByName("menu_panel")->getChildByName("informations")->setVisible(false);
					//log("Problem, you shouldn't be in this state: TOUCHED when touched began in the background.");
				}
				else if (state == TURRET_CHOSEN) {
					Vec2 pos = touch->getLocation();
					moveSelectedTurret(pos);
					selected_turret->setVisible(true);
				}
			}
			Dango* dango = game->getLevel()->touchingDango(p);
			if (dango != nullptr) {
				if (state == TURRET_SELECTED) {
					state = IDLE;
					getChildByName("menu_panel")->getChildByName("informations")->setVisible(false);
					selected_turret->displayRange(false);
					hideTowerInfo();
					selected_turret = nullptr;
				}
				if (state == IDLE || state == DANGO_SELECTED || state == TOUCHED) {
					getChildByName("menu_panel")->getChildByName("informations")->setVisible(false);
					selected_dango = dango;
					state = DANGO_SELECTED;
				}
			}
			else {
				if (state == DANGO_SELECTED) {
					hideDangoInfo();
					selected_dango = nullptr;
					state = IDLE;
				}
			}
		}
		return true; // to indicate that we have consumed it.
	}
	return false; // we did not consume this event, pass thru.
}

void InterfaceGame::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) {
	cocos2d::Vec2 p = touch->getLocation();
	cocos2d::Rect rectrightpanel = getChildByName("menu_panel")->getChildByName("panel")->getBoundingBox();
	rectrightpanel.origin += getChildByName("menu_panel")->getBoundingBox().origin;
	if (((AppDelegate*)Application::getInstance())->getConfigClass()->isMovingGridEnabled()) {
		game->getLevel()->showGrid(false);
	}
	if (state == TURRET_SELECTED) {
		Tower* tower = game->getLevel()->touchingTower(p);
		if (tower != nullptr) {
			selected_turret = tower;
			std::string name = tower->getSpecConfig()["name"].asString();
			displayTowerInfos(tower->getSpecConfig()["name"].asString());
			selected_turret->displayRange(true);
			showTowerInfo();
		}
		else {
			getChildByName("menu_panel")->getChildByName("informations")->setVisible(false);
			selected_turret = nullptr;
			state = IDLE;
			displayStartMenuIfInTitleState();
		}
	}
	else if (state == DANGO_SELECTED) {
		Dango* dango = game->getLevel()->touchingDango(p);
		if (dango != nullptr) {
			showDangoInfo();
		}
		else {
			selected_dango = nullptr;
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
				getChildByName("menu_panel")->getChildByName("informations")->setVisible(false);
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
		getChildByName("menu_panel")->getChildByName("informations")->setVisible(false);
	}
}

void InterfaceGame::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) {
	if (state == State::TOUCHED) {
		Size visibleSize = Director::getInstance()->getVisibleSize();

		double dist = touch->getLocation().distanceSquared(touch->getStartLocation());
		if (touch->getLocation().x - visibleSize.width * 3 / 4 < 0) {
			if (((AppDelegate*)Application::getInstance())->getConfigClass()->isMovingGridEnabled()) {
				game->getLevel()->showGrid(true);
			}
			auto item = getTowerFromPoint(touch->getStartLocation());
			if (item.first != "nullptr") {
				if (game->getLevel()->getQuantity() >= Tower::getConfig()[item.first]["cost"][0].asDouble()) {
					state = TURRET_CHOSEN;
					Tower::TowerType tape = Tower::getTowerTypeFromString(item.first);
					menuTurretTouchCallback(Tower::getTowerTypeFromString(item.first));
					moveSelectedTurret(touch->getLocation());
					selected_turret->displayRange(true);
					selected_turret->setVisible(true);
				}
				else {
					resetSugarLabel();
					shakeScaleElement((Label*)getChildByName("label_information")->getChildByName("sugar"), false);
				}
			}
		}
	}
	else if (state == State::TURRET_CHOSEN) {
		Vec2 pos = touch->getLocation();
		cocos2d::Rect rectrightpanel = getChildByName("menu_panel")->getChildByName("panel")->getBoundingBox();
		rectrightpanel.origin += getChildByName("menu_panel")->getBoundingBox().origin;
		if (rectrightpanel.containsPoint(pos)) {
			selected_turret->destroyCallback(nullptr);
			selected_turret = nullptr;
			state = TOUCHED;
			if (((AppDelegate*)Application::getInstance())->getConfigClass()->isMovingGridEnabled()) {
				game->getLevel()->showGrid(false);
			}
		}
		else {
			moveSelectedTurret(pos);
		}
	}
}

void InterfaceGame::resetSugarLabel()
{
	getChildByName("label_information")->getChildByName("sugar")->stopAllActions();
	getChildByName("label_information")->getChildByName("sugar")->setRotation(0.f);
	getChildByName("label_information")->getChildByName("sugar")->setScale(1.0f);
}

void InterfaceGame::resetTowerMenu()
{
	for (auto it : towers_menu) {
		it.second.first->stopAllActions();
		it.second.first->setRotation(0);
	}
}

void InterfaceGame::addEvents()
{
	listener = cocos2d::EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = CC_CALLBACK_2(InterfaceGame::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(InterfaceGame::onTouchEnded, this);
	listener->onTouchMoved = CC_CALLBACK_2(InterfaceGame::onTouchMoved, this);

	//cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 30);
}

void InterfaceGame::setGame(MyGame* ngame) {
	game = ngame;
}

void InterfaceGame::accelerateOnOffCallback(Ref* sender) {
	if (game->isAccelerated()) {
		game->pause();
		game->setNormalSpeed();
	}
	else {
		game->resume();
		game->increaseSpeed();
	}
}

void InterfaceGame::update(float dt) {
	std::string sugarText = " " + to_string(game->getLevel()->getQuantity());
	std::string lifeText = to_string(game->getLevel()->getLife());
	Label* sugar = (Label*)getChildByName("label_information")->getChildByName("sugar");
	Label* life = (Label*)getChildByName("label_information")->getChildByName("life");
	if (sugar->getString() != sugarText) {
		sugar->setString(sugarText);
		sugar->enableOutline(Color4B::BLACK, 2.0);
	}
	if (life->getString() != lifeText) {
		life->setString(lifeText);
		life->enableOutline(Color4B::BLACK, 2.0);
	}
	if (getChildByName("information_tower") != nullptr && selected_turret != nullptr) {
		selected_turret->updateInformationLayout((ui::Layout*)getChildByName("information_tower"));
	}
	if (getChildByName("information_dango") != nullptr && selected_dango != nullptr) {
		selected_dango->updateInformationLayout((ui::Layout*)getChildByName("information_dango"));
	}
	if (abs(((ui::LoadingBar*)getChildByName("label_information")->getChildByName("loading_bar"))->getPercent() -
		game->getLevel()->getProgress() * 100) > 0.01) {
		Size visibleSize = Director::getInstance()->getVisibleSize();

		((ui::LoadingBar*)getChildByName("label_information")->getChildByName("loading_bar"))->setPercent(game->getLevel()->getProgress() * 100);
		((ui::LoadingBar*)getChildByName("label_information")->getChildByName("dango_head"))->runAction(MoveTo::create(0.5f,
			getChildByName("label_information")->getChildByName("loading_bar")->getPosition() +
			Vec2((((ui::LoadingBar*)getChildByName("label_information")->getChildByName("loading_bar"))->
				getPercent() - 50) / 100 * visibleSize.width / 6 * 0.98, 0))
		);
	}
	switch (game_state) {
	case INTRO:
		dialogues->update();
		if (dialogues->hasFinished()) {
			Size visibleSize = Director::getInstance()->getVisibleSize();

			removeChild(dialogues);
			dialogues = nullptr;
			game_state = TITLE;
			((StartMenu*)getChildByName("start"))->displayWithAnimation();
		}
		updateButtonDisplay();
		break;
	case TITLE:
		updateButtonDisplay();
		updateObjectDisplay(dt);
		break;
	case RUNNING:
		updateButtonDisplay();
		updateObjectDisplay(dt);
		break;
	}
}

void InterfaceGame::menuTurretTouchCallback(Tower::TowerType turret) {
	if (selected_turret == nullptr && !game->isPaused()) {
		Tower* createdTower = TowerFactory::createTower(turret);
		game->getLevel()->addTurret(createdTower);
		selected_turret = createdTower;
	}
}

void InterfaceGame::moveSelectedTurret(Vec2 pos) {
	if (selected_turret != nullptr) {
		Cell* nearestCell = game->getLevel()->getNearestCell(selected_turret->getPosition());
		Cell* nearestCell2 = game->getLevel()->getNearestCell(pos / game->getLevel()->getScale());
		if (nearestCell2 != nullptr && nearestCell != nullptr) {
			if (nearestCell2->isFree() && !nearestCell2->isPath() && !nearestCell2->isOffLimit()) {
				nearestCell->setObject(nullptr);
				nearestCell2->setObject(selected_turret);
				selected_turret->setPosition(nearestCell2->getPosition());
			}
		}
	}
}

bool InterfaceGame::isOnTower(Vec2 pos) {
	Cell* nearestCell = game->getLevel()->getNearestCell(pos / game->getLevel()->getScale());
	if (nearestCell != nullptr) {
		if ((nearestCell->isFree() || nearestCell->getObject() == selected_turret) && !nearestCell->isPath() && !nearestCell->isOffLimit()) {
			return false;
		}
	}
	return true;
}

void InterfaceGame::showLose() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto* showAction = TargetedAction::create(getChildByName("menu_lose"),
		EaseBackOut::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height / 2))));
	getChildByName("menu_lose")->runAction(showAction);
	getChildByName("black_mask")->setVisible(true);
}

void InterfaceGame::showWin() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto* showAction = TargetedAction::create(getChildByName("menu_win"),
		EaseBackOut::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height / 2))));
	Json::Value root = ((AppDelegate*)Application::getInstance())->getSave();

	((Label*)getChildByName("menu_win")->getChildByName("reward_sugar_n"))->setString("+" + Value(game->getLevel()->getHolySugar()).asString());
	getChildByName("menu_win")->getChildByName("reward_sugar_n")->runAction(
		Sequence::create(
			DelayTime::create(0.5f),
			ScaleTo::create(0.25f, 1.5f),
			ScaleTo::create(0.25f, 1.f),
			nullptr));
	for (unsigned int i(0); i < root["towers"].getMemberNames().size(); ++i) {
		std::string tower_name = root["towers"].getMemberNames()[i];
		if (root["towers"][tower_name]["unlocked"].asBool()) {
			getChildByName("menu_win")->getChildByName(tower_name + "_levelup")->setVisible(false);
			auto exp_label = ((Label*)getChildByName("menu_win")->getChildByName(tower_name + "_exp"));
			int max_level = root["towers"][tower_name]["max_level"].asInt();
			exp_label->setColor(Color3B::WHITE);
			auto loading_bar = ((ui::LoadingBar*)getChildByName("menu_win")->getChildByName(tower_name + "_bar"));
			int diff_exp = game->getLevel()->getTowerXP(tower_name);
			exp_label->setString("+" + Value(game->getLevel()->getTowerXP(tower_name)).asString());
			float* increment = new float(0);
			int initial_xp = root["towers"][tower_name]["exp"].asInt();
			int loop(0);
			auto incrementExp = CallFunc::create([this, exp_label, loading_bar, tower_name, increment, initial_xp, diff_exp, loop, max_level]() {
				updateIncrementXP(exp_label, loading_bar, tower_name, increment, initial_xp, diff_exp, loop, max_level);
			});
			exp_label->runAction(Sequence::create(DelayTime::create(0.1f), ScaleTo::create(0.25f, 1.5f), ScaleTo::create(0.25f, 1.f), incrementExp, nullptr));
		}
	}
	getChildByName("menu_win")->runAction(showAction);
	getChildByName("black_mask")->setVisible(true);
}

void InterfaceGame::reset() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();
	Json::Value config = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GENERAL);


	state = IDLE;
	game_state = TITLE;
	selected_turret = nullptr;
	selected_dango = nullptr;
	tutorial_running = false;

	getChildByName("menu_panel")->getChildByName("informations")->setVisible(false);
	((ui::CheckBox*)getChildByName("menu_panel")->getChildByName("speed_up"))->setSelected(false);
	getChildByName("menu_lose")->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 1.5));
	getChildByName("menu_win")->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 1.5));
	getChildByName("menu_pause")->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 1.5));
	getChildByName("black_mask")->setVisible(false);

	((StartMenu*)getChildByName("start"))->reset(game->getLevel()->getLevelId());
	getChildByName("label_information")->getChildByName("dango_head")->setVisible(false);
	getChildByName("label_information")->getChildByName("loading_bar_background")->setVisible(false);
	getChildByName("label_information")->getChildByName("loading_bar")->setVisible(false);
	getChildByName("reward_layout")->removeAllChildren();
	removeChildByName("information_tower");
	removeChildByName("information_dango");
	if (dialogues != nullptr) {
		removeChild(dialogues, 1);
		delete dialogues;
	}
	initDialoguesFromLevel(config);
	towers_menu.empty();
	getChildByName("menu_panel")->getChildByName("towers")->removeAllChildren();
	createTowersLayout();
	removeChildByName("menu_win");
	initWinMenu(config);
	removeChild(challenges);
	removeChildByName("label_information");
	initLabels(config);
}

void InterfaceGame::initParametersMenu(const Json::Value& config) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	ui::Layout* menu_pause = ParametersMenu::create(game);
	addChild(menu_pause, 4, "menu_pause");
}

void InterfaceGame::initStartMenu(const Json::Value& config) {
	addChild(StartMenu::create(this, game->getLevel()->getLevelId()), 2, "start");
}

void InterfaceGame::showLabelInformation() {
	getChildByName("label_information")->getChildByName("dango_head")->setVisible(true);
	getChildByName("label_information")->getChildByName("loading_bar_background")->setVisible(true);
	getChildByName("label_information")->getChildByName("loading_bar")->setVisible(true);
}

void InterfaceGame::initLabels(const Json::Value& config) {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	ui::Layout* node_top = ui::Layout::create();
	node_top->setPosition(Vec2(0, visibleSize.height));

	Sprite* sugar = Sprite::create("res/buttons/sugar.png");
	sugar->setScale(sizeButton / sugar->getContentSize().width * 0.75);
	sugar->setPosition(Point(sizeButton * 0.125,
		-sugar->getContentSize().height * sugar->getScale()));
	sugar->setAnchorPoint(Vec2(0, 0.5f));
	node_top->addChild(sugar, 3);
	Label* label = Label::createWithTTF("x3000", "fonts/LICABOLD.ttf", 42 * visibleSize.width / 1280);
	node_top->addChild(label, 3, "sugar");
	label->setPosition(Point(sugar->getPosition().x + sugar->getContentSize().width * sugar->getScale(),
		sugar->getPosition().y));
	label->setAnchorPoint(Vec2(0, 0.5f));
	label->setColor(Color3B::WHITE);
	label->enableOutline(Color4B::BLACK, 3);
	Sprite* life = Sprite::create("res/buttons/life2.png");
	life->setPosition(Point(visibleSize.width * 3 / 4,
		sugar->getPosition().y));
	life->setAnchorPoint(Vec2(1, 0.5f));
	life->setScale(sizeButton / life->getContentSize().width * 0.75);
	node_top->addChild(life, 3);

	Label* label_life = Label::createWithTTF("", "fonts/LICABOLD.ttf", 42 * visibleSize.width / 1280);
	node_top->addChild(label_life, 3, "life");
	label_life->setPosition(Point(life->getPosition().x - life->getContentSize().width * life->getScale(),
		sugar->getPosition().y));
	label_life->setAnchorPoint(Vec2(1, 0.5f));
	label_life->setColor(Color3B::WHITE);
	label_life->enableOutline(Color4B::BLACK, 3);

	ui::LoadingBar* loadingBar = ui::LoadingBar::create("res/buttons/level_progression.png");
	loadingBar->setPercent(75);
	loadingBar->setScale(visibleSize.width / 6 / loadingBar->getContentSize().width);
	loadingBar->setDirection(ui::LoadingBar::Direction::LEFT);
	loadingBar->setPosition(Vec2(visibleSize.width * 3 / 8, -sizeButton / 2));
	loadingBar->setVisible(false);
	node_top->addChild(loadingBar, 3, "loading_bar");

	Sprite* dango_head = Sprite::create("res/buttons/minidango.png");
	dango_head->setPosition(loadingBar->getPosition() +
		Vec2((loadingBar->getPercent() - 50) / 100 * visibleSize.width / 6, 0));
	dango_head->setScale(loadingBar->getScale());
	dango_head->setVisible(false);
	node_top->addChild(dango_head, 4, "dango_head");

	Sprite* loadingBarBackground = Sprite::create("res/buttons/level_progression_background.png");
	loadingBarBackground->setPosition(loadingBar->getPosition());
	loadingBarBackground->setScale(loadingBar->getScale());
	loadingBarBackground->setVisible(false);
	node_top->addChild(loadingBarBackground, 3, "loading_bar_background");
	challenges = ChallengeHandler::create(((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::LEVEL)["worlds"]
		[game->getLevel()->getWorldId()]["levels"][game->getLevel()->getLevelId()]["challenges"]);
	challenges->setPosition(Vec2(sugar->getPosition().x + sugar->getContentSize().width * sugar->getScale() / 2,
		node_top->getPosition().y + sugar->getPosition().y - sugar->getContentSize().height * sugar->getScale()));
	addChild(challenges, 1, "challenges");

	addChild(node_top, 2, "label_information");
}

void InterfaceGame::initLoseMenu(const Json::Value& config) {
	Color3B color1 = Color3B(255, 200, 51);
	Color4F grey(102 / 255.0f, 178 / 255.0f, 255 / 255.0f, 0.66f);
	Size visibleSize = Director::getInstance()->getVisibleSize();
	std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();
	Json::Value buttons = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::BUTTON);

	auto menu_lose = ui::Layout::create();
	menu_lose->setPosition(Vec2(Point(visibleSize.width / 2, visibleSize.height * 1.5)));

	ui::Button* panel = ui::Button::create("res/buttons/centralMenuPanel2.png");
	panel->setZoomScale(0);
	menu_lose->addChild(panel, 1, "panel");
	panel->setScaleX(0.45*visibleSize.width / panel->getContentSize().width);
	panel->setScaleY(0.45*visibleSize.width / panel->getContentSize().width);

	ui::Button* retry = ui::Button::create("res/buttons/yellow_button.png");
	retry->setScale(visibleSize.width / 5 / retry->getContentSize().width);
	retry->setTitleText(buttons["retry"][language].asString());
	retry->setTitleFontName("fonts/LICABOLD.ttf");
	retry->setTitleFontSize(45.f);
	Label* retry_label = retry->getTitleRenderer();
	retry_label->enableOutline(Color4B::BLACK, 2);
	retry->setTitleColor(Color3B::WHITE);
	retry->setTitleAlignment(cocos2d::TextHAlignment::CENTER);
	retry->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Size visibleSize = Director::getInstance()->getVisibleSize();
			auto* hideAction = TargetedAction::create(getChildByName("menu_pause"),
				EaseBackIn::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height * 1.5))));
			getChildByName("menu_lose")->runAction(hideAction);
			getChildByName("black_mask")->setVisible(false);
			game->setReloading(true);
		}
	});
	retry->setPosition(Vec2(-panel->getContentSize().width*panel->getScaleX() / 4,
		-panel->getContentSize().height*panel->getScaleY() / 2 -
		retry->getContentSize().height*retry->getScaleY() * 0.41));
	menu_lose->addChild(retry, 1, "retry");

	ui::Button* main_menu_back = ui::Button::create("res/buttons/red_button.png");
	main_menu_back->setScale(visibleSize.width / 5 / main_menu_back->getContentSize().width);
	main_menu_back->setTitleText(buttons["main_menu"][language].asString());
	main_menu_back->setTitleFontName("fonts/LICABOLD.ttf");
	main_menu_back->setTitleFontSize(45.f);
	Label* menu_back_label = main_menu_back->getTitleRenderer();
	menu_back_label->enableOutline(Color4B::BLACK, 2);
	main_menu_back->setTitleColor(Color3B::WHITE);
	main_menu_back->setTitleAlignment(cocos2d::TextHAlignment::CENTER);
	main_menu_back->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			mainMenuCallBack("menu_lose");
		}
	});
	main_menu_back->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() / 4,
		-panel->getContentSize().height*panel->getScaleY() / 2 -
		main_menu_back->getContentSize().height*main_menu_back->getScaleY() * 0.41));
	menu_lose->addChild(main_menu_back, 1, "main_menu_back");

	Label* you_lose = Label::createWithTTF(
		buttons["lose_info"][language].asString(),
		"fonts/LICABOLD.ttf", 50.f * visibleSize.width / 1280);
	you_lose->enableOutline(Color4B::BLACK, 2);
	you_lose->setPosition(0, panel->getContentSize().height*panel->getScaleY() * 0.35);
	you_lose->setColor(Color3B::YELLOW);
	menu_lose->addChild(you_lose, 2, "text");

	auto conf = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::ADVICE)[language];
	std::string advice_text = conf[rand() % conf.size()].asString();

	Label* advice = Label::createWithTTF(advice_text, "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	advice->setDimensions(panel->getContentSize().width * panel->getScaleX() * 0.75,
		panel->getContentSize().height * panel->getScaleY() * 0.4);
	advice->setPosition(0, 0);
	advice->setColor(Color3B::BLACK);
	advice->setHorizontalAlignment(TextHAlignment::CENTER);
	advice->setVerticalAlignment(TextVAlignment::CENTER);
	menu_lose->addChild(advice, 2, "advice_text");

	menu_lose->setVisible(true);
	addChild(menu_lose, 4, "menu_lose");
}

void InterfaceGame::initWinMenu(const Json::Value& config) {
	Color3B color1 = Color3B(255, 200, 51);
	Color4F grey(102 / 255.0f, 178 / 255.0f, 255 / 255.0f, 0.66f);
	Size visibleSize = Director::getInstance()->getVisibleSize();
	std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();
	Json::Value buttons = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::BUTTON);

	auto menu_win = ui::Layout::create();
	menu_win->setPosition(Vec2(Point(visibleSize.width / 2, visibleSize.height * 1.5)));
	//menu_win->setPosition(Vec2(Point(visibleSize.width / 2, visibleSize.height / 2)));

	ui::Button* panel = ui::Button::create("res/buttons/centralMenuPanel2.png");
	panel->setZoomScale(0);
	menu_win->addChild(panel, 1, "panel");
	panel->setScaleX(0.55*visibleSize.width / panel->getContentSize().width);
	panel->setScaleY(0.55*visibleSize.width / panel->getContentSize().width);

	ui::Button* next_level = ui::Button::create("res/buttons/yellow_button.png");
	next_level->setScale(visibleSize.width / 5 / next_level->getContentSize().width);
	next_level->setTitleText(buttons["next_level"][language].asString());
	next_level->setTitleFontName("fonts/LICABOLD.ttf");
	next_level->setTitleFontSize(45.f);
	Label* next_level_label = next_level->getTitleRenderer();
	next_level_label->enableOutline(Color4B::BLACK, 2);
	next_level->setTitleColor(Color3B::WHITE);
	next_level->setTitleAlignment(cocos2d::TextHAlignment::CENTER);
	next_level->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Size visibleSize = Director::getInstance()->getVisibleSize();
			auto* hideAction = TargetedAction::create(getChildByName("menu_win"),
				EaseBackIn::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height * 1.5))));
			auto callbacknextlevel = CallFunc::create([&]() {
				game_state = GameState::NEXT_LEVEL;
			});
			getChildByName("menu_win")->runAction(Sequence::create(hideAction, callbacknextlevel, nullptr));
			getChildByName("black_mask")->setVisible(false);
		}
	});
	next_level->setPosition(Vec2(-panel->getContentSize().width*panel->getScaleX() / 4,
		-panel->getContentSize().height*panel->getScaleY() / 2 -
		next_level->getContentSize().height*next_level->getScaleY() * 0.41));
	menu_win->addChild(next_level, 1, "next_level");

	ui::Button* main_menu_back = ui::Button::create("res/buttons/red_button.png");
	main_menu_back->setScale(visibleSize.width / 5 / main_menu_back->getContentSize().width);
	main_menu_back->setTitleText(buttons["main_menu"][language].asString());
	main_menu_back->setTitleFontName("fonts/LICABOLD.ttf");
	main_menu_back->setTitleFontSize(45.f);
	Label* menu_back_label = main_menu_back->getTitleRenderer();
	menu_back_label->enableOutline(Color4B::BLACK, 2);
	main_menu_back->setTitleColor(Color3B::WHITE);
	main_menu_back->setTitleAlignment(cocos2d::TextHAlignment::CENTER);
	main_menu_back->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			mainMenuCallBack("menu_win");
		}
	});
	main_menu_back->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() / 4,
		-panel->getContentSize().height*panel->getScaleY() / 2 -
		main_menu_back->getContentSize().height*main_menu_back->getScaleY() * 0.41));
	menu_win->addChild(main_menu_back, 1, "main_menu_back");

	Label* you_win = Label::createWithTTF(buttons["level_cleared"][language].asString(),
		"fonts/LICABOLD.ttf", 60.f * visibleSize.width / 1280);
	you_win->enableOutline(Color4B::BLACK, 2);
	you_win->setPosition(0, panel->getContentSize().height*panel->getScaleY() * 0.35);
	you_win->setColor(Color3B::YELLOW);
	menu_win->addChild(you_win, 2, "you_win");

	Sprite* star_left = Sprite::create("res/levels/rewards/star_empty.png");
	star_left->setScale(panel->getContentSize().width * panel->getScaleX() / 8 / star_left->getContentSize().width);
	star_left->setPosition(Vec2(-panel->getContentSize().width * panel->getScaleX() * 1 / 5,
		you_win->getPosition().y -
		star_left->getContentSize().height / 2 - you_win->getContentSize().height));
	star_left->setRotation(-35);
	Sprite* star_middle = Sprite::create("res/levels/rewards/star_empty.png");
	star_middle->setScale(panel->getContentSize().width * panel->getScaleX() / 7 / star_middle->getContentSize().width);
	star_middle->setPosition(Vec2(0, star_left->getPosition().y + sizeButton / 3));
	Sprite* star_right = Sprite::create("res/levels/rewards/star_empty.png");
	star_right->setScale(panel->getContentSize().width * panel->getScaleX() / 8 / star_right->getContentSize().width);
	star_right->setRotation(35);
	star_right->setPosition(Vec2(panel->getContentSize().width * panel->getScaleX() * 1 / 5, star_left->getPosition().y));
	menu_win->addChild(star_left, 2, "star_left");
	menu_win->addChild(star_middle, 2, "star_middle");
	menu_win->addChild(star_right, 2, "star_right");


	Label* reward_sugar = Label::createWithTTF(buttons["holy_sugar"][language].asString(),
		"fonts/LICABOLD.ttf", 40.f * visibleSize.width / 1280);
	reward_sugar->enableOutline(Color4B::BLACK, 2);
	reward_sugar->setPosition(-panel->getContentSize().width * panel->getScaleX() * 2 / 5, star_left->getPosition().y -
		reward_sugar->getContentSize().height / 2 - star_middle->getContentSize().height * star_middle->getScale() * 3 / 4);
	reward_sugar->setAnchorPoint(Vec2(0.f, 0.5f));
	reward_sugar->setColor(Color3B::WHITE);
	menu_win->addChild(reward_sugar, 2, "reward_sugar_label");

	Label* reward_sugar_n = Label::createWithTTF("0", "fonts/LICABOLD.ttf", 45.f * visibleSize.width / 1280);
	reward_sugar_n->enableOutline(Color4B::BLACK, 2);
	reward_sugar_n->setPosition(panel->getContentSize().width * panel->getScaleX() / 3, reward_sugar->getPosition().y);
	reward_sugar_n->setAnchorPoint(Vec2(0.f, 0.5f));
	reward_sugar_n->setColor(Color3B::YELLOW);
	menu_win->addChild(reward_sugar_n, 2, "reward_sugar_n");

	Sprite* holy_sugar = Sprite::create("res/buttons/holy_sugar.png");
	//holy_sugar->setAnchorPoint(Vec2(0.f, 0.5f));
	holy_sugar->setScale(panel->getContentSize().width * panel->getScaleX() / 10 / holy_sugar->getContentSize().width);
	holy_sugar->setPosition(panel->getContentSize().width * panel->getScaleX() * 0.125, reward_sugar->getPosition().y);
	menu_win->addChild(holy_sugar, 2, "holy_sugar");

	Json::Value root = ((AppDelegate*)Application::getInstance())->getConfigClass()->getSaveValues()["towers"];
	Vec2 ini_pos = reward_sugar->getPosition() - Vec2(0, reward_sugar->getContentSize().height);
	std::vector<std::string> tower_names = root.getMemberNames();
	for (auto tower_name : tower_names) {
		if (root[tower_name]["unlocked"].asBool()) {
			Label* exp_tower = Label::createWithTTF("Exp " + tower_name, "fonts/LICABOLD.ttf", 40.f * visibleSize.width / 1280);
			exp_tower->enableOutline(Color4B::BLACK, 2);
			exp_tower->setPosition(-panel->getContentSize().width * panel->getScaleX() * 2 / 5, ini_pos.y - exp_tower->getContentSize().height / 2);
			exp_tower->setAnchorPoint(Vec2(0.f, 0.5f));
			exp_tower->setColor(Color3B::WHITE);
			menu_win->addChild(exp_tower, 2, tower_name);

			Label* exp_tower_n = Label::createWithTTF("0", "fonts/LICABOLD.ttf", 40.f * visibleSize.width / 1280);
			exp_tower_n->enableOutline(Color4B::BLACK, 2);
			exp_tower_n->setPosition(panel->getContentSize().width * panel->getScaleX() / 3, exp_tower->getPosition().y);
			exp_tower_n->setAnchorPoint(Vec2(0.f, 0.5f));
			exp_tower_n->setColor(Color3B::WHITE);
			menu_win->addChild(exp_tower_n, 2, tower_name + "_exp");

			ui::LoadingBar* loading_bar = ui::LoadingBar::create("res/buttons/loaderProgress.png");
			loading_bar->setPercent(100 * root[tower_name]["exp"].asDouble() /
				Tower::getConfig()[tower_name]["xp_level"][root[tower_name]["max_level"].asInt() + 1].asDouble());
			loading_bar->setScaleX(panel->getContentSize().width * panel->getScaleX() * 0.25 / loading_bar->getContentSize().width);
			loading_bar->setScaleY(exp_tower->getContentSize().height * 0.35f / loading_bar->getContentSize().height);
			loading_bar->setDirection(ui::LoadingBar::Direction::LEFT);
			loading_bar->setPosition(Vec2(loading_bar->getContentSize().width * loading_bar->getScaleX() / 2, exp_tower->getPosition().y));
			menu_win->addChild(loading_bar, 2, tower_name + "_bar");

			Sprite* loadingBarBackground = Sprite::create("res/buttons/loaderBackground.png");
			loadingBarBackground->setPosition(loading_bar->getPosition());
			loadingBarBackground->setScale(panel->getContentSize().width * panel->getScaleX() * 0.26 / loadingBarBackground->getContentSize().width);
			loadingBarBackground->setScaleY(exp_tower->getContentSize().height * 0.40f / loading_bar->getContentSize().height);
			menu_win->addChild(loadingBarBackground, 1, tower_name + "_bar_background");

			Label* level_up = Label::createWithTTF("Level up!", "fonts/LICABOLD.ttf", 23.f * visibleSize.width / 1280);
			level_up->setColor(Color3B::YELLOW);
			level_up->enableOutline(Color4B::RED, 1);
			level_up->setVisible(false);
			level_up->setPosition(loading_bar->getPosition().x, loading_bar->getPosition().y +
				loading_bar->getContentSize().height * loading_bar->getScaleY() / 2 +
				level_up->getContentSize().height / 2);
			menu_win->addChild(level_up, 2, tower_name + "_levelup");
			ini_pos.y = exp_tower->getPosition().y - exp_tower->getContentSize().height / 2;
		}
	}

	Sprite* win_mallow = Sprite::create("res/buttons/win.png");
	win_mallow->setScale(panel->getContentSize().width * panel->getScaleX() / 7 / win_mallow->getContentSize().width);
	win_mallow->setPosition(you_win->getPosition().x - you_win->getContentSize().width / 2 - win_mallow->getContentSize().width*win_mallow->getScale() * 3 / 4,
		you_win->getPosition().y);
	menu_win->addChild(win_mallow, 2, "win_mallow");
	/*Sprite* win_mallow2 = Sprite::create("res/buttons/win.png");
	win_mallow2->setScale(panel->getContentSize().width * panel->getScaleX() / 6 / win_mallow2->getContentSize().width);
	win_mallow2->setPosition(0,
		-panel->getContentSize().height * panel->getScaleY() * 2 / 5);
	menu_win->addChild(win_mallow2, 2, "win_mallow2");*/
	Sprite* win_mallow3 = Sprite::create("res/buttons/win.png");
	win_mallow3->setScale(panel->getContentSize().width * panel->getScaleX() / 7 / win_mallow3->getContentSize().width);
	win_mallow3->setPosition(you_win->getPosition().x + you_win->getContentSize().width / 2 + win_mallow3->getContentSize().width*win_mallow3->getScale() * 3 / 4,
		you_win->getPosition().y);
	menu_win->addChild(win_mallow3, 2, "win_mallow3");

	addChild(menu_win, 4, "menu_win");
}

void InterfaceGame::initRightPanel(const Json::Value& config) {
	Size visibleSize = Director::getInstance()->getVisibleSize();


	Layer* menu_panel = Layer::create();
	menu_panel->setPosition(Vec2(visibleSize.width * (3 / 4.0 + 1 / 8.0), visibleSize.height / 2));
	addChild(menu_panel, 2, "menu_panel");

	// PANEL FOR TOWERS, PARAMETERS AND INFOS
	auto panel = Sprite::create("res/buttons/menupanel5.png");
	panel->setScaleY(visibleSize.height / panel->getContentSize().height * 1.025);
	panel->setScaleX(visibleSize.width / panel->getContentSize().width / 4);
	menu_panel->addChild(panel, 2, "panel");

	// DISPLAY BUTTONS FOR GAME
	auto parameters = ui::Button::create("res/buttons/parameters_button.png");
	parameters->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			((ParametersMenu*)getChildByName("menu_pause"))->displayWithAnimation();
			game->pause();
		}
	});
	parameters->setScale(1 / (parameters->getBoundingBox().size.width / sizeButton));
	parameters->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() * 0.3,
		panel->getContentSize().height*panel->getScaleY() * 0.48 -
		parameters->getContentSize().height*parameters->getScaleY() / 2));
	menu_panel->addChild(parameters, 2, "parameters");

	auto speed_up = ui::CheckBox::create("res/buttons/speed_up.png", "res/buttons/normal_speed.png", ui::Widget::TextureResType::LOCAL);
	speed_up->addTouchEventListener([&, speed_up](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			if (speed_up->isSelected()) {
				game->increaseSpeed();
			}
			else {
				game->setNormalSpeed();
			}
		}
	});
	speed_up->setScale(1 / (speed_up->getBoundingBox().size.width / sizeButton));
	speed_up->setPosition(Vec2(0,
		panel->getContentSize().height*panel->getScaleY() * 0.48 -
		speed_up->getContentSize().height*speed_up->getScaleY() / 2));
	menu_panel->addChild(speed_up, 2, "speed_up");

	auto reload = ui::Button::create("res/buttons/restart_button.png");
	reload->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			game->updateTracker("reloaded");
			game->setReloading(true);
		}
	});
	reload->setScale(1 / (reload->getBoundingBox().size.width / sizeButton));
	reload->setPosition(Vec2(-panel->getContentSize().width*panel->getScaleX() * 0.3,
		panel->getContentSize().height*panel->getScaleY() * 0.48 -
		reload->getContentSize().height*reload->getScaleY() / 2));
	menu_panel->addChild(reload, 2, "reload");

	// PANEL FOR TOWERS
	auto towers = Layer::create();
	towers->setContentSize(Size(visibleSize.width / 4 * 0.9, visibleSize.height * 3 / 5));
	towers->setPosition(Vec2(-visibleSize.width / 8 * 0.9, reload->getPosition().y -
		reload->getContentSize().height * reload->getScaleY() * 2 / 3));
	towers->setAnchorPoint(Vec2(0.5, 0.5));
	menu_panel->addChild(towers, 2, "towers");
	createTowersLayout();

	// PANEL INFOS
	auto informations = ui::Layout::create();
	informations->setContentSize(Size(towers->getContentSize().width, panel->getContentSize().height / 4));
	informations->setPosition(Vec2(towers->getPosition().x + towers->getContentSize().width / 0.9 / 2,
		towers->getPosition().y - towers->getContentSize().height));
	informations->setVisible(false);
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();

	Node* animation = Node::create();
	informations->addChild(animation, 1, "animation");

	Sprite* attack = Sprite::create("res/buttons/attack.png");
	Sprite* speed = Sprite::create("res/buttons/speed.png");
	Sprite* range = Sprite::create("res/buttons/range.png");

	attack->setScale(informations->getContentSize().width / 10 / attack->getContentSize().width);
	speed->setScale(informations->getContentSize().width / 10 / speed->getContentSize().width);
	range->setScale(informations->getContentSize().width / 10 / range->getContentSize().width);
	attack->setPosition(0, 0);
	speed->setPosition(0, attack->getPosition().y - attack->getContentSize().height*
		attack->getScaleY() / 2 - speed->getContentSize().height * speed->getScaleY() * 3 / 4);
	range->setPosition(0, speed->getPosition().y - speed->getContentSize().height*
		speed->getScaleY() / 2 - range->getContentSize().height * range->getScaleY() * 3 / 4);
	animation->setPosition(-informations->getContentSize().width * 0.33, range->getPosition().y);
	informations->addChild(attack, 1, "attack");
	informations->addChild(speed, 1, "speed");
	informations->addChild(range, 1, "range");
	Label* attack_label = Label::createWithTTF("10", "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	attack_label->setColor(Color3B::BLACK);
	attack_label->setPosition(Vec2(attack->getPosition().x + attack->getContentSize().width * attack->getScale(),
		attack->getPosition().y));
	attack_label->setAnchorPoint(Vec2(0.f, 0.5f));
	informations->addChild(attack_label, 1, "attack_label");
	Label* range_label = Label::createWithTTF("2", "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	range_label->setColor(Color3B::BLACK);
	range_label->setPosition(Vec2(range->getPosition().x + attack->getContentSize().width * attack->getScale(),
		range->getPosition().y));
	range_label->setAnchorPoint(Vec2(0.f, 0.5f));
	informations->addChild(range_label, 1, "range_label");
	Label* speed_label = Label::createWithTTF("1.5", "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	speed_label->setColor(Color3B::BLACK);
	speed_label->setPosition(Vec2(speed->getPosition().x + attack->getContentSize().width * attack->getScale(),
		speed->getPosition().y));
	speed_label->setAnchorPoint(Vec2(0.f, 0.5f));
	informations->addChild(speed_label, 1, "speed_label");

	Label* description_tower = Label::createWithTTF("Sword Master: Attack all the dangos around him. Unavoidable damages.",
		"fonts/LICABOLD.ttf",
		20 * visibleSize.width / 1280);
	description_tower->setDimensions(informations->getContentSize().width, informations->getContentSize().height * 2 / 5);
	description_tower->setColor(Color3B::BLACK);
	description_tower->setAnchorPoint(Vec2(0.5f, 0.5f));
	description_tower->setPosition(Vec2(0, range->getPosition().y -
		range->getContentSize().height * range->getScaleY() -
		description_tower->getDimensions().height / 2));
	informations->addChild(description_tower, 1, "description_tower");

	Sprite* sugar = Sprite::create("res/buttons/sugar.png");
	sugar->setScale(sizeButton / 2 / sugar->getContentSize().width);
	sugar->setPosition(Vec2(0, description_tower->getPosition().y - sugar->getContentSize().height * sugar->getScaleY() / 2));
	sugar->setAnchorPoint(Vec2(1.f, 0.5f));
	informations->addChild(sugar, 1, "sugar");
	Label* sugar_label = Label::createWithTTF("30", "fonts/LICABOLD.ttf", 35 * visibleSize.width / 1280);
	sugar_label->setColor(Color3B::GREEN);
	sugar_label->setPosition(Vec2(sugar->getPosition().x, sugar->getPosition().y));
	sugar_label->setAnchorPoint(Vec2(0, 0.5f));
	sugar_label->enableOutline(Color4B::BLACK, 1);
	informations->addChild(sugar_label, 1, "sugar_label");

	menu_panel->addChild(informations, 2, "informations");

}

void InterfaceGame::displayTowerInfos(std::string item_name) {
	if (item_name != "") {
		getChildByName("menu_panel")->getChildByName("informations")->setVisible(true);
		Node* batch = getChildByName("menu_panel")->getChildByName("informations")->getChildByName("animation");
		batch->removeAllChildren();

		Size visibleSize = Director::getInstance()->getVisibleSize();
		Json::Value tower_config = Tower::getConfig()[item_name];
		unsigned int required_quantity(0);
		((Label*)getChildByName("menu_panel")->getChildByName("informations")->getChildByName("attack_label"))->setString(
			tower_config["damages"][0].asString());
		std::string s = tower_config["attack_speed"][0].asString();
		s.resize(4);
		((Label*)getChildByName("menu_panel")->getChildByName("informations")->getChildByName("speed_label"))->setString(s);
		double range = round(tower_config["range"][0].asDouble() / Cell::getCellWidth() * 100) / 100;
		s = Value(range).asString();
		s.resize(4);
		((Label*)getChildByName("menu_panel")->getChildByName("informations")->getChildByName("range_label"))->setString(
			s);
		((Label*)getChildByName("menu_panel")->getChildByName("informations")->getChildByName("description_tower"))->setString(
			tower_config["description_" +
			((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage()].asString());
		((Label*)getChildByName("menu_panel")->getChildByName("informations")->getChildByName("sugar_label"))->setString(
			tower_config["cost"][0].asString());
		required_quantity = tower_config["cost"][0].asDouble();
		if (game->getLevel()->getQuantity() < required_quantity) {
			((Label*)getChildByName("menu_panel")->getChildByName("informations")->getChildByName("sugar_label"))->setColor(Color3B::RED);
		}
		else {
			((Label*)getChildByName("menu_panel")->getChildByName("informations")->getChildByName("sugar_label"))->setColor(Color3B::GREEN);
		}

		auto animated_skeleton = Tower::getSkeletonAnimationFromName(item_name);
		animated_skeleton->setScale(
			getChildByName("menu_panel")->getChildByName("informations")->getContentSize().width *
			0.40 / animated_skeleton->getSkeleton()->data->width);
		batch->addChild(animated_skeleton);
	}
	else {
		getChildByName("menu_panel")->getChildByName("informations")->setVisible(false);
	}

}

void InterfaceGame::mainMenuCallBack(std::string id_menu) {
	setListening(false);
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto* hideAction = TargetedAction::create(getChildByName(id_menu),
		EaseBackIn::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height * 1.5))));
	auto callbackmainmenu = CallFunc::create([&]() {
		SceneManager::getInstance()->setScene(SceneManager::LEVELS);
	});
	getChildByName(id_menu)->runAction(Sequence::create(hideAction, callbackmainmenu, nullptr));
}

void InterfaceGame::removeTower() {
	selected_turret = nullptr;
	state = State::IDLE;
}

void InterfaceGame::destroyCallback(Ref* sender) {

	game->getLevel()->increaseQuantity(selected_turret->getCost() * 0.5 * (selected_turret->getLevel() + 1));
}

void InterfaceGame::builtCallback(Ref* sender) {
	Json::Value action;
	action["tower_name"] = selected_turret->getName();
	action["time"] = (unsigned int)time(0);
	Vec2 turret_position = game->getLevel()->getNearestPositionInGrid(selected_turret->getPosition());
	action["position"]["x"] = turret_position.x;
	action["position"]["y"] = turret_position.y;
	action["action"] = "create_tower";
	game->addActionToTracker(action);
	challenges->addTower(Tower::getTowerTypeFromString(selected_turret->getName()), selected_turret->getPosition());

	selected_turret->builtCallback(sender);
	selected_turret->setFixed(true);
	game->getLevel()->decreaseQuantity(selected_turret->getCost());
	displayTowerInfos("");
}

void InterfaceGame::displayStartMenuIfInTitleState() {
	if (game_state == TITLE) {
		((StartMenu*)getChildByName("start"))->displayWithAnimation();
	}
}

void InterfaceGame::showDangoInfo() {
	if (getChildByName("information_dango") != nullptr) {
		auto scale_to = ScaleTo::create(0.125f, 0.f);
		auto removeAndCreateLayout = CallFunc::create([&]() {
			removeChildByName("information_dango");
			if (selected_dango != nullptr) {
				auto layout = selected_dango->getInformationLayout(this);
				addChild(layout, 1, "information_dango");
				layout->setScale(0);
				auto scale_to = ScaleTo::create(0.125f, 1.f);
				layout->runAction(scale_to);
			}
		});
		getChildByName("information_dango")->runAction(Sequence::create(scale_to, removeAndCreateLayout, nullptr));
	}
	else if (selected_dango != nullptr) {
		auto layout = selected_dango->getInformationLayout(this);
		addChild(layout, 1, "information_dango");
		layout->setScale(0);
		auto scale_to = ScaleTo::create(0.125f, 1.f);
		layout->runAction(scale_to);
	}
}

void InterfaceGame::hideDangoInfo() {
	if (getChildByName("information_dango") != nullptr) {
		auto scale_to = ScaleTo::create(0.125f, 0.f);
		auto removeAndCreateLayout = CallFunc::create([&]() {
			removeChildByName("information_dango");
		});
		getChildByName("information_dango")->runAction(Sequence::create(scale_to, removeAndCreateLayout, nullptr));
	}
	displayStartMenuIfInTitleState();
}

void InterfaceGame::showTowerInfo() {
	if (getChildByName("information_tower") != nullptr) {
		auto scale_to = ScaleTo::create(0.125f, 0.f);
		auto removeAndCreateLayout = CallFunc::create([&]() {
			removeChildByName("information_tower");
			if (selected_turret != nullptr) {
				auto layout = selected_turret->getInformationLayout(this);
				addChild(layout, 1, "information_tower");
				layout->setScale(0);
				auto scale_to = ScaleTo::create(0.125f, 1.f);
				layout->runAction(scale_to);
			}
		});
		getChildByName("information_tower")->runAction(Sequence::create(scale_to, removeAndCreateLayout, nullptr));
	}
	else {
		auto layout = selected_turret->getInformationLayout(this);
		addChild(layout, 1, "information_tower");
		layout->setScale(0);
		auto scale_to = ScaleTo::create(0.125f, 1.f);
		layout->runAction(scale_to);
	}
}

void InterfaceGame::hideTowerInfo() {
	if (getChildByName("information_tower") != nullptr) {
		auto scale_to = ScaleTo::create(0.125f, 0.f);
		auto removeAndCreateLayout = CallFunc::create([&]() {
			removeChildByName("information_tower");
		});
		getChildByName("information_tower")->runAction(Sequence::create(scale_to, removeAndCreateLayout, nullptr));
	}
	displayStartMenuIfInTitleState();
}

void InterfaceGame::hideStartMenu()
{
	((StartMenu*)getChildByName("start"))->hide();
}

void InterfaceGame::updateButtonDisplay() {
	for (auto& tower : towers_menu) {
		auto cost_label = (Label*)tower.second.first->getChildByName("cost");
		double cost = tower.second.second;
		double quantity = game->getLevel()->getQuantity();
		if (quantity < cost) {
			if (cost_label->getColor() != Color3B::RED) {
				cost_label->setColor(Color3B::RED);
				cost_label->enableOutline(Color4B::BLACK, 1.0);
			}
			if (!tower.second.first->getChildByName("disable")->isVisible()) {
				tower.second.first->getChildByName("disable")->setVisible(true);
				tower.second.first->getChildByName("sprite")->setColor(Color3B::GRAY);
			}
		}
		else {
			if (cost_label->getColor() != Color3B::GREEN) {
				cost_label->setColor(Color3B::GREEN);
				cost_label->enableOutline(Color4B::BLACK, 1.0);
			}
			if (tower.second.first->getChildByName("disable")->isVisible()) {
				tower.second.first->getChildByName("disable")->setVisible(false);
				tower.second.first->getChildByName("sprite")->setColor(Color3B::WHITE);
			}
		}
	}
	challenges->update();
}

void InterfaceGame::updateObjectDisplay(float dt) {
	if (selected_turret != nullptr) {
		selected_turret->updateDisplay(dt);
	}
}

std::pair<std::string, cocos2d::Sprite*> InterfaceGame::getTowerFromPoint(cocos2d::Vec2 location) {
	cocos2d::Vec2 origin = getChildByName("menu_panel")->getBoundingBox().origin +
		getChildByName("menu_panel")->getChildByName("towers")->getPosition();
	for (auto& item : towers_menu) {
		Vec2 pointInSprite = location - item.second.first->getPosition() - origin;
		pointInSprite.x += item.second.first->getSpriteFrame()->getRect().size.width *
			item.second.first->getScale() / 2;
		pointInSprite.y += item.second.first->getSpriteFrame()->getRect().size.height *
			item.second.first->getScale() / 2;
		Rect itemRect = Rect(0, 0, item.second.first->getSpriteFrame()->getRect().size.width *
			item.second.first->getScale(),
			item.second.first->getSpriteFrame()->getRect().size.height * item.second.first->getScale());
		if (itemRect.containsPoint(pointInSprite)) {
			return std::make_pair(item.first, item.second.first);
		}
	}
	std::pair<std::string, Sprite*> item;
	item.first = "nullptr";
	return item;
}

InterfaceGame::GameState InterfaceGame::getGameState() const {
	return game_state;
}
cocos2d::Vec2 InterfaceGame::getAbsoluteMenuTowerPosition(std::string towerName)
{
	return towers_menu[towerName].first->getPosition() - cocos2d::Vec2(0, sizeTower) + getChildByName("menu_panel")->getChildByName("towers")->getPosition() + getChildByName("menu_panel")->getPosition();
}
Sprite * InterfaceGame::getMenuTower(std::string towerName)
{
	return towers_menu[towerName].first;
}
void InterfaceGame::setGameState(GameState g_state) {
	game_state = g_state;
}

void InterfaceGame::setSelectedTower(Tower * tower)
{
	selected_turret = tower;
}

void InterfaceGame::setListening(bool listening) {
	listener->setEnabled(listening);
}

void InterfaceGame::generateHolySugar(Vec2 pos) {
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

void InterfaceGame::startRewarding(Vec2 pos) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();
	Json::Value rootSave = ((AppDelegate*)Application::getInstance())->getConfigClass()->getSaveValues();
	Json::Value level_config = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::LEVEL)["worlds"]
		[game->getLevel()->getWorldId()]["levels"][game->getLevel()->getLevelId()];

	challenges->endChallengeHandler();
	int successfull_challenges = challenges->countSucceedChallenges();
	if (successfull_challenges > 0) {
		Sprite* star_left = Sprite::create("res/levels/rewards/star_full.png");
		star_left->setPosition(Vec2(star_left->getContentSize().width / 2, star_left->getContentSize().height / 2));
		((Sprite*)getChildByName("menu_win")->getChildByName("star_left"))->addChild(star_left);
		if (successfull_challenges > 1) {
			Sprite* star_middle = Sprite::create("res/levels/rewards/star_full.png");
			star_middle->setPosition(Vec2(star_middle->getContentSize().width / 2, star_middle->getContentSize().height / 2));
			((Sprite*)getChildByName("menu_win")->getChildByName("star_middle"))->addChild(star_middle);
			if (successfull_challenges > 2) {
				Sprite* star_right = Sprite::create("res/levels/rewards/star_full.png");
				star_right->setPosition(Vec2(star_right->getContentSize().width / 2, star_right->getContentSize().height / 2));
				((Sprite*)getChildByName("menu_win")->getChildByName("star_right"))->addChild(star_right);
			}
		}
	}


	if ((int)game->getLevel()->getWorldId() >= rootSave["c_world"].asInt() &&
		(int)game->getLevel()->getLevelId() >= rootSave["c_level"].asInt()) {
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
				((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::BUTTON)
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

void InterfaceGame::initDialoguesFromLevel(const Json::Value& config) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto fileUtils = FileUtils::getInstance();

	Json::Reader reader;
	Json::Value dialogue_json;
	bool parsingConfigSuccessful = reader.parse(fileUtils->getStringFromFile(
		((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::LEVEL)["worlds"]
		[game->getLevel()->getWorldId()]["levels"][game->getLevel()->getLevelId()]["dialogues_file"].asString()), dialogue_json, false);

	bool play_dialogue = ((AppDelegate*)Application::getInstance())->getConfigClass()->isDialoguesEnabled();
	if (dialogue_json["introDialogue"].size() != 0 && play_dialogue) {
		dialogues = Dialogue::createFromConfig(dialogue_json["introDialogue"]);
		addChild(dialogues, 1, "dialogue");
		dialogues->launch();
		game_state = INTRO;
	}
	else {
		game_state = TITLE;
		((StartMenu*)getChildByName("start"))->displayWithAnimation();
		dialogues = nullptr;
	}
}

Dango* InterfaceGame::getCurrentDango() {
	return selected_dango;
}

int InterfaceGame::getSugarQuantity()
{
	return game->getLevel()->getQuantity();
}

int InterfaceGame::getLifeQuantity()
{
	return game->getLevel()->getLife();
}

void InterfaceGame::handleDeadDango() {
	hideDangoInfo();
	selected_dango = nullptr;
	state = IDLE;
}

void InterfaceGame::updateIncrementXP(Label* exp_label, ui::LoadingBar* loading_bar, std::string tower_name,
	float* increment, int initial_value, int diff_exp, int loop, int max_level) {
	Json::Value root = ((AppDelegate*)Application::getInstance())->getSave();
	Json::Value config = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::TOWER);

	int c_exp = initial_value;
	float duration = 4.0 / ((float)loop + 1.0);
	float speed = 0.05f;
	int nb_iterations = duration / speed;

	if (root["towers"][tower_name]["unlocked"].asBool()) {
		c_exp += (*increment);
		while (max_level < (int)config[tower_name]["xp_level"].size() &&
			c_exp > config[tower_name]["xp_level"][max_level + 1].asInt()) {
			c_exp -= config[tower_name]["xp_level"][max_level + 1].asInt();
			++max_level;
		}
		if (initial_value != 0 && c_exp - (*increment) <= 0) {
			getChildByName("menu_win")->getChildByName(tower_name + "_levelup")->setVisible(true);
			getChildByName("menu_win")->getChildByName(tower_name + "_levelup")->runAction(Sequence::create(ScaleTo::create(0.125f, 1.5f),
				ScaleTo::create(0.125f, 1.f), nullptr));
		}
	}
	loading_bar->setPercent(100 * (float)c_exp / Tower::getConfig()[tower_name]["xp_level"][max_level + 1].asDouble());
	exp_label->setString("+" + Value(diff_exp - (int)(*increment)).asString());
	if (exp_label->getString() != "+0") {
		(*increment) += (float)diff_exp / (float)nb_iterations;
		if ((*increment) > diff_exp) {
			(*increment) = diff_exp;
		}
		auto incrementExp = CallFunc::create([this, exp_label, loading_bar, tower_name,
			increment, initial_value, diff_exp, loop, max_level]() {
			updateIncrementXP(exp_label, loading_bar, tower_name, increment, initial_value,
				diff_exp, loop, max_level);
		});
		exp_label->runAction(Sequence::create(DelayTime::create(0.04f), incrementExp, nullptr));
	}
	else if (loop == 0) {
		++loop;
		initial_value = c_exp;
		diff_exp = game->getLevel()->getTotalExperience();
		exp_label->setString("+" + Value(diff_exp).asString());
		exp_label->setColor(Color3B::YELLOW);
		(*increment) = 0;
		auto incrementExp = CallFunc::create([this, exp_label, loading_bar, tower_name,
			increment, initial_value, diff_exp, loop, max_level]() {
			updateIncrementXP(exp_label, loading_bar, tower_name, increment, initial_value,
				diff_exp, loop, max_level);
		});
		exp_label->runAction(Sequence::create(ScaleTo::create(0.25f, 1.5f),
			ScaleTo::create(0.25f, 1.f), DelayTime::create(0.04f), incrementExp, nullptr));
	}
}

void InterfaceGame::pauseLevel()
{
	game->getLevel()->pause();
}

void InterfaceGame::resumeLevel()
{
	game->getLevel()->resume();
}

void InterfaceGame::shakeElement(Node* element, bool loop) {
	RotateTo* left = RotateTo::create(0.1f, 15);
	RotateTo* right = RotateTo::create(0.1f, -15);
	RotateTo* center = RotateTo::create(0.2f, 0);
	Sequence* seq = Sequence::create(
		left, right,
		left->clone(), right->clone(),
		left->clone(), right->clone(),
		center, DelayTime::create(1.f),
		nullptr);
	if (loop) {
		element->runAction(RepeatForever::create(seq));
	}
	else {
		element->runAction(seq);
	}

}

void InterfaceGame::shakeScaleElement(Node* element, bool loop) {
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

void InterfaceGame::createTowersLayout() {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	ui::Layout* towers = (ui::Layout*)getChildByName("menu_panel")->getChildByName("towers");
	auto save = ((AppDelegate*)Application::getInstance())->getSave()["towers"];

	int j(0);
	std::vector<std::string> tower_names = Tower::getConfig().getMemberNames();

	for (unsigned int i(0); i < tower_names.size(); ++i) {
		if (save[save.getMemberNames()[i]]["unlocked"].asBool()) {
			Json::Value tower_config = Tower::getConfig();
			std::string sprite3_filename = tower_config[tower_names[i]]["image"].asString();
			//std::string sprite3_background_filename = "res/buttons/tower_button.png";
			std::string sprite3_background_filename = "res/buttons/tower_button2.png";
			//std::string sprite3_disable_filename = "res/buttons/tower_inactive.png";
			std::string sprite3_disable_filename = "res/buttons/tower_button2_disabled.png";
			std::string sprite3_clip_filename = "res/buttons/tower_button_clip.png";
			std::string sprite7_active_filename = "res/buttons/tower_active.png";

			auto tower = Sprite::create(sprite3_background_filename);
			Sprite* inactive_background = Sprite::create(sprite3_disable_filename);
			inactive_background->setScale(tower->getContentSize().width / inactive_background->getContentSize().width);
			inactive_background->setPosition(Vec2(tower->getContentSize().width / 2,
				tower->getContentSize().height / 2));
			tower->addChild(inactive_background, 4, "disable");
			tower->addChild(Sprite::create(sprite3_filename), 5, "sprite");
			towers->addChild(tower, 1, Value(tower_names[i]).asString());
			tower->setScale(sizeTower / tower->getContentSize().width);
			//tower->setScaleY(sizeTower / tower->getContentSize().height);
			tower->getChildByName("sprite")->setScale(tower->getContentSize().width * 0.75 /
				tower->getChildByName("sprite")->getContentSize().width);
			tower->setPosition(Vec2(sizeTower / 2 + j % 2 * (sizeTower + towers->getContentSize().width / 20) +
				towers->getContentSize().width / 40,
				-tower->getContentSize().height * tower->getScale() / 2 - (j / 2) *
				(tower->getContentSize().height * tower->getScale() + towers->getContentSize().width / 20) -
				towers->getContentSize().width / 20));
			tower->getChildByName("sprite")->setPosition(Vec2(tower->getContentSize().width / 2,
				tower->getContentSize().height * 0.55));
			Label* cost = Label::createWithTTF(tower_config[tower_names[i]]["cost"][0].asString(),
				"fonts/LICABOLD.ttf", 25);
			cost->setColor(Color3B::RED);
			cost->setPosition(Vec2(tower->getContentSize().width / 2 + cost->getContentSize().width / 2,
				cost->getContentSize().height));
			tower->addChild(cost, 6, "cost");
			Sprite* sugar = Sprite::create("res/buttons/sugar.png");
			sugar->setScale(tower->getContentSize().width / 5 / sugar->getContentSize().width);
			sugar->setPosition(Vec2(cost->getPosition().x - cost->getContentSize().width / 2 - sugar->getContentSize().width * sugar->getScale() / 2,
				cost->getPosition().y));
			tower->addChild(sugar, 6, "sugar");

			towers_menu[tower_names[i]] = std::make_pair(tower, tower_config[tower_names[i]]["cost"][0].asDouble());
			++j;
		}
	}
}

void InterfaceGame::endGame() {
	setGameState(DONE);
	setListening(false);
}