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
#include "../Config/Config.h"


USING_NS_CC;

InterfaceGame::InterfaceGame() : sizeButton(cocos2d::Director::getInstance()->getVisibleSize().width / 15),
sizeTower(cocos2d::Director::getInstance()->getVisibleSize().width / 10), tutorial_running(false), startMenu(nullptr)
{}

InterfaceGame::~InterfaceGame() {
	if (dialogues != nullptr) {
		delete dialogues;
		log("deleted dialogue");
	}
	cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
}

bool InterfaceGame::init() {

	if (!Layer::init()) { return false; }
	Size visibleSize = Director::getInstance()->getVisibleSize();

	state = State::IDLE;
	game_state = INTRO;
	configClass = ((AppDelegate*)Application::getInstance())->getConfigClass();
	Json::Value config = configClass->getConfigValues(Config::ConfigType::GENERAL);

	initParametersMenu(config);
	initLoseMenu(configClass->getSettings()->getLanguage(), configClass->getConfigValues(Config::ConfigType::BUTTON), configClass->getConfigValues(Config::ConfigType::ADVICE));
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
	if (configClass->getSettings()->isMovingGridEnabled()) {
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
			if (configClass->getSettings()->isMovingGridEnabled()) {
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
			if (configClass->getSettings()->isMovingGridEnabled()) {
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
			startMenu->displayWithAnimation();
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
	loseMenu->showLose();
	setGameState(GameState::ENDING);
	getChildByName("black_mask")->setVisible(true);
}

void InterfaceGame::showWin() {
	winMenu->showWin();
	getChildByName("black_mask")->setVisible(true);
}

void InterfaceGame::reset() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Json::Value config = configClass->getConfigValues(Config::ConfigType::GENERAL);


	state = IDLE;
	game_state = TITLE;
	selected_turret = nullptr;
	selected_dango = nullptr;
	tutorial_running = false;

	getChildByName("menu_panel")->getChildByName("informations")->setVisible(false);
	((ui::CheckBox*)getChildByName("menu_panel")->getChildByName("speed_up"))->setSelected(false);
	loseMenu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 1.5));
	winMenu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 1.5));
	//startMenu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 1.5));
	getChildByName("black_mask")->setVisible(false);

	startMenu->reset(game->getLevel()->getLevelId());
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
	ui::Layout* menu_pause = ParametersMenu::create(game, configClass);
	addChild(menu_pause, 4, "menu_pause");
}

void InterfaceGame::initStartMenu(const Json::Value& config) {
	startMenu = StartMenu::create(this, game->getLevel()->getLevelId());
	addChild(startMenu, 2, "start");
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
	challenges = ChallengeHandler::create(configClass->getConfigValues(Config::ConfigType::LEVEL)["worlds"]
		[game->getLevel()->getWorldId()]["levels"][game->getLevel()->getLevelId()]["challenges"]);
	challenges->setPosition(Vec2(sugar->getPosition().x + sugar->getContentSize().width * sugar->getScale() / 2,
		node_top->getPosition().y + sugar->getPosition().y - sugar->getContentSize().height * sugar->getScale()));
	addChild(challenges, 1, "challenges");

	addChild(node_top, 2, "label_information");
}

void InterfaceGame::initLoseMenu(const std::string& language, const Json::Value& buttons, const Json::Value& advice) {
	loseMenu = LoseMenu::create(game, language, buttons, advice);
	addChild(loseMenu, 4, "menu_lose");
}

void InterfaceGame::initWinMenu(const Json::Value& config) {
	winMenu = WinMenu::create(game);
	addChild(winMenu, 4, "menu_win");
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
			if (!speed_up->isSelected()) {
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
			configClass->getSettings()->getLanguage()].asString());
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
		SceneManager::getInstance()->setScene(SceneFactory::LEVELS);
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
		startMenu->displayWithAnimation();
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
	startMenu->hide();
}

void InterfaceGame::lockStartMenu()
{
	startMenu->lock(true);
}

void InterfaceGame::unlockStartMenu()
{
	startMenu->lock(false);
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
	std::string language = configClass->getSettings()->getLanguage();
	Json::Value rootSave = configClass->getSaveValues();
	Json::Value level_config = configClass->getConfigValues(Config::ConfigType::LEVEL)["worlds"]
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

void InterfaceGame::initDialoguesFromLevel(const Json::Value& config) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto fileUtils = FileUtils::getInstance();

	Json::Reader reader;
	Json::Value dialogue_json;
	bool parsingConfigSuccessful = reader.parse(fileUtils->getStringFromFile(
		configClass->getConfigValues(Config::ConfigType::LEVEL)["worlds"]
		[game->getLevel()->getWorldId()]["levels"][game->getLevel()->getLevelId()]["dialogues_file"].asString()), dialogue_json, false);

	bool play_dialogue = configClass->getSettings()->isDialoguesEnabled();
	if (dialogue_json["introDialogue"].size() != 0 && play_dialogue) {
		dialogues = Dialogue::createFromConfig(dialogue_json["introDialogue"]);
		addChild(dialogues, 1, "dialogue");
		dialogues->launch();
		game_state = INTRO;
	}
	else {
		game_state = TITLE;
		startMenu->displayWithAnimation();
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

void InterfaceGame::pauseLevel()
{
	game->getLevel()->pause();
}

void InterfaceGame::resumeLevel()
{
	game->getLevel()->resume();
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
	Json::Value tower_config = Tower::getConfig();
	for (unsigned int i(0); i < tower_names.size(); ++i) {
		if (save[save.getMemberNames()[i]]["unlocked"].asBool()) {
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