#include "InterfaceGame.h"
#include "SceneManager.h"
#include "AppDelegate.h"
#include "MyGame.h"
#include "Towers/Cutter.h"
#include "Towers/Scorpion.h"
#include "Lib/Functions.h"
#include "Lib/AudioSlider.h"
#include "Config/AudioController.h"
#include <iostream>
#include <string>
#include <sstream>


USING_NS_CC;

InterfaceGame::InterfaceGame() : sizeButton(cocos2d::Director::getInstance()->getVisibleSize().width / 15),
	sizeTower(cocos2d::Director::getInstance()->getVisibleSize().width / 10)
{}
InterfaceGame::~InterfaceGame() {
	if (dialogues != nullptr) {
		delete dialogues;
		log("deleted dialogue");
	}
}

bool InterfaceGame::init(){

	if (!Layer::init()){ return false; }
	Size visibleSize = Director::getInstance()->getVisibleSize();

	state = State::IDLE;

	initParametersMenu();
	initLoseMenu();
	initWinMenu();
	initRightPanel();
	initLabels();
	initStartMenu();
	initDialoguesFromLevel();

	game_state = INTRO;

	addChild(ui::Layout::create(), 3, "black_mask");
	ui::Button* mask = ui::Button::create("res/buttons/mask.png");
	mask->setScaleX(visibleSize.width / mask->getContentSize().width);
	mask->setScaleY(visibleSize.height / mask->getContentSize().height);
	mask->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	getChildByName("black_mask")->addChild(mask);
	getChildByName("black_mask")->setVisible(false);

	selectedTurret = nullptr;

	addEvents();

	return true;
}

InterfaceGame* InterfaceGame::create(MyGame* ngame, int id_level){

	InterfaceGame* interface_game = new InterfaceGame();
	interface_game->setGame(ngame);
	interface_game->setLevel(id_level);

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


void InterfaceGame::addEvents()
{
    listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [&](cocos2d::Touch* touch, cocos2d::Event* event){
        cocos2d::Vec2 p = touch->getLocation();
        cocos2d::Rect rect = this->getBoundingBox();
        cocos2d::Rect rectrightpanel = getChildByName("menu_panel")->getChildByName("panel")->getBoundingBox();
        rectrightpanel.origin += getChildByName("menu_panel")->getBoundingBox().origin;

        if(rect.containsPoint(p)){
			if(rectrightpanel.containsPoint(p)){
				auto item = getTowerFromPoint(touch->getStartLocation());
				if (item.first != "nullptr"){
					if (state == TURRET_SELECTED){
						selectedTurret->displayRange(false);
						hideTowerInfo();
						selectedTurret = nullptr;
					}
					else if(state == TURRET_CHOSEN){
						if(selectedTurret != nullptr){
							removeTower();
						}
					}
					state = State::TOUCHED;
					displayTowerInfos(item.first);
					if (game_state == TITLE) {
						getChildByName("start")->setVisible(false);
						getChildByName("title")->setVisible(false);
					}
				}
				else{
					if(selectedTurret != nullptr && state == TURRET_CHOSEN){
						getChildByName("menu_panel")->getChildByName("informations")->setVisible(false);
						removeTower();
						state = IDLE;
					}
					else if (state == TURRET_SELECTED){
						getChildByName("menu_panel")->getChildByName("informations")->setVisible(false);
						selectedTurret->displayRange(false);
						hideTowerInfo();
						selectedTurret = nullptr;
					}
					state = State::IDLE;
				}
			}
			else{
				Tower* tower = game->getLevel()->touchingTower(p);
				if (tower != nullptr && state != TURRET_CHOSEN){
					if (state == TURRET_SELECTED){
						selectedTurret->displayRange(false);
					}
					getChildByName("menu_panel")->getChildByName("informations")->setVisible(true);
					selectedTurret = tower;
					state = TURRET_SELECTED;
					if (game_state == TITLE) {
						getChildByName("start")->setVisible(false);
						getChildByName("title")->setVisible(false);
					}
				}
				else if (tower == nullptr && state == TURRET_SELECTED ){
					state = IDLE;
					getChildByName("menu_panel")->getChildByName("informations")->setVisible(false);
					selectedTurret->displayRange(false);
					hideTowerInfo();
					selectedTurret = nullptr;
					if (game_state == TITLE) {
						getChildByName("start")->setVisible(true);
						getChildByName("title")->setVisible(true);
					}
				}
				else if(state == TOUCHED){ 
					auto item = getTowerFromPoint(touch->getStartLocation());
					if (item.first != "nullptr"){
						if (game->getLevel()->getQuantity() >= Tower::getConfig()[item.first]["cost"][0].asDouble()){
							state = State::TURRET_CHOSEN;
							Tower::TowerType tape = Tower::getTowerTypeFromString(item.first);
							menuTurretTouchCallback(Tower::getTowerTypeFromString(item.first));
							moveSelectedTurret(touch->getLocation());
							selectedTurret->displayRange(true);
							selectedTurret->setVisible(true);
							if (game_state == TITLE) {
								getChildByName("start")->setVisible(false);
								getChildByName("title")->setVisible(false);
							}
						}
					}
				}
				else if (selectedTurret != nullptr && !game->isPaused() && state == TURRET_CHOSEN){
					Vec2 pos = touch->getLocation();
					moveSelectedTurret(pos);
					selectedTurret->setVisible(true);
				}
			}
			return true; // to indicate that we have consumed it.
        }
        return false; // we did not consume this event, pass thru.
    };

    listener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event){
		cocos2d::Vec2 p = touch->getLocation();
		cocos2d::Rect rectrightpanel = getChildByName("menu_panel")->getChildByName("panel")->getBoundingBox();
        rectrightpanel.origin += getChildByName("menu_panel")->getBoundingBox().origin;
		if (state == TURRET_SELECTED){
			Tower* tower = game->getLevel()->touchingTower(p);
			if (tower != nullptr){
				std::string name = tower->getSpecConfig()["name"].asString();
				displayTowerInfos(tower->getSpecConfig()["name"].asString());
				selectedTurret->displayRange(true);
				showTowerInfo();
			}
			else{
				getChildByName("menu_panel")->getChildByName("informations")->setVisible(false);
				selectedTurret->displayRange(false);
				hideTowerInfo();
				selectedTurret = nullptr;
				state = IDLE;
				if (game_state == TITLE) {
					getChildByName("start")->setVisible(true);
					getChildByName("title")->setVisible(true);
				}
			}
		}
		else if(state == TURRET_CHOSEN){
			if( !rectrightpanel.containsPoint(p)){
				if(!isOnTower(p)){
					builtCallback(nullptr);
					state = State::IDLE;
					selectedTurret->displayRange(false);
					selectedTurret = nullptr;
					if (game_state == TITLE) {
						getChildByName("start")->setVisible(true);
						getChildByName("title")->setVisible(true);
					}
				}
			}
			else{
				removeTower();
				state = State::IDLE;
				if (game_state == TITLE) {
					getChildByName("start")->setVisible(true);
					getChildByName("title")->setVisible(true);
				}
			}
		}
		else if(state == TOUCHED){
			if (game_state == TITLE) {
				getChildByName("start")->setVisible(true);
				getChildByName("title")->setVisible(true);
			}
			if(selectedTurret != nullptr){
				destroyCallback(nullptr);
			}
			auto item = getTowerFromPoint(p);
			if (item.first != "nullptr"){
				if (game->getLevel()->getQuantity() >= Tower::getConfig()[item.first]["cost"][0].asDouble()){
					state = State::TURRET_CHOSEN;
					Tower::TowerType tape = Tower::getTowerTypeFromString(item.first);
					menuTurretTouchCallback(Tower::getTowerTypeFromString(item.first));
				}
				else{
					state = IDLE;
				}
			}
		}
    };

    listener->onTouchMoved = [&](cocos2d::Touch* touch, cocos2d::Event* event){
		if(state == State::TOUCHED){
			Size visibleSize = Director::getInstance()->getVisibleSize();

			double dist = touch->getLocation().distanceSquared(touch->getStartLocation());
			if (touch->getLocation().x - visibleSize.width * 3 / 4 < 0) {
				auto item = getTowerFromPoint(touch->getStartLocation());
				if (item.first != "nullptr") {
					if (game->getLevel()->getQuantity() >= Tower::getConfig()[item.first]["cost"][0].asDouble()) {
						state = State::TURRET_CHOSEN;
						Tower::TowerType tape = Tower::getTowerTypeFromString(item.first);
						menuTurretTouchCallback(Tower::getTowerTypeFromString(item.first));
						moveSelectedTurret(touch->getLocation());
						selectedTurret->displayRange(true);
						selectedTurret->setVisible(true);
					}
				}
			}
			/*if(dist > 100){
				
			}*/
		}
		else if(selectedTurret != nullptr && !game->isPaused() &&
			state == State::TURRET_CHOSEN){
			Vec2 pos = touch->getLocation();
			moveSelectedTurret(pos);
			selectedTurret->displayRange(true);
		}
	};
	//cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 30);
}

void InterfaceGame::setGame(MyGame* ngame){
	game = ngame;
}

void InterfaceGame::setLevel(int id_level) {
	id = id_level;
}

void InterfaceGame::accelerateOnOffCallback(Ref* sender){
	if (game->isAccelerated()){
		game->pause();
		game->setNormalSpeed();
	}
	else{
		game->resume();
		game->increaseSpeed();
	}
}

void InterfaceGame::update(float dt){
	std::string sugarText = "x " + to_string(game->getLevel()->getQuantity());
	std::string lifeText = "x " + to_string(game->getLevel()->getLife());
	Label* sugar = (Label*)getChildByName("label_information")->getChildByName("sugar");
	Label* life = (Label*)getChildByName("label_information")->getChildByName("life");
	if (sugar->getString() != sugarText) {
		sugar->setString(sugarText);
		sugar->enableOutline(Color4B::BLACK, 1.0);
	}
	if (life->getString() != lifeText) {
		life->setString(lifeText);
		life->enableOutline(Color4B::BLACK, 1.0);
	}
	if (getChildByName("information_tower") != nullptr && selectedTurret != nullptr) {
		selectedTurret->updateInformationLayout((ui::Layout*)getChildByName("information_tower"));
	}
	switch (game_state) {
	case INTRO:
		dialogues->update();
		if (dialogues->hasFinished()) {
			Size visibleSize = Director::getInstance()->getVisibleSize();

			removeChild(dialogues);
			game_state = TITLE;
			getChildByName("start")->setVisible(true);
			getChildByName("title")->setVisible(true);
			getChildByName("title")->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(0.5f,Vec2(getChildByName("title")->getPosition().x,
				visibleSize.height/2))), DelayTime::create(1.f), FadeOut::create(0.5f),nullptr));
			getChildByName("start")->runAction(EaseBackOut::create(MoveTo::create(0.5f, Vec2(getChildByName("start")->getPosition().x,
				getChildByName("start")->getContentSize().height * getChildByName("start")->getScaleY()))));
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

void InterfaceGame::menuTurretTouchCallback(Tower::TowerType turret){
	if(selectedTurret == nullptr && !game->isPaused()){
		if (turret == Tower::TowerType::ARCHER){
			Scorpion* createdTurret = Scorpion::create();
			game->getLevel()->addTurret(createdTurret);
			selectedTurret = createdTurret;
		}
		else if (turret == Tower::TowerType::CUTTER){
			Cutter* createdTurret = Cutter::create();
			game->getLevel()->addTurret(createdTurret);
			selectedTurret = createdTurret;
		}
	}
}

void InterfaceGame::moveSelectedTurret(Vec2 pos){
	if(selectedTurret != nullptr){
		Cell* nearestCell = game->getLevel()->getNearestCell(selectedTurret->getPosition());
		Cell* nearestCell2 = game->getLevel()->getNearestCell(pos/game->getLevel()->getScale());
		if(nearestCell2 != nullptr && nearestCell != nullptr){
			if(nearestCell2->isFree() && !nearestCell2->isPath() && !nearestCell2->isOffLimit()){
				nearestCell->setObject(nullptr);
				nearestCell2->setObject(selectedTurret);
				selectedTurret->setPosition(nearestCell2->getPosition());
			}
		}
	}
}
bool InterfaceGame::isOnTower(Vec2 pos){
	Cell* nearestCell = game->getLevel()->getNearestCell(pos/game->getLevel()->getScale());
	if(nearestCell != nullptr){
		if((nearestCell->isFree()|| nearestCell->getObject() == selectedTurret) && !nearestCell->isPath() && !nearestCell->isOffLimit()){
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

void InterfaceGame::showWin(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto* showAction = TargetedAction::create(getChildByName("menu_win"),
		EaseBackOut::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height / 2))));
	getChildByName("menu_win")->runAction(showAction);
	getChildByName("black_mask")->setVisible(true);
}

void InterfaceGame::reset(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	state = IDLE;
	game_state = INTRO;
	selectedTurret = nullptr;
	((AudioSlider*)getChildByName("menu_pause")->getChildByName("MusicVolume"))->enable(false);
	((AudioSlider*)getChildByName("menu_pause")->getChildByName("EffectsVolume"))->enable(false);

	getChildByName("menu_panel")->getChildByName("informations")->setVisible(false);
	getChildByName("menu_lose")->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 1.5));
	getChildByName("menu_win")->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 1.5));
	getChildByName("menu_pause")->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 1.5));
	getChildByName("black_mask")->setVisible(false);
	getChildByName("start")->setPosition(Vec2(getChildByName("start")->getPosition().x, 
		-getChildByName("start")->getContentSize().height * getChildByName("start")->getScaleY()));
	getChildByName("title")->setPosition(Vec2(getChildByName("title")->getPosition().x,
		visibleSize.height + getChildByName("title")->getContentSize().height * getChildByName("title")->getScaleY()));
	((Label*)getChildByName("title"))->runAction(FadeIn::create(0.5f));
	removeChildByName("reward_layout");
	removeChildByName("information_tower");
	if (dialogues != nullptr) {
		removeChild(dialogues,1);
		delete dialogues;
		initDialoguesFromLevel();
	}
}

void InterfaceGame::initParametersMenu(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	ui::Layout* menu_pause = ui::Layout::create();
	menu_pause->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 1.5));

	ui::Button* panel = ui::Button::create("res/buttons/centralMenuPanel.png");
	panel->setZoomScale(0);
	menu_pause->addChild(panel, 1, "panel");
	panel->setScaleX(0.45*visibleSize.width / panel->getContentSize().width);
	panel->setScaleY(0.45*visibleSize.width / panel->getContentSize().width);

	Label* title = Label::createWithTTF("Settings", "fonts/LICABOLD.ttf", 45.0f);
	title->setColor(Color3B::BLACK);
	title->setPosition(0, panel->getContentSize().height*panel->getScaleY() / 2 - title->getContentSize().height);
	menu_pause->addChild(title, 2, "title");

	ui::Button* resume = ui::Button::create("res/buttons/buttonResume.png");
	resume->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Size visibleSize = Director::getInstance()->getVisibleSize();
			((AudioSlider*)getChildByName("menu_pause")->getChildByName("MusicVolume"))->enable(false);
			((AudioSlider*)getChildByName("menu_pause")->getChildByName("EffectsVolume"))->enable(false);
			auto* hideAction = TargetedAction::create(getChildByName("menu_pause"),
				EaseBackIn::create(MoveTo::create(0.5f,Vec2(visibleSize.width/2,visibleSize.height * 1.5))));
			getChildByName("menu_pause")->runAction(hideAction);
			getChildByName("black_mask")->setVisible(false);
			game->resume();
		}
	});
	resume->setScale(panel->getContentSize().width*panel->getScaleX() / 3 / resume->getContentSize().width);
	resume->setPosition(Vec2(-panel->getContentSize().width*panel->getScaleX() / 4,
		-panel->getContentSize().height*panel->getScaleY() / 2 +
		resume->getContentSize().height*resume->getScaleY() / 4));
	menu_pause->addChild(resume, 1, "resume");

	ui::Button* main_menu_back = ui::Button::create("res/buttons/buttonMainMenu.png");
	main_menu_back->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			mainMenuCallBack("menu_pause");
		}
	});
	main_menu_back->setScale(panel->getContentSize().width*panel->getScaleX() / 3 / main_menu_back->getContentSize().width);
	main_menu_back->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() / 4,
		-panel->getContentSize().height*panel->getScaleY() / 2 +
		main_menu_back->getContentSize().height*main_menu_back->getScaleY() / 4));
	menu_pause->addChild(main_menu_back, 1, "main_menu_back");

	Label* music = Label::createWithTTF("Music", "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	Label* effects = Label::createWithTTF("Effects", "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	Label* loop = Label::createWithTTF("Loop Music", "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	music->setColor(Color3B::BLACK);
	effects->setColor(Color3B::BLACK);
	loop->setColor(Color3B::BLACK);
	music->setPosition(-panel->getContentSize().width*panel->getScaleX() * 2 / 5 +
		music->getContentSize().width / 2, panel->getContentSize().height*panel->getScaleX() * 2 / 12);
	effects->setPosition(-panel->getContentSize().width*panel->getScaleX() * 2 / 5 +
		effects->getContentSize().width / 2, 0);
	loop->setPosition(-panel->getContentSize().width*panel->getScaleX() * 2 / 5 +
		loop->getContentSize().width / 2, -panel->getContentSize().height*panel->getScaleX() * 2 / 12);
	menu_pause->addChild(music, 2);
	menu_pause->addChild(effects, 2);
	menu_pause->addChild(loop, 2);
	
	auto checkbox_music = cocos2d::ui::CheckBox::create("res/buttons/music.png", "res/buttons/music.png",
		"res/buttons/disable.png", "res/buttons/music.png", "res/buttons/music.png");
	auto checkbox_effects = cocos2d::ui::CheckBox::create("res/buttons/music.png", "res/buttons/music.png",
		"res/buttons/disable.png", "res/buttons/music.png", "res/buttons/music.png");
	auto checkbox_loop = cocos2d::ui::CheckBox::create("res/buttons/checkbox.png", "res/buttons/checkbox.png",
		"res/buttons/filled.png", "res/buttons/checkbox.png", "res/buttons/checkbox.png");	

	AudioSlider* sliderMusicVolume = AudioSlider::create(AudioSlider::Horizontal);
	sliderMusicVolume->setValue(0, 1, ((AppDelegate*)Application::getInstance())->getAudioController()
		->getMaxMusicVolume());
	sliderMusicVolume->setPosition(panel->getContentSize().width*panel->getScaleX()/7,
		panel->getContentSize().height*panel->getScaleX() * 2 / 12);
	((AppDelegate*)Application::getInstance())->addAudioSlider(sliderMusicVolume, 
		AudioController::SOUNDTYPE::MUSIC);
	sliderMusicVolume->setScale(visibleSize.width / 1280);
	sliderMusicVolume->enable(false);

	AudioSlider* sliderEffectsVolume = AudioSlider::create(AudioSlider::Horizontal);
	sliderEffectsVolume->setValue(0, 1, ((AppDelegate*)Application::getInstance())->getAudioController()
		->getMaxEffectsVolume());
	sliderEffectsVolume->setPosition(panel->getContentSize().width*panel->getScaleX()/7, 0);
	((AppDelegate*)Application::getInstance())->addAudioSlider(sliderEffectsVolume, 
		AudioController::SOUNDTYPE::EFFECT);
	sliderEffectsVolume->setScale(visibleSize.width / 1280);
	sliderEffectsVolume->enable(false);

	checkbox_music->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX()* 2 / 5,
		panel->getContentSize().height*panel->getScaleX() * 2 / 12));
	checkbox_music->setScale(visibleSize.width / 1280);
	checkbox_effects->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() * 2 / 5, 0));
	checkbox_effects->setScale(visibleSize.width / 1280);
	checkbox_loop->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() / 7, 
		-panel->getContentSize().height*panel->getScaleX() * 2 / 12));
	checkbox_loop->setScale(visibleSize.width / 1280);

	((AppDelegate*)Application::getInstance())->getAudioController()->addButton(checkbox_music, 
		AudioController::SOUNDTYPE::MUSIC);
	((AppDelegate*)Application::getInstance())->getAudioController()->addButton(checkbox_effects, 
		AudioController::SOUNDTYPE::EFFECT);
	((AppDelegate*)Application::getInstance())->getAudioController()->addButtonLoop(checkbox_loop);


	checkbox_music->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		switch (type){
			case ui::Widget::TouchEventType::ENDED:
				((AppDelegate*)Application::getInstance())->getAudioController()->enableMusic(
					!((cocos2d::ui::CheckBox*)sender)->isSelected());
				break;
		}
	});
	checkbox_effects->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		switch (type) {
		case ui::Widget::TouchEventType::ENDED:
			((AppDelegate*)Application::getInstance())->getAudioController()->enableEffects(
				!((cocos2d::ui::CheckBox*)sender)->isSelected());
			break;
		}
	});
	checkbox_loop->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		switch (type) {
		case ui::Widget::TouchEventType::ENDED:
			((AppDelegate*)Application::getInstance())->getAudioController()->enableLoop(
				((cocos2d::ui::CheckBox*)sender)->isSelected());
			break;
		}
	});

	menu_pause->addChild(sliderEffectsVolume,5,"EffectsVolume");
	menu_pause->addChild(sliderMusicVolume,5,"MusicVolume");
	menu_pause->addChild(checkbox_music, 6, "MusicEnable");
	menu_pause->addChild(checkbox_effects, 6, "EffectsEnable");
	menu_pause->addChild(checkbox_loop, 6, "LoopEnable");

	addChild(menu_pause, 4, "menu_pause");
}
void InterfaceGame::initStartMenu() {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	Label* title = Label::createWithTTF("Level " + Value((int)(id + 1)).asString(), "fonts/LICABOLD.ttf", round(visibleSize.width / 12.0));
	title->setColor(Color3B::YELLOW);
	title->enableOutline(Color4B::BLACK, 3);
	title->setPosition(round(visibleSize.width * 3 / 8.0), round(visibleSize.height));
	//title->setPosition(round(visibleSize.width * 3 / 8.0), round(visibleSize.height / 2.0));
	addChild(title, 2, "title");

	ui::Button* start = ui::Button::create("res/buttons/list_level_checkbox.png");
	start->setScale(visibleSize.width / 4 / start->getContentSize().width);
	start->setTitleText("START");
	start->setTitleFontName("fonts/LICABOLD.ttf");
	start->setTitleFontSize(75.f * visibleSize.width / 1280);
	start->setTitleColor(Color3B::YELLOW);
	start->setTitleAlignment(cocos2d::TextHAlignment::CENTER);
	start->setPosition(Vec2(title->getPosition().x, -start->getContentSize().height * start->getScaleY()));
	//start->setPosition(Vec2(title->getPosition().x, start->getContentSize().height * start->getScale()));
	start->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			setListening(true);
			getChildByName("start")->runAction(EaseBackIn::create(MoveTo::create(0.5f, Vec2(getChildByName("start")->getPosition().x,
				-getChildByName("start")->getContentSize().height * getChildByName("start")->getScaleY()))));
			getChildByName("title")->setVisible(false);
			this->setGameState(RUNNING);
		}
	});
	addChild(start, 2, "start");
}

void InterfaceGame::initLabels(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	ui::Layout* node_top = ui::Layout::create();
	node_top->setScale(0.75);
	node_top->setPosition(Vec2(0,visibleSize.height));

	Sprite* panel_top = Sprite::create("res/buttons/panel_top.png");
	panel_top->setScale(visibleSize.width / 3 / panel_top->getContentSize().width);
	panel_top->setPosition(Vec2(panel_top->getContentSize().width * panel_top->getScale() / 2,
		-panel_top->getContentSize().height * panel_top->getScale() / 2));
	node_top->addChild(panel_top, 2);
	Sprite* panel_top_shadow = Sprite::create("res/buttons/panel_top_shadow.png");
	panel_top_shadow->setScale(visibleSize.width / 3 / panel_top->getContentSize().width);
	panel_top_shadow->setPosition(Vec2(panel_top_shadow->getContentSize().width * panel_top_shadow->getScale() * 2 / 5,
		-panel_top_shadow->getContentSize().height * panel_top_shadow->getScale() * 2 / 5));
	//panel_top_shadow->setPosition(Point(-15, 15));
	node_top->addChild(panel_top_shadow, 1);

	Sprite* sugar = Sprite::create("res/buttons/sugar.png");
	sugar->setScale(sizeButton / sugar->getContentSize().width);
	sugar->setPosition(Point(sizeButton * 0.125,
		-panel_top->getContentSize().height * panel_top->getScale() / 2));
	sugar->setAnchorPoint(Vec2(0, 0.5f));
	node_top->addChild(sugar, 3);
	Sprite* life = Sprite::create("res/buttons/life.png");
	life->setPosition(Point(panel_top->getContentSize().width * panel_top->getScale() / 2,
		-panel_top->getContentSize().height * panel_top->getScale() / 2));
	life->setAnchorPoint(Vec2(0, 0.5f));
	life->setScale(sizeButton / life->getContentSize().width);
	node_top->addChild(life, 3);
	

	Label* label = Label::createWithTTF("", "fonts/LICABOLD.ttf", 56 * visibleSize.width / 1280);
	node_top->addChild(label, 3, "sugar");
	label->setPosition(Point(sugar->getPosition().x + sugar->getContentSize().width * sugar->getScale(), 
		-panel_top->getContentSize().height * panel_top->getScale() / 2));
	label->setAnchorPoint(Vec2(0, 0.5f));
	label->setColor(Color3B::BLACK);
	Label* label_life = Label::createWithTTF("", "fonts/LICABOLD.ttf", 56 * visibleSize.width / 1280);
	node_top->addChild(label_life, 3, "life");
	label_life->setPosition(Point(life->getPosition().x + life->getContentSize().width * life->getScale(),
		-panel_top->getContentSize().height * panel_top->getScale() / 2));
	label_life->setAnchorPoint(Vec2(0, 0.5f));
	label_life->setColor(Color3B::BLACK);

	
	
	addChild(node_top, 2, "label_information");
}

void InterfaceGame::initLoseMenu(){
	Color3B color1 = Color3B(255, 200, 51);
	Color4F grey(102 / 255.0f, 178 / 255.0f, 255 / 255.0f, 0.66f);
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto menu_lose = ui::Layout::create();
	menu_lose->setPosition(Vec2(Point(visibleSize.width / 2, visibleSize.height * 1.5)));

	ui::Button* panel = ui::Button::create("res/buttons/centralMenuPanel.png");
	panel->setZoomScale(0);
	menu_lose->addChild(panel, 1, "panel");
	panel->setScaleX(0.45*visibleSize.width / panel->getContentSize().width);
	panel->setScaleY(0.45*visibleSize.width / panel->getContentSize().width);

	ui::Button* retry = ui::Button::create("res/buttons/buttonRetry.png");
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
	retry->setScale(panel->getContentSize().width*panel->getScaleX() / 3 / retry->getContentSize().width);
	retry->setPosition(Vec2(-panel->getContentSize().width*panel->getScaleX() / 4,
		-panel->getContentSize().height*panel->getScaleY() / 2 +
		retry->getContentSize().height*retry->getScaleY() / 4));
	menu_lose->addChild(retry, 1, "retry");

	ui::Button* main_menu_back = ui::Button::create("res/buttons/buttonMainMenu.png");
	main_menu_back->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			mainMenuCallBack("menu_lose");
		}
	});
	main_menu_back->setScale(panel->getContentSize().width*panel->getScaleX() / 3 / main_menu_back->getContentSize().width);
	main_menu_back->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() / 4,
		-panel->getContentSize().height*panel->getScaleY() / 2 +
		main_menu_back->getContentSize().height*main_menu_back->getScaleY() / 4));
	menu_lose->addChild(main_menu_back, 1, "main_menu_back");

	Label* you_lose = Label::createWithTTF("Nice Try !", "fonts/LICABOLD.ttf", 50.f * visibleSize.width / 1280);
	you_lose->enableShadow(Color4B::BLACK, Size(0, 0), 1);
	you_lose->setPosition(0, 20);
	you_lose->setColor(Color3B::BLACK);
	menu_lose->addChild(you_lose, 2, "text");

	menu_lose->setVisible(true);
	addChild(menu_lose, 4, "menu_lose");
}

void InterfaceGame::initWinMenu(){
	Color3B color1 = Color3B(255, 200, 51);
	Color4F grey(102 / 255.0f, 178 / 255.0f, 255 / 255.0f, 0.66f);
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto menu_win = ui::Layout::create();
	menu_win->setPosition(Vec2(Point(visibleSize.width / 2, visibleSize.height * 1.5)));

	ui::Button* panel = ui::Button::create("res/buttons/centralMenuPanel.png");
	panel->setZoomScale(0);
	menu_win->addChild(panel, 1, "panel");
	panel->setScaleX(0.45*visibleSize.width / panel->getContentSize().width);
	panel->setScaleY(0.45*visibleSize.width / panel->getContentSize().width);

	ui::Button* next_level = ui::Button::create("res/buttons/buttonNextLevel.png");
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
	next_level->setScale(panel->getContentSize().width*panel->getScaleX() / 3 / next_level->getContentSize().width);
	next_level->setPosition(Vec2(-panel->getContentSize().width*panel->getScaleX() / 4,
		-panel->getContentSize().height*panel->getScaleY() / 2 +
		next_level->getContentSize().height*next_level->getScaleY() / 4));
	menu_win->addChild(next_level, 1, "next_level");

	ui::Button* main_menu_back = ui::Button::create("res/buttons/buttonMainMenu.png");
	main_menu_back->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			mainMenuCallBack("menu_win");
		}
	});
	main_menu_back->setScale(panel->getContentSize().width*panel->getScaleX() / 3 / main_menu_back->getContentSize().width);
	main_menu_back->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() / 4,
		-panel->getContentSize().height*panel->getScaleY() / 2 +
		main_menu_back->getContentSize().height*main_menu_back->getScaleY() / 4));
	menu_win->addChild(main_menu_back, 1, "main_menu_back");

	Label* you_win = Label::createWithTTF("Level Cleared !", "fonts/LICABOLD.ttf", 50.f * visibleSize.width / 1280);
	you_win->enableShadow(Color4B::BLACK, Size(0, 0), 1);
	you_win->setPosition(0, panel->getContentSize().height / 4);
	you_win->setColor(Color3B::BLACK);
	menu_win->addChild(you_win, 2, "text");

	Sprite* win_mallow = Sprite::create("res/buttons/win.png");
	win_mallow->setPosition(0, -panel->getContentSize().height / 4);
	win_mallow->setScale(panel->getContentSize().height / 4 / win_mallow->getContentSize().height);
	menu_win->addChild(win_mallow, 2, "image");
	Sprite* win_mallow2 = Sprite::create("res/buttons/win.png");
	win_mallow2->setScale(panel->getContentSize().height / 4 / win_mallow2->getContentSize().height);
	win_mallow2->setPosition(win_mallow2->getContentSize().width*win_mallow2->getScale(), 
		-panel->getContentSize().height / 4);
	menu_win->addChild(win_mallow2, 2, "image");
	Sprite* win_mallow3 = Sprite::create("res/buttons/win.png");
	win_mallow3->setScale(panel->getContentSize().height / 4 / win_mallow3->getContentSize().height);
	win_mallow3->setPosition(-win_mallow3->getContentSize().width*win_mallow3->getScale(),
		-panel->getContentSize().height / 4);
	menu_win->addChild(win_mallow3, 2, "image");
	
	addChild(menu_win, 4, "menu_win");
}

void InterfaceGame::initRightPanel(){
	Size visibleSize = Director::getInstance()->getVisibleSize();

	Layer* menu_panel = Layer::create();
	menu_panel->setPosition(Vec2(visibleSize.width * (3 / 4.0 + 1 / 8.0), visibleSize.height / 2));

	// PANEL FOR TOWERS, PARAMETERS AND INFOS
	auto panel = Sprite::create("res/buttons/menupanel.png");
	panel->setScaleY(visibleSize.height / panel->getContentSize().height);
	panel->setScaleX(visibleSize.width / panel->getContentSize().width / 4);
	menu_panel->addChild(panel, 2, "panel");

	// DISPLAY BUTTONS FOR GAME
	auto parameters = ui::Button::create("res/buttons/parameters_wood.png");
	parameters->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Size visibleSize = Director::getInstance()->getVisibleSize();
			((AudioSlider*)getChildByName("menu_pause")->getChildByName("MusicVolume"))->enable(true);
			((AudioSlider*)getChildByName("menu_pause")->getChildByName("EffectsVolume"))->enable(true);
			auto* showAction = TargetedAction::create(getChildByName("menu_pause"),
				EaseBackOut::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height / 2))));
			getChildByName("menu_pause")->runAction(showAction);
			getChildByName("black_mask")->setVisible(true);
			game->pause();
		}
	});
	parameters->setScale(1 / (parameters->getBoundingBox().size.width / sizeButton));
	parameters->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() / 4,
		panel->getContentSize().height*panel->getScaleY() / 2 -
		parameters->getContentSize().height*parameters->getScaleY()));
	menu_panel->addChild(parameters, 2, "parameters");
	auto reload = ui::Button::create("res/buttons/reload_wood.png"); 
	reload->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			game->setReloading(true);
		}
	});
	reload->setScale(1 / (reload->getBoundingBox().size.width / sizeButton));
	reload->setPosition(Vec2(-panel->getContentSize().width*panel->getScaleX() / 4,
		panel->getContentSize().height*panel->getScaleY() / 2 -
		reload->getContentSize().height*reload->getScaleY()));
	menu_panel->addChild(reload, 2, "reload");
	
	auto towers = Layer::create();
	towers->setContentSize(Size(visibleSize.width / 4 * 0.9, visibleSize.height * 3 / 5));
	towers->setPosition(Vec2(-visibleSize.width / 8 * 0.9, reload->getPosition().y - 
		reload->getContentSize().height * reload->getScaleY() * 2 / 3));
	towers->setAnchorPoint(Vec2(0.5, 0.5));
	int j(0);
	for (unsigned int i(0); i < Tower::getConfig().size(); ++i) {
		std::string sprite3_filename = Tower::getConfig()[Tower::getConfig().getMemberNames()[i]]["image"].asString();
		std::string sprite3_background_filename = "res/buttons/tower_button.png";
		std::string sprite3_disable_filename = "res/buttons/tower_inactive.png";
		std::string sprite3_clip_filename = "res/buttons/tower_button_clip.png";
		std::string sprite7_active_filename = "res/buttons/tower_active.png";
		std::string tower_name = Tower::getConfig().getMemberNames()[i];
		
		auto tower = Sprite::create(sprite3_background_filename);
		tower->addChild(Sprite::create(sprite3_filename), 5, "sprite");
		towers->addChild(tower, 1, Value(Tower::getConfig().getMemberNames()[i]).asString());
		tower->setScale(sizeTower / tower->getContentSize().width);
		tower->setPosition(Vec2(sizeTower / 2 + j % 2 * (sizeTower + towers->getContentSize().width / 20)+
			towers->getContentSize().width / 20,
			- sizeTower / 2 - (j / 2) * (sizeTower + towers->getContentSize().width / 20) - 
			towers->getContentSize().width / 20));
		tower->getChildByName("sprite")->setPosition(Vec2(tower->getContentSize().width/ 2, 
			tower->getContentSize().height / 2));
		Label* cost = Label::createWithTTF(Tower::getConfig()[Tower::getConfig().getMemberNames()[i]]["cost"][0].asString(),
			"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
		cost->setColor(Color3B::RED);
		cost->setPosition(Vec2(tower->getContentSize().width - cost->getContentSize().width * 2 / 3,
			cost->getContentSize().height / 2));
		tower->addChild(cost, 6, "cost");
		Sprite* sugar = Sprite::create("res/buttons/sugar.png");
		sugar->setScale(tower->getContentSize().width / 5 / sugar->getContentSize().width);
		sugar->setPosition(Vec2(cost->getPosition().x - cost->getContentSize().width / 2 - sugar->getContentSize().width * sugar->getScale() / 2,
			cost->getPosition().y));
		tower->addChild(sugar, 6, "sugar");

		towers_menu[Tower::getConfig().getMemberNames()[i]] = tower;
		++j;
	}
	menu_panel->addChild(towers, 2, "towers");

	// PANEL INFOS
	auto informations = ui::Layout::create();
	informations->setContentSize(Size(towers->getContentSize().width, panel->getContentSize().height / 4));
	informations->setPosition(Vec2(towers->getPosition().x + towers->getContentSize().width / 0.9 / 2,
		towers->getPosition().y - towers->getContentSize().height));
	informations->setVisible(false);
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();

	SpriteBatchNode* animation = SpriteBatchNode::create("res/turret/animations/archer.png");
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
	description_tower->setAnchorPoint(Vec2(0.5, 0.5));
	description_tower->setPosition(Vec2(0, range->getPosition().y - 
		range->getContentSize().height * range->getScaleY() - 
		description_tower->getDimensions().height / 2));
	informations->addChild(description_tower, 1, "description_tower");

	Sprite* sugar = Sprite::create("res/buttons/sugar.png");
	sugar->setScale(sizeButton / 2 / sugar->getContentSize().width);
	sugar->setPosition(Vec2(informations->getContentSize().width / 4,
		description_tower->getPosition().y - description_tower->getContentSize().height / 2 - 
		sugar->getContentSize().height * sugar->getScaleY() / 2));
	informations->addChild(sugar, 1, "sugar");
	Label* sugar_label = Label::createWithTTF("30", "fonts/LICABOLD.ttf", 35 * visibleSize.width / 1280);
	sugar_label->setColor(Color3B::GREEN);
	sugar_label->setPosition(Vec2(sugar->getPosition().x + sugar->getContentSize().width * sugar->getScale()  * 2 / 3,
		sugar->getPosition().y));
	sugar_label->setAnchorPoint(Vec2(0, 0.5f));
	sugar_label->enableOutline(Color4B::BLACK, 1);
	informations->addChild(sugar_label, 1, "sugar_label");

	menu_panel->addChild(informations, 2, "informations");
	addChild(menu_panel, 2, "menu_panel");
}

void InterfaceGame::displayTowerInfos(std::string item_name){
	if (item_name != "") {
		getChildByName("menu_panel")->getChildByName("informations")->setVisible(true);
		SpriteBatchNode* batch = (SpriteBatchNode*)getChildByName("menu_panel")->getChildByName("informations")->getChildByName("animation");
		batch->removeAllChildren();
		getChildByName("menu_panel")->getChildByName("informations")->removeChild(batch, true);

		cocos2d::Vector<SpriteFrame*> attackFrames;
		cocos2d::Vector<SpriteFrame*> steadyFrames;
		cocos2d::Vector<SpriteFrame*> staticFrames;
		std::string firstFrame;

		SpriteFrameCache* cache = SpriteFrameCache::getInstance();

		SpriteBatchNode* spriteBatchNode;

		Size visibleSize = Director::getInstance()->getVisibleSize();
		unsigned int required_quantity(0);
		((Label*)getChildByName("menu_panel")->getChildByName("informations")->getChildByName("attack_label"))->setString(
			Tower::getConfig()[item_name]["damages"][0].asString());
		((Label*)getChildByName("menu_panel")->getChildByName("informations")->getChildByName("speed_label"))->setString(
			Tower::getConfig()[item_name]["attack_speed"][0].asString());
		double range = round(Tower::getConfig()[item_name]["range"][0].asDouble() / Cell::getCellWidth() * 100) / 100;
		std::string s = Value(range).asString();
		s.resize(4);
		((Label*)getChildByName("menu_panel")->getChildByName("informations")->getChildByName("range_label"))->setString(
			s);

		((Label*)getChildByName("menu_panel")->getChildByName("informations")->getChildByName("description_tower"))->setString(
			Tower::getConfig()[item_name]["description"].asString());

		((Label*)getChildByName("menu_panel")->getChildByName("informations")->getChildByName("sugar_label"))->setString(
			Tower::getConfig()[item_name]["cost"][0].asString());
		required_quantity = Tower::getConfig()[item_name]["cost"][0].asDouble();
		if (game->getLevel()->getQuantity() < required_quantity) {
			((Label*)getChildByName("menu_panel")->getChildByName("informations")->getChildByName("sugar_label"))->setColor(Color3B::RED);
		}
		else {
			((Label*)getChildByName("menu_panel")->getChildByName("informations")->getChildByName("sugar_label"))->setColor(Color3B::GREEN);
		}

		steadyFrames = Tower::getAnimationFromName(item_name, Tower::IDLE);
		staticFrames.pushBack(steadyFrames.front());
		staticFrames.pushBack(steadyFrames.front());
		spriteBatchNode = SpriteBatchNode::create(Tower::getConfig()[item_name]["animation"].asString());
		spriteBatchNode->setContentSize(Size(
			getChildByName("menu_panel")->getChildByName("informations")->getContentSize().width / 2,
			getChildByName("menu_panel")->getChildByName("informations")->getContentSize().height / 3));
		spriteBatchNode->setAnchorPoint(Vec2(0.5f, 0.5f));
		Sprite* image = Sprite::createWithSpriteFrame(staticFrames.front());

		spriteBatchNode->addChild(image);
		spriteBatchNode->setPosition(Vec2(0, 0));

		Animation* animation2 = Animation::createWithSpriteFrames(steadyFrames, 0.075f);
		Animation* animation3 = Animation::createWithSpriteFrames(staticFrames, 0.5f);
		Vector<FiniteTimeAction*> sequence;
		sequence.pushBack(Animate::create(animation2));
		sequence.pushBack(Animate::create(animation2)->reverse());
		sequence.pushBack(Animate::create(animation3));
		sequence.pushBack(Animate::create(animation2));
		sequence.pushBack(Animate::create(animation2)->reverse());
		sequence.pushBack(Animate::create(animation3));
		sequence.pushBack(Animate::create(animation3));
		image->runAction(RepeatForever::create(Sequence::create(sequence)));

		getChildByName("menu_panel")->getChildByName("informations")->addChild(spriteBatchNode, 1, "animation");
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

void InterfaceGame::removeTower(){
	//selectedTurret->destroyCallback(nullptr);
	selectedTurret = nullptr;
	state = State::IDLE;
}

void InterfaceGame::destroyCallback(Ref* sender){
	game->getLevel()->increaseQuantity(selectedTurret->getCost() * 0.5 * (selectedTurret->getLevel() + 1));
}

void InterfaceGame::builtCallback(Ref* sender){
	selectedTurret->builtCallback(sender);
	selectedTurret->setFixed(true);
	game->getLevel()->decreaseQuantity(selectedTurret->getCost());
	//selectedTurret = nullptr;
	displayTowerInfos("");
}

void InterfaceGame::hideTowerInfo() {
	if (getChildByName("information_tower") != nullptr) {
		auto scale_to = ScaleTo::create(0.125f, 0.f);
		auto removeAndCreateLayout = CallFunc::create([&]() {
			removeChildByName("information_tower");
		});
		getChildByName("information_tower")->runAction(Sequence::create(scale_to, removeAndCreateLayout, nullptr));
	}
	if (game_state == TITLE) {
		getChildByName("start")->setVisible(true);
		getChildByName("title")->setVisible(true);
	}
}

void InterfaceGame::showTowerInfo() {
	if (getChildByName("information_tower") != nullptr) {
		auto scale_to = ScaleTo::create(0.125f, 0.f);
		auto removeAndCreateLayout = CallFunc::create([&]() {
			removeChildByName("information_tower");
			auto layout = selectedTurret->getInformationLayout(this);
			addChild(layout, 1, "information_tower");
			layout->setScale(0);
			auto scale_to = ScaleTo::create(0.125f, 1.f);
			layout->runAction(scale_to);
		});
		getChildByName("information_tower")->runAction(Sequence::create(scale_to, removeAndCreateLayout, nullptr));
	}
	else {
		auto layout = selectedTurret->getInformationLayout(this);
		addChild(layout, 1, "information_tower");
		layout->setScale(0);
		auto scale_to = ScaleTo::create(0.125f, 1.f);
		layout->runAction(scale_to);
	}
}

void InterfaceGame::updateButtonDisplay(){
	for (auto& tower : towers_menu) {
		if (game->getLevel()->getQuantity() < Tower::getConfig()[tower.first]["cost"][0].asDouble()) {
			((Label*)tower.second->getChildByName("cost"))->setColor(Color3B::RED);
			((Label*)tower.second->getChildByName("cost"))->enableOutline(Color4B::BLACK, 1.0);
		}
		else {
			((Label*)tower.second->getChildByName("cost"))->setColor(Color3B::GREEN);
			((Label*)tower.second->getChildByName("cost"))->enableOutline(Color4B::BLACK, 1.0);
		}
	}
}

void InterfaceGame::updateObjectDisplay(float dt){
	if(selectedTurret != nullptr){
		selectedTurret->updateDisplay(dt);
	}
}

std::pair<std::string, cocos2d::Sprite*> InterfaceGame::getTowerFromPoint(cocos2d::Vec2 location){
	cocos2d::Vec2 origin = getChildByName("menu_panel")->getBoundingBox().origin + 
		getChildByName("menu_panel")->getChildByName("towers")->getPosition();
	for (auto& item : towers_menu){
		Vec2 pointInSprite = location - item.second->getPosition() - origin;
		pointInSprite.x += item.second->getSpriteFrame()->getRect().size.width *
			item.second->getScale() / 2;
		pointInSprite.y += item.second->getSpriteFrame()->getRect().size.height *
			item.second->getScale() / 2;
		Rect itemRect = Rect(0, 0, item.second->getSpriteFrame()->getRect().size.width *
			item.second->getScale(),
			item.second->getSpriteFrame()->getRect().size.height * item.second->getScale());
		if (itemRect.containsPoint(pointInSprite)){
			return item;
		}
	}
	std::pair<std::string, Sprite*> item;
	item.first = "nullptr";
	return item;
}

InterfaceGame::GameState InterfaceGame::getGameState() const{
	return game_state;
}
void InterfaceGame::setGameState(GameState g_state) {
	game_state = g_state;
}

void InterfaceGame::setListening(bool listening){
	listener->setEnabled(listening);
}

void InterfaceGame::startRewarding(Vec2 pos){
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto reward_layout = ui::Layout::create();

	Node* node = Node::create();
	Sprite* sugar = Sprite::create("res/buttons/life.png");
	Sprite* shining = Sprite::create("res/buttons/shining.png");

	sugar->setScale(Cell::getCellWidth() / sugar->getContentSize().width);
	shining->setScale(2 * Cell::getCellWidth() / (shining->getContentSize().width));
	node->addChild(shining);
	node->addChild(sugar);

	Json::Value config = ((AppDelegate*)Application::getInstance())->getConfig()["levels"][id];
	auto* move1 = EaseOut::create(MoveBy::create(0.25f, Vec2(0, 75)), 2);
	auto* move2 = EaseBounceOut::create(MoveBy::create(1.0f, Vec2(0, -75)));

	node->setPosition(pos);
	reward_layout->addChild(node, 2);

	RotateBy* rotation = RotateBy::create(3.0f, 360);
	shining->runAction(RepeatForever::create(rotation));

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

	EaseIn* move = EaseIn::create(MoveTo::create(1.5f, Vec2(visibleSize.width / 2, visibleSize.height)), 2);
	EaseIn* scale = EaseIn::create(ScaleTo::create(1.5f, 0.01f), 2);

	node->runAction(Sequence::create(Spawn::createWithTwoActions(move1, move_h1),
		Spawn::createWithTwoActions(move2, move_h2),
		Spawn::createWithTwoActions(move, scale), nullptr));

	if (config["reward"].asString() != "none") {
		auto reward = ui::Button::create(config["reward"].asString());
		reward->addTouchEventListener([&](Ref *sender, cocos2d::ui::Widget::TouchEventType type) {
			if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
				setGameState(DONE);
				setListening(false);
			}
		});
		reward->setPosition(Vec2(0, 0));
		reward->setPosition(pos);
		reward->setScale(Cell::getCellWidth() / reward->getContentSize().width);
		reward_layout->addChild(reward, 1);
		MoveBy* move_h1 = nullptr;
		MoveBy* move_h2 = nullptr;
		if (60 + pos.x < visibleSize.width * 2 / 3) {
			move_h1 = MoveBy::create(0.25f, Vec2(15, 0));
			move_h2 = MoveBy::create(1.0f, Vec2(45, 0));
		}
		else {
			move_h1 = MoveBy::create(0.25f, Vec2(-25, 0));
			move_h2 = MoveBy::create(1.0f, Vec2(-75, 0));
		}
		auto* scale = EaseOut::create(ScaleTo::create(1.0f, 1), 2);
		// Since the level takes 3/4 of the screen. The center of the level is at 3/8.
		auto* movetocenter = EaseIn::create(MoveTo::create(0.5f, Vec2(visibleSize.width * 3 / 8,
			visibleSize.height / 2)), 2);

		reward->runAction(Sequence::create(Spawn::createWithTwoActions(move1->clone(), move_h1),
			Spawn::createWithTwoActions(move2->clone(), move_h2),
			Spawn::createWithTwoActions(scale, movetocenter), nullptr));

		// Add a Tap to continue to inform the user what to do.
		auto tapToContinue = Label::createWithSystemFont("Tap to continue", "Arial", 25.f);
		tapToContinue->setPosition(Vec2(visibleSize.width * 3 / 8, 50));

		tapToContinue->setColor(Color3B::WHITE);
		tapToContinue->setVisible(true);
		FadeIn* fadeIn = FadeIn::create(0.75f);
		FadeOut* fadeout = FadeOut::create(0.75f);

		tapToContinue->runAction(RepeatForever::create(Sequence::create(fadeIn, fadeout, NULL)));
		reward_layout->addChild(tapToContinue, 2,"taptocontinue");
		game_state = ENDING;
	}
	else {
		game_state = DONE;
	}
	addChild(reward_layout, 1, "reward_layout");
}

void InterfaceGame::initDialoguesFromLevel() {
	Json::Value config = ((AppDelegate*)Application::getInstance())->getConfig()["levels"][id];
	bool play_dialogue = ((AppDelegate*)Application::getInstance())->getConfig()["play_dialogue"].asInt() > 0;
	if (config["introDialogue"].size() != 0 && play_dialogue) {
		dialogues = Dialogue::createFromConfig(config["introDialogue"]);
		addChild(dialogues, 1, "dialogue");
		dialogues->launch();
		getChildByName("start")->setVisible(false);
		getChildByName("title")->setVisible(false);
	}
	else {
		game_state = TITLE;
		dialogues = nullptr;
	}
}