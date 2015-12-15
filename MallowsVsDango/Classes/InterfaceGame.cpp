#include "InterfaceGame.h"
#include "SceneManager.h"
#include "AppDelegate.h"
#include "MyGame.h"
#include "Towers/Cutter.h"
#include "Towers/Scorpion.h"
#include "Lib/Functions.h"
#include <iostream>
#include <string>
#include <sstream>


USING_NS_CC;

InterfaceGame::InterfaceGame() : sizeButton(cocos2d::Director::getInstance()->getVisibleSize().width / 15),
	sizeTower(cocos2d::Director::getInstance()->getVisibleSize().width / 10)
{}

bool InterfaceGame::init(){

	if (!Layer::init()){ return false; }

	state = State::IDLE;

	initParametersMenu();
	initLooseMenu();
	initWinMenu();
	initRightPanel();
	initLabels();

	Size visibleSize = Director::getInstance()->getVisibleSize();

	Color4F grey(0.0f, 0.0f, 0.0f, 0.66f);
	blackMask = DrawNode::create();
	Vec2 rectangle[4];
	rectangle[0] = Vec2(-visibleSize.width / 2, -visibleSize.height / 2);
	rectangle[1] = Vec2(visibleSize.width / 2, -visibleSize.height / 2);
	rectangle[2] = Vec2(visibleSize.width / 2, visibleSize.height / 2);
	rectangle[3] = Vec2(-visibleSize.width / 2, visibleSize.height / 2);
	blackMask->drawPolygon(rectangle, 4, grey, 1, grey);
	blackMask->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	blackMask->setVisible(false);


	selectedTurret = nullptr;
	addChild(menuPause, 5);
	addChild(menuLoose,4);
	addChild(menuWin,4);
	addChild(menuPanel, 2);
	addChild(blackMask,3);

	addEvents();

	return true;
}

InterfaceGame* InterfaceGame::create(MyGame* ngame){

	InterfaceGame* interfaceGame = new InterfaceGame();
	interfaceGame->setGame(ngame);

	if (interfaceGame->init())
	{
		interfaceGame->autorelease();
		return interfaceGame;
	}
	else
	{
		delete interfaceGame;
		interfaceGame = NULL;
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
        cocos2d::Rect rectrightpanel = menuPanel->getChildByName("panel")->getBoundingBox();
        rectrightpanel.origin += menuPanel->getBoundingBox().origin;

        if(rect.containsPoint(p)){
			if(rectrightpanel.containsPoint(p)){
				resetTowerMenu();
				auto item = getTowerFromPoint(touch->getStartLocation());
				if (item.first != "nullptr"){
					if (state == TURRET_SELECTED){
						selectedTurret->displayRange(false);
						selectedTurret = nullptr;
					}
					else if(state == TURRET_CHOSEN){
						if(selectedTurret != nullptr){
							removeTower();
						}
					}
					state = State::TOUCHED;
					menuPanel->getChildByName("toolsMenu")->setVisible(true);
					displayTowerInfos(item.first);
					displayTowerMenu(item.second.second);
				}
				else{
					if(selectedTurret != nullptr && state == TURRET_CHOSEN){
						informationPanel->setVisible(false);
						removeTower();
						state = IDLE;
						resetTowerMenu();
					}
					else if (state == TURRET_SELECTED){
						menuPanel->getChildByName("toolsMenu")->setVisible(false);
						informationPanel->setVisible(false);
						selectedTurret->displayRange(false);
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
					menuPanel->getChildByName("toolsMenu")->setVisible(true);
					informationPanel->setVisible(true);
					selectedTurret = tower;
					selectedTurret->displayRange(true);
					state = TURRET_SELECTED;
				}
				else if (tower == nullptr && state == TURRET_SELECTED ){
					state = IDLE;
					menuPanel->getChildByName("toolsMenu")->setVisible(false);
					informationPanel->setVisible(false);
					selectedTurret->displayRange(false);
					selectedTurret = nullptr;
					resetTowerMenu();
				}
				else if(state == TOUCHED){ 
					auto item = getTowerFromPoint(touch->getStartLocation());
					if (item.first != "nullptr"){
						if (game->getLevel()->getQuantity() >= Tower::getConfig()[item.first]["cost"].asDouble()){
							state = State::TURRET_CHOSEN;
							Tower::TowerType tape = Tower::getTowerTypeFromString(item.first);
							menuTurretTouchCallback(Tower::getTowerTypeFromString(item.first));
							moveSelectedTurret(touch->getLocation());
							selectedTurret->displayRange(true);
							selectedTurret->setVisible(true);
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
		cocos2d::Rect rectrightpanel = menuPanel->getChildByName("panel")->getBoundingBox();
        rectrightpanel.origin += menuPanel->getBoundingBox().origin;
		if (state == TURRET_SELECTED){
			Tower* tower = game->getLevel()->touchingTower(p);
			if (tower != nullptr){
				std::string name = tower->getSpecConfig()["name"].asString();
				displayTowerInfos(tower->getSpecConfig()["name"].asString());
				selectedTurret->displayRange(true);
			}
			else{
				informationPanel->setVisible(false);
				selectedTurret->displayRange(false);
				selectedTurret = nullptr;
				state = IDLE;
			}
			resetTowerMenu();
		}
		else if(state == TURRET_CHOSEN){
			if( !rectrightpanel.containsPoint(p)){
				if(!isOnTower(p)){
					builtCallback(nullptr);
					state = State::TURRET_SELECTED;
				}
			}
			else{
				removeTower();
				state = State::IDLE;
				resetTowerMenu();
			}
		}
		else if(state == TOUCHED){
			if(selectedTurret != nullptr){
				destroyCallback(nullptr);
			}
			auto item = getTowerFromPoint(p);
			if (item.first != "nullptr"){
				if (game->getLevel()->getQuantity() >= Tower::getConfig()[item.first]["cost"].asDouble()){
					state = State::TURRET_CHOSEN;
					Tower::TowerType tape = Tower::getTowerTypeFromString(item.first);
					menuTurretTouchCallback(Tower::getTowerTypeFromString(item.first));
					selectedTurret->setVisible(false);
					selectedTurret->displayRange(true);
				}
				else{
					state = IDLE;
					resetTowerMenu();
				}
			}
		}
    };

    listener->onTouchMoved = [&](cocos2d::Touch* touch, cocos2d::Event* event){
		if(state == State::TOUCHED){
			double dist = touch->getLocation().distanceSquared(touch->getStartLocation());
			if(dist > 100){

				auto item = getTowerFromPoint(touch->getStartLocation());
				if (item.first != "nullptr"){
					if (game->getLevel()->getQuantity() >= Tower::getConfig()[item.first]["cost"].asDouble()){
						resetTowerMenu();
						state = State::TURRET_CHOSEN;
						Tower::TowerType tape = Tower::getTowerTypeFromString(item.first);
						menuTurretTouchCallback(Tower::getTowerTypeFromString(item.first));
						moveSelectedTurret(touch->getLocation());
						selectedTurret->displayRange(true);
						selectedTurret->setVisible(true);
					}
				}
			}
		}
		else if(selectedTurret != nullptr && !game->isPaused() &&
			state == State::TURRET_CHOSEN){
			Vec2 pos = touch->getLocation();
			moveSelectedTurret(pos);
			selectedTurret->displayRange(true);
		}
	};
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 30);
}

void InterfaceGame::setGame(MyGame* ngame){
	game = ngame;
}

void InterfaceGame::menuGameCallback(Ref* sender){
	setMenuVisibleCallback(sender,0);

}

void InterfaceGame::menuMainCallback(Ref* sender){
	setListening(false);
	SceneManager::getInstance()->setScene(SceneManager::LEVELS);
}

void InterfaceGame::reloadCallback(Ref* sender){
	game->setReloading(true);
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


void InterfaceGame::setMenuVisibleCallback(Ref* sender,int origin)
{
	if (!menuPause->isVisible()){
		Show* showAction = Show::create();
		showAction->setDuration(0.5f);
		menuPause->runAction(showAction);
		blackMask->setVisible(true);
		game->pause();

	}
	else{
		if(origin == 0){
			Hide* hideAction = Hide::create();
			hideAction->setDuration(0.5f);
			menuPause->runAction(hideAction);
			blackMask->setVisible(false);
			game->resume();
		}
	}
}

void InterfaceGame::update(float dt){
	updateButtonDisplay();
	for (auto& tower : turretsMenu){
		if (game->getLevel()->getQuantity() < Tower::getConfig()[tower.first]["cost"].asDouble()){
			tower.second.first->setVisible(true);
		}
		else{
			tower.second.first->setVisible(false);
		}
	}
	
	updateObjectDisplay(dt);

	std::string sugarText = "x" + to_string(game->getLevel()->getQuantity());
	std::string lifeText = "x" + to_string(game->getLevel()->getLife());

	infos["sugar"]->setString(sugarText);
	infos["sugar"]->enableOutline(Color4B::BLACK, 1.0);
	infos["life"]->setString(lifeText);
	infos["life"]->enableOutline(Color4B::BLACK, 1.0);
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

void InterfaceGame::touchEvent(cocos2d::Touch* touch, cocos2d::Vec2 point)
{
}

void InterfaceGame::moveSelectedTurret(Vec2 pos){
	if(selectedTurret != nullptr){
		Cell* nearestCell = game->getLevel()->getNearestCell(selectedTurret->getPosition());
		Cell* nearestCell2 = game->getLevel()->getNearestCell(pos/game->getLevel()->getScale());
		if(nearestCell2 != nullptr && nearestCell != nullptr){
			if(nearestCell2->isFree() && !nearestCell2->isPath()){
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
		if((nearestCell->isFree()|| nearestCell->getObject() == selectedTurret) && !nearestCell->isPath()){
			return false;
		}
	}
	return true;
}

void InterfaceGame::showLoose(){
	menuLoose->setVisible(true);
	blackMask->setVisible(true);
}

void InterfaceGame::showWin(){
	menuWin->setVisible(true);
	blackMask->setVisible(true);
}

void InterfaceGame::reset(){
	state = IDLE;
	selectedTurret = nullptr;
	menuLoose->setVisible(false);
	menuWin->setVisible(false);
	menuPause->setVisible(false);
	blackMask->setVisible(false);
	resetTowerMenu();
}

void InterfaceGame::initParametersMenu(){
	Size visibleSize = Director::getInstance()->getVisibleSize();

	menuPause = Node::create();
	cocos2d::Menu* menu = Menu::create();
	menu->setPosition(Vec2());
	menuPause->setPosition(Vec2(Point(visibleSize.width / 2, visibleSize.height / 2)));

	Vector<MenuItem*> items;
	Label* label1 = Label::createWithTTF("Resume", "fonts/ChalkDust.ttf", 45.f);
	label1->enableShadow(Color4B::BLACK, Size(0, 0), 1);
	items.pushBack(MenuItemLabel::create(label1, CC_CALLBACK_1(InterfaceGame::menuGameCallback, this)));
	Label* label3 = Label::createWithTTF("Main Menu", "fonts/ChalkDust.ttf", 45.f);
	label3->enableShadow(Color4B::BLACK, Size(0, 0), 1);
	items.pushBack(MenuItemLabel::create(label3, CC_CALLBACK_1(InterfaceGame::menuMainCallback, this)));

	Size size_menu = Size(0.0f, 0.0f);
	for (auto el : items){
		el->setColor(Color3B::BLACK);
		if (el->getContentSize().width > size_menu.width){
			size_menu.width = el->getContentSize().width;
		}
		size_menu.height += el->getContentSize().height;
		menu->addChild(el);
	}
	menu->alignItemsVertically();

	Sprite* mask = Sprite::create("res/buttons/centralMenuPanel.png");
	mask->setScaleX((size_menu.width * 1.5) / mask->getContentSize().width);
	mask->setScaleY((size_menu.height * 1.5) / mask->getContentSize().height);
	menuPause->addChild(mask, 4);
	menuPause->addChild(menu, 5);
	menuPause->setVisible(false);
}

void InterfaceGame::initLabels(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	Node* node_top = Node::create();
	node_top->setScale(0.75);
	node_top->setPosition(0,visibleSize.height);
	

	Label* label = Label::createWithTTF("", "fonts/ChalkDust.ttf", round(visibleSize.width /27.0));
	Label* label_life = Label::createWithTTF("", "fonts/ChalkDust.ttf", round(visibleSize.width / 27.0));
	infos["sugar"] = label;
	infos["life"] = label_life;
	label->setPosition(Point(sizeButton * 1.25,  - sizeButton * 0.125));
	label_life->setPosition(Point(2 * sizeButton * 1.35 + sizeButton * 0.75,  - sizeButton * 0.125));

	Sprite* sugar = Sprite::create("res/buttons/sugar.png");
	sugar->setPosition(Point(sizeButton * 0.125, - sizeButton * 0.125));
	sugar->setAnchorPoint(Point(0.0f, 1.0f));
	sugar->setScale(sizeButton / sugar->getContentSize().width);
	node_top->addChild(sugar, 4);
	Sprite* life = Sprite::create("res/buttons/life.png");
	life->setPosition(Point(sizeButton * 2.5, - sizeButton * 0.125));
	life->setAnchorPoint(Point(0.0f, 1.0f));
	life->setScale(sizeButton / life->getContentSize().width);
	node_top->addChild(life, 4);
	Sprite* panel_top = Sprite::create("res/buttons/panel_top.png");
	panel_top->setPosition(Point(0, 0));
	panel_top->setAnchorPoint(Point(0.0f, 1.0f));
	panel_top->setScale(sizeButton / sugar->getContentSize().width);
	node_top->addChild(panel_top, 3);
	Sprite* panel_top_shadow = Sprite::create("res/buttons/panel_top_shadow.png");
	panel_top_shadow->setPosition(Point(-15, 15));
	panel_top_shadow->setAnchorPoint(Point(0.0f, 1.0f));
	panel_top_shadow->setScale(sizeButton / sugar->getContentSize().width);
	node_top->addChild(panel_top_shadow, 2);
	
	for (auto el : infos){
		el.second->setColor(Color3B::WHITE);
		el.second->setAnchorPoint(Point(0.0f, 1.0f));
		node_top->addChild(el.second,4);
	}
	
	addChild(node_top);
}

void InterfaceGame::initLooseMenu(){
	Color3B color1 = Color3B(255, 200, 51);
	Color4F grey(102 / 255.0f, 178 / 255.0f, 255 / 255.0f, 0.66f);
	Size visibleSize = Director::getInstance()->getVisibleSize();

	menuLoose = Node::create();
	Menu* menuForLoose = Menu::create();
	menuForLoose->setPosition(Vec2());

	Sprite* sprite = Sprite::create("res/buttons/buttonRetry.png");
	sprite->setAnchorPoint(Point(0.5f, 0.5f));
	Sprite* sprite2 = Sprite::create("res/buttons/buttonMainMenu.png");
	sprite->setAnchorPoint(Point(0.5f, 0.5f));

	MenuItemSprite* next = MenuItemSprite::create(sprite, sprite, CC_CALLBACK_1(InterfaceGame::reloadCallback, this));
	MenuItemSprite* mainMenu = MenuItemSprite::create(sprite2, sprite2, CC_CALLBACK_1(InterfaceGame::menuMainCallback, this));
	menuForLoose->addChild(next);
	menuForLoose->addChild(mainMenu);
	menuForLoose->alignItemsHorizontallyWithPadding(20);
	menuForLoose->setPosition(0, -75);
	Sprite* mask = Sprite::create("res/buttons/centralMenuPanel.png");

	menuLoose->addChild(mask);
	menuLoose->addChild(menuForLoose);
	Label* youloose = Label::createWithTTF("YOU LOSE !", "fonts/ChalkDust.ttf", 45.f);
	youloose->enableShadow(Color4B::BLACK, Size(0, 0), 1);
	youloose->setPosition(0, 20);
	youloose->setColor(Color3B::BLACK);
	menuLoose->addChild(youloose);
	menuLoose->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	menuLoose->setScale(1.25);
	menuLoose->setVisible(false);
}

void InterfaceGame::initWinMenu(){
	Color3B color1 = Color3B(255, 200, 51);
	Size visibleSize = Director::getInstance()->getVisibleSize();

	menuWin = Node::create();
	Menu* menuForWin = Menu::create();
	menuForWin->setPosition(Vec2());

	Sprite* sprite = Sprite::create("res/buttons/buttonNextLevel.png");
	sprite->setAnchorPoint(Point(0.5f, 0.5f));
	Sprite* sprite2 = Sprite::create("res/buttons/buttonMainMenu.png");
	sprite->setAnchorPoint(Point(0.5f, 0.5f));

	MenuItemSprite* next = MenuItemSprite::create(sprite, sprite, CC_CALLBACK_1(InterfaceGame::menuNextCallback, this));
	MenuItemSprite* mainMenu = MenuItemSprite::create(sprite2, sprite2, CC_CALLBACK_1(InterfaceGame::menuMainCallback, this));
	menuForWin->addChild(next);
	menuForWin->addChild(mainMenu);
	menuForWin->alignItemsHorizontallyWithPadding(20);
	menuForWin->setPosition(0, -75);

	Sprite* mask2 = Sprite::create("res/buttons/centralMenuPanel.png");
	
	Label* youwin = Label::createWithTTF("Level Cleared !", "fonts/ChalkDust.ttf", 28.f);
	youwin->enableOutline(Color4B::BLACK, 1);
	youwin->setPosition(0, 50);
	Sprite* winMallow = Sprite::create("res/buttons/win.png");
	winMallow->setPosition(0, -10);
	winMallow->setScale(0.5);
	
	//adding children
	menuWin->addChild(mask2);
	menuWin->addChild(menuForWin);
	menuWin->addChild(youwin);
	menuWin->addChild(winMallow);
	menuWin->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	menuWin->setScale(1.5);
	menuWin->setVisible(false);
}

void InterfaceGame::initRightPanel(){
	Size visibleSize = Director::getInstance()->getVisibleSize();

	menuPanel = Node::create();
	menuPanel->setPosition(visibleSize.width * (3 / 4.0 + 1 / 8.0), visibleSize.height / 2);


	// PANEL FOR TOWERS, PARAMETERS AND INFOS
	Sprite* panel = Sprite::create("res/buttons/menupanel.png");
	panel->setAnchorPoint(Point(0.5f, 0.5f));
	panel->setScaleY(visibleSize.height / panel->getTextureRect().size.height);
	panel->setScaleX(visibleSize.width / panel->getTextureRect().size.width / 4);

	// DISPLAY BUTTONS FOR GAME
	Sprite* sprite = Sprite::create("res/buttons/parameters_wood.png");
	sprite->setAnchorPoint(Point(0.5f, 0.5f));

	Sprite* sprite2 = Sprite::create("res/buttons/reload_wood.png");
	sprite2->setAnchorPoint(Point(0.5f, 0.5f));

	Sprite* accelerate = Sprite::create("res/buttons/accelerate_wood.png");
	accelerate->setAnchorPoint(Point(0.5f, 0.5f));

	Sprite* normal = Sprite::create("res/buttons/play_wood.png");
	normal->setAnchorPoint(Point(0.5f, 0.5f));

	MenuItemSprite* parameters = MenuItemSprite::create(sprite, sprite, CC_CALLBACK_1(InterfaceGame::setMenuVisibleCallback, this, 1));
	MenuItemSprite* reload = MenuItemSprite::create(sprite2, sprite2, CC_CALLBACK_1(InterfaceGame::reloadCallback, this));
	MenuItemSprite* acc = MenuItemSprite::create(accelerate, accelerate, NULL);
	MenuItemSprite* play = MenuItemSprite::create(normal, normal, NULL);
	//MenuItemToggle* itemToggle = MenuItemToggle::createWithCallback(CC_CALLBACK_1(InterfaceGame::accelerateOnOffCallback, this), acc, play, NULL);
	reload->setScale(1 / (reload->getBoundingBox().size.width / sizeButton));
	parameters->setScale(1 / (parameters->getBoundingBox().size.width / sizeButton));
	//itemToggle->setScale(1 / (itemToggle->getBoundingBox().size.width / sizeButton));
	menuOpen = Menu::create();
	menuOpen->addChild(reload);
	menuOpen->addChild(parameters);
	//menuOpen->addChild(itemToggle);
	menuOpen->alignItemsHorizontally();
	
	if(Tower::getConfig().size() > 4){
		Sprite* down = Sprite::create("res/buttons/updown_bar.png");
		down->setAnchorPoint(Point(0.5f, 0.5f));
		Sprite* down_arrow = Sprite::create("res/buttons/updown_button.png");
		down_arrow->setAnchorPoint(Point(0.5f, 0.5f));
		Sprite* down_arrow_disable = Sprite::create("res/buttons/updown_button_disable.png");
		down_arrow_disable->setAnchorPoint(Point(0.5f, 0.5f));
		down_arrow_disable->setVisible(false);
		down->addChild(down_arrow,1,"down_arrow");
		down_arrow->setPosition(Vec2(down->getContentSize().width / 2, down->getContentSize().height / 2));
		down->addChild(down_arrow_disable,1,"down_arrow_disable");
		down_arrow_disable->setPosition(Vec2(down->getContentSize().width / 2, down->getContentSize().height / 2));
		MenuItemSprite* scroll_down = MenuItemSprite::create(down, down, CC_CALLBACK_1(InterfaceGame::setMenuVisibleCallback, this, 1));
		
		Sprite* up = Sprite::create("res/buttons/updown_bar.png");
		up->setAnchorPoint(Point(0.5f, 0.5f));
		Sprite* up_arrow = Sprite::create("res/buttons/updown_button.png");
		up_arrow->setAnchorPoint(Point(0.5f, 0.5f));
		Sprite* up_arrow_disable = Sprite::create("res/buttons/updown_button_disable.png");
		up_arrow_disable->setAnchorPoint(Point(0.5f, 0.5f));
		up_arrow_disable->setVisible(false);
		up->addChild(up_arrow,1,"up_arrow");
		up_arrow->setPosition(Vec2(up->getContentSize().width / 2, up->getContentSize().height / 2));
		up->addChild(up_arrow_disable,1,"up_arrow_disable");
		up_arrow_disable->setPosition(Vec2(up->getContentSize().width / 2, up->getContentSize().height / 2));
		up->setScaleY(-1);
		MenuItemSprite* scroll_up = MenuItemSprite::create(up, up, CC_CALLBACK_1(InterfaceGame::setMenuVisibleCallback, this, 1));
		
		scroll_down->setPosition(Vec2((panel->getContentSize().width - down->getContentSize().width) / 4.0,
			-visibleSize.height * 13 / 24));
		scroll_up->setPosition(Vec2((panel->getContentSize().width - up->getContentSize().width) / 4.0, 
			- sizeButton / 2 + up->getContentSize().height / 2));
		
		menuOpen->addChild(scroll_down,3);
		menuOpen->addChild(scroll_up,3);
	}
	
	Size size = parameters->getBoundingBox().size;
	size.width += reload->getBoundingBox().size.width + 45;
	//size.width += reload->getBoundingBox().size.width + itemToggle->getBoundingBox().size.width + 45;
	menuOpen->setPosition(0, visibleSize.height / 2 - size.height * 3 / 4.0);

	// DISPLAY TOWERS
	int j(0);
	for (int i(0); i < Tower::getConfig().size(); ++i){
		std::string sprite3_filename = Tower::getConfig()[Tower::getConfig().getMemberNames()[i]]["image"].asString();
		std::string sprite3_background_filename = "res/buttons/tower_button.png";
		std::string sprite3_disable_filename = "res/buttons/tower_inactive.png";
		std::string sprite3_clip_filename = "res/buttons/tower_button_clip.png";
		std::string sprite7_active_filename = "res/buttons/tower_active.png";

		Sprite* sprite3 = Sprite::create(sprite3_filename);
		sprite3->setScale(0.8 / (sprite3->getContentSize().width / sizeTower));

		Sprite* sprite4 = Sprite::create(sprite3_background_filename);
		sprite4->setScale(1 / (sprite4->getContentSize().width / sizeTower));
		sprite4->setVisible(true);
		
		Sprite* sprite5 = Sprite::create(sprite3_clip_filename);
		sprite5->setScale(1 / (sprite5->getContentSize().width / sizeTower));
		sprite5->setVisible(true);
		
		Sprite* sprite6 = Sprite::create(sprite3_disable_filename);
		sprite6->setScale(1 / (sprite6->getContentSize().width / sizeTower));
		sprite6->setVisible(false);
		
		Sprite* sprite7 = Sprite::create(sprite7_active_filename);
		sprite7->setScale(1 / (sprite6->getContentSize().width / sizeTower));
		sprite7->setVisible(false);

		size = sprite3->getContentSize();
		size.width *= sprite3->getScale();
		size.height *= sprite3->getScale();

		sprite3->setPosition(Vec2((j % 2 - (j + 1) % 2)*(sizeTower / 2 + visibleSize.width / 96.0), 
			visibleSize.height / 2 - sizeTower / 2 - visibleSize.width / 9.6 - (j / 2) * (sizeTower + visibleSize.width / 64)));
		sprite4->setPosition(sprite3->getPosition());
		sprite5->setPosition(sprite3->getPosition() + Vec2(sizeTower/30.0,sizeTower/2.0));
		sprite6->setPosition(sprite3->getPosition());
		sprite7->setPosition(sprite3->getPosition());

		menuPanel->addChild(sprite7, 2);
		menuPanel->addChild(sprite3, 3);
		menuPanel->addChild(sprite4, 2);
		menuPanel->addChild(sprite5, 5);
		menuPanel->addChild(sprite6, 4);
		
		turretsMenu[Tower::getConfig().getMemberNames()[i]] = std::pair<Sprite*,Sprite*>(sprite6,sprite7);
		++j;
	}

	// PANEL INFOS
	informationPanel = Node::create();

	Sprite* background = Sprite::create("res/buttons/infopanel.png");
	background->setScale(panel->getTextureRect().size.width * panel->getScaleX() / background->getContentSize().width * 90.0 / 100.0);

	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("res/turret/animations/archer.plist", "res/turret/animations/archer.png");
	cache->addSpriteFramesWithFile("res/turret/animations/cutter.plist", "res/turret/animations/cutter.png");
	cache->addSpriteFramesWithFile("res/turret/animations/cut.plist", "res/turret/animations/cut.png");
	cache->addSpriteFramesWithFile("res/turret/animations/splash.plist", "res/turret/animations/splash.png");
	cache->addSpriteFramesWithFile("res/dango/animations/dango1.plist", "res/dango/animations/dango1.png");
	cache->addSpriteFramesWithFile("res/dango/animations/dango2.plist", "res/dango/animations/dango2.png");
	cache->addSpriteFramesWithFile("res/dango/animations/dangobese1.plist", "res/dango/animations/dangobese1.png");

	Sprite* image = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("archer_steady_movement_000.png"));
	image->setScale(sizeButton / image->getBoundingBox().size.width);
	SpriteBatchNode* spriteBatchNode = SpriteBatchNode::create("res/turret/animations/archer.png");

	Label* infoTower = Label::createWithTTF("Damages: \nSpeed: \nRange:", "fonts/Love Is Complicated Again.ttf", round(visibleSize.width / 53.0));
	infoTower->setColor(Color3B::BLACK);
	infoTower->setAnchorPoint(Vec2(0, 0));
	infoTower->setPosition(-round(visibleSize.width / 32.0), -0.5*sizeButton);

	Sprite* sugar = Sprite::create("res/buttons/sugar.png");
	sugar->setScale(0.6*visibleSize.width/960);
	sugar->setPosition(Vec2(-background->getContentSize().width*background->getScale() / 2 + visibleSize.width / 40.0,
			-visibleSize.height * 2.8 / 15.0 ));
	Label* sugar_label = Label::createWithTTF("\n Cost:", "fonts/Love Is Complicated Again.ttf", round(visibleSize.width / 40.0));
	sugar_label->setColor(Color3B::GREEN);
	sugar_label->setPosition(Vec2(sugar->getPosition().x + sugar->getContentSize().width*sugar->getScale() / 2 + 5*visibleSize.width/960,
			-visibleSize.height * 2.8 / 15.0));
	sugar_label->setAnchorPoint(Vec2(0,0.5f));
	sugar_label->enableOutline(Color4B::BLACK, 1);

	Label* descriptionTower = Label::createWithTTF("\n Cost:", "fonts/Love Is Complicated Again.ttf", round(visibleSize.width / 53.0));
	descriptionTower->setDimensions(round(visibleSize.width / 4.44), round(visibleSize.width / 12.8));
	descriptionTower->setColor(Color3B::BLACK);
	descriptionTower->setAnchorPoint(Vec2(0.5, 0.5));
	//descriptionTower->setPosition(Vec2(-background->getContentSize().width*background->getScale() / 2 + visibleSize.width / 192.0,
	//	-image->getContentSize().width * image->getScale() / (visibleSize.width / 960) - visibleSize.width / 96.0 - sizeButton));
	descriptionTower->setPosition(Vec2( visibleSize.width / 192.0, -image->getContentSize().height*image->getScale() - 15*visibleSize.height/640));
	Label* nextLevel = Label::createWithTTF("", "fonts/Love Is Complicated Again.ttf", round(visibleSize.width / 55.0));
	nextLevel->setColor(Color3B::GREEN);
	nextLevel->setPosition(60 * visibleSize.width / 960, -0.5*sizeButton);
	nextLevel->enableOutline(Color4B::BLACK, 1);
	nextLevel->setAnchorPoint(Vec2(0,0));

	informationPanel->addChild(background,0,"background");
	informationPanel->addChild(sugar_label,1,"sugar_label");
	informationPanel->addChild(sugar,1,"sugar");
	informationPanel->addChild(infoTower,1,"infoTower");
	informationPanel->addChild(nextLevel,1,"nextLevel");
	informationPanel->addChild(descriptionTower, 1, "DescriptionTower");
	informationPanel->addChild(spriteBatchNode, 1, "Animation");
	informationPanel->setPosition(0, -visibleSize.height * 4.0 / 15.0 );
	informationPanel->setVisible(false);

	Menu* toolsMenu = Menu::create();
	Sprite* button = Sprite::create("res/buttons/sell.png");
	button->setAnchorPoint(Point(0.5f, 0.5f));
	button->setScale(visibleSize.width/960);


	Sprite* button2 = Sprite::create("res/buttons/upgrade_bg.png");
	button2->setAnchorPoint(Point(0.5f, 0.5f));
	button2->setScale(visibleSize.width/960);

	MenuItemSprite* destroy = MenuItemSprite::create(button, button, CC_CALLBACK_1(InterfaceGame::destroyCallback, this));
	MenuItemSprite* upgrade = MenuItemSprite::create(button2, button2, CC_CALLBACK_1(InterfaceGame::upgradeCallback, this));

	upgrade->setPosition(informationPanel->getPosition() + nextLevel->getPosition() + Vec2(sizeButton/6.0,0));
	upgrade->setAnchorPoint(Vec2(0.5,0));
	destroy->setPosition(destroy->getContentSize().width / 4.0, -visibleSize.height / 2
	 + destroy->getContentSize().height / 2 * destroy->getScale() + visibleSize.width / 64.0);
	Label* sell = Label::createWithTTF("Sell", "fonts/Love Is Complicated Again.ttf", round(visibleSize.width / 45.0));
	sell->setColor(Color3B::BLACK);
	sell->setAnchorPoint(Vec2(0,0));
	sell->setPosition(5,5);
	Sprite* sugar_destroy = Sprite::create("res/buttons/sugar.png");
	sugar_destroy->setScale(0.5 * visibleSize.width/960);
	sugar_destroy->setPosition((destroy->getContentSize().width - sugar_destroy->getContentSize().width/4  - 5)*visibleSize.width/960, (sugar_destroy->getContentSize().height/4 + 5)*visibleSize.width/960);
	destroy->addChild(sell,1,"quantity");
	destroy->addChild(sugar_destroy);
	toolsMenu->addChild(destroy,1,"destroy");
	toolsMenu->addChild(upgrade,1,"upgrade");
	//toolsMenu->alignItemsHorizontally();
	//toolsMenu->setPosition(destroy->getContentSize().width / 2.0, -visibleSize.height / 2 + destroy->getContentSize().width / 2 * destroy->getScale() + visibleSize.width / 64.0);
	toolsMenu->setPosition(0,0);
	toolsMenu->setVisible(false);


	menuPanel->addChild(panel,0,"panel");
	menuPanel->addChild(menuOpen);
	menuPanel->addChild(informationPanel,2,"informationPanel");
	menuPanel->addChild(toolsMenu,1,"toolsMenu");
}

void InterfaceGame::displayTowerInfos(std::string itemName){
	informationPanel->setVisible(true);
	SpriteBatchNode* batch = (SpriteBatchNode*)informationPanel->getChildByName("Animation");
	batch->removeAllChildren();
	informationPanel->removeChild(batch, true);
	
	cocos2d::Vector<SpriteFrame*> attackFrames;
	cocos2d::Vector<SpriteFrame*> steadyFrames;
	cocos2d::Vector<SpriteFrame*> staticFrames;
	std::string firstFrame;
	
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	
	SpriteBatchNode* spriteBatchNode;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	int required_quantity(0);
	if(selectedTurret == nullptr){
		((Label*)informationPanel->getChildByName("infoTower"))->setString("Damages: " +
			Tower::getConfig()[itemName]["damages"].asString() + " \nSpeed: " +
			Tower::getConfig()[itemName]["attack_speed"].asString() + "\nRange: " +
			Tower::getConfig()[itemName]["range"].asString());

		((Label*)informationPanel->getChildByName("DescriptionTower"))->setString(
			Tower::getConfig()[itemName]["description"].asString());
		
		((Label*)informationPanel->getChildByName("sugar_label"))->setString(
			Tower::getConfig()[itemName]["cost"].asString());
		required_quantity = Tower::getConfig()[itemName]["cost"].asDouble();
		attackFrames = Tower::getAnimationFromName(itemName, Tower::ATTACKING);
		steadyFrames = Tower::getAnimationFromName(itemName, Tower::IDLE);
		staticFrames.pushBack(steadyFrames.front());
		staticFrames.pushBack(steadyFrames.front());
		spriteBatchNode = SpriteBatchNode::create(Tower::getConfig()[itemName]["animation"].asString());
		firstFrame = Tower::getConfig()[itemName]["image"].asString();
		informationPanel->getChildByName("nextLevel")->setVisible(false);
	}
	else{
		int next_L = selectedTurret->getLevel() + 1;
		((Label*)informationPanel->getChildByName("infoTower"))->setString("LVL: " +
			to_string(selectedTurret->getLevel()) +
			"\nDMG: " + to_string(round(selectedTurret->getDamage()*10)/10) +" \nSPD: " +
			to_string(round(selectedTurret->getAttackSpeed()*10)/10) + "\nRNG: " +
			to_string(round(selectedTurret->getRange()*10)/10));
		((Label*)informationPanel->getChildByName("nextLevel"))->setString(
			" "+to_string(next_L) + "\n " + to_string(round((1+selectedTurret->getNextLevelDamage())*
			selectedTurret->getDamage()*10)/10) +"\n " + to_string(round((1+selectedTurret->getNextLevelSpeed())*
			selectedTurret->getAttackSpeed()*10)/10) + "\n " + to_string(round((1+selectedTurret->getNextLevelRange())*
			selectedTurret->getRange()*10)/10) );

		((Label*)informationPanel->getChildByName("DescriptionTower"))->setString(
			selectedTurret->getSpecConfig()["description"].asString());
		((Label*)informationPanel->getChildByName("sugar_label"))->setString(
			to_string(selectedTurret->getCost() * 0.5 * (selectedTurret->getLevel() + 1)));
		((Label*)menuPanel->getChildByName("toolsMenu")->getChildByName("destroy")->getChildByName("quantity"))->setString("Sell for " + 
			to_string(round(selectedTurret->getCost() * 0.5 * (selectedTurret->getLevel() + 1))));
		required_quantity = selectedTurret->getCost() * 0.5 * (selectedTurret->getLevel() + 1);
		attackFrames = selectedTurret->getAnimation(Tower::ATTACKING);
		steadyFrames = selectedTurret->getAnimation(Tower::IDLE);
		staticFrames.pushBack(steadyFrames.front());
		staticFrames.pushBack(steadyFrames.front());
		spriteBatchNode = SpriteBatchNode::create(selectedTurret->getSpecConfig()["animation"].asString());
		firstFrame = selectedTurret->getSpecConfig()["image"].asString();
		informationPanel->getChildByName("nextLevel")->setVisible(true);
	}
	if (game->getLevel()->getQuantity() < required_quantity){
		((Label*)informationPanel->getChildByName("sugar_label"))->setColor(Color3B::RED);
	}
	else{
		((Label*)informationPanel->getChildByName("sugar_label"))->setColor(Color3B::GREEN);
	}
	Sprite* image2 = Sprite::create(firstFrame);
	Sprite* image = Sprite::createWithSpriteFrame(staticFrames.front());
	
	spriteBatchNode->addChild(image);
	
	image->setScale((sizeButton / image2->getBoundingBox().size.width) );
	//spriteBatchNode->setScale(0.5);

	Node* background = informationPanel->getChildByName("background");
	
	spriteBatchNode->setPosition(Vec2(-visibleSize.width/16 - 10*visibleSize.width/960,0));

	Animation* animation = Animation::createWithSpriteFrames(attackFrames, 0.075f);
	Animation* animation2 = Animation::createWithSpriteFrames(steadyFrames, 0.075f);
	Animation* animation3 = Animation::createWithSpriteFrames(staticFrames, 0.5f);
	Vector<FiniteTimeAction*> sequence;
	sequence.pushBack(Animate::create(animation2));
	sequence.pushBack(Animate::create(animation2)->reverse());
	sequence.pushBack(Animate::create(animation3));
	sequence.pushBack(Animate::create(animation2));
	sequence.pushBack(Animate::create(animation2)->reverse());
	sequence.pushBack(Animate::create(animation3));
	sequence.pushBack(Animate::create(animation));
	sequence.pushBack(Animate::create(animation3));
	image->runAction(RepeatForever::create(Sequence::create(sequence)));

	informationPanel->addChild(spriteBatchNode,1,"Animation");
}

void InterfaceGame::removeTower(){
	selectedTurret->destroyCallback(nullptr);
	selectedTurret = nullptr;
	state = State::IDLE;
	resetTowerMenu();
}

void InterfaceGame::destroyCallback(Ref* sender){
	game->getLevel()->increaseQuantity(selectedTurret->getCost() * 0.5 * (selectedTurret->getLevel() + 1));
	removeTower();
}

void InterfaceGame::builtCallback(Ref* sender){
	selectedTurret->builtCallback(sender);
	selectedTurret->setFixed(true);
	game->getLevel()->decreaseQuantity(selectedTurret->getCost());
	selectedTurret->displayRange(true);
	resetTowerMenu();
	displayTowerInfos("");
}

void InterfaceGame::upgradeCallback(Ref* sender){
	
	if(game->getLevel()->getQuantity() >= selectedTurret->getCost() * 0.5 * (selectedTurret->getLevel() + 1) &&
		selectedTurret->getLevel() < Tower::MAXLEVEL){
		game->getLevel()->decreaseQuantity(selectedTurret->getCost() * 0.5 * (selectedTurret->getLevel() + 1));
		selectedTurret->upgradeCallback(sender);
		displayTowerInfos(""); 
	}
}

void InterfaceGame::menuNextCallback(Ref* sender){
	state = State::NEXT_LEVEL;
}

void InterfaceGame::updateButtonDisplay(){
	if (selectedTurret != nullptr){
		menuPanel->getChildByName("toolsMenu")->setVisible(true);
		if (selectedTurret->isFixed()){
			menuPanel->getChildByName("toolsMenu")->getChildByName("upgrade")->setVisible(true);
			menuPanel->getChildByName("toolsMenu")->getChildByName("destroy")->setVisible(true);
			if (game->getLevel()->getQuantity() < Value(((Label*)informationPanel->getChildByName("sugar_label"))->getString()).asInt()){
				((Label*)informationPanel->getChildByName("sugar_label"))->setColor(Color3B::RED);
			}
			else{
				((Label*)informationPanel->getChildByName("sugar_label"))->setColor(Color3B::GREEN);
			}
		}
		else{
			menuPanel->getChildByName("toolsMenu")->getChildByName("upgrade")->setVisible(false);
			menuPanel->getChildByName("toolsMenu")->getChildByName("destroy")->setVisible(false);
		}
	}
	else{
		menuPanel->getChildByName("toolsMenu")->setVisible(false);
	}
}

void InterfaceGame::updateObjectDisplay(float dt){
	if(selectedTurret != nullptr){
		selectedTurret->updateDisplay(dt);
	}
}

std::pair<std::string, std::pair<cocos2d::Sprite*, cocos2d::Sprite*>> InterfaceGame::getTowerFromPoint(cocos2d::Vec2 location){
	cocos2d::Vec2 origin = menuPanel->getBoundingBox().origin;
	for (auto& item : turretsMenu){
		Vec2 pointInSprite = location - item.second.first->getPosition() - origin;
		pointInSprite.x += item.second.first->getSpriteFrame()->getRect().size.width *
			item.second.first->getScale() / 2;
		pointInSprite.y += item.second.first->getSpriteFrame()->getRect().size.height *
			item.second.first->getScale() / 2;
		Rect itemRect = Rect(0, 0, item.second.first->getSpriteFrame()->getRect().size.width *
			item.second.first->getScale(),
			item.second.first->getSpriteFrame()->getRect().size.height * item.second.first->getScale());
		if (itemRect.containsPoint(pointInSprite)){
			return item;
		}
	}
	std::pair<std::string, std::pair<Sprite*,Sprite*>> item;
	item.first = "nullptr";
	return item;
}


InterfaceGame::State InterfaceGame::getState() const{
	return state;
}

void InterfaceGame::resetTowerMenu(){
	for (auto& tower : turretsMenu){
		if (tower.second.second->isVisible()){
			tower.second.second->stopAllActions();
			tower.second.second->setVisible(false);
		}
	}
}

void InterfaceGame::displayTowerMenu(Sprite* sprite){
	sprite->setVisible(true);
	sprite->setOpacity(0.5);
	auto action1 = FadeIn::create(0.5f);
    auto action1Back = action1->reverse();
    sprite->runAction( RepeatForever::create(Sequence::create( action1, action1Back, nullptr)));
}

void InterfaceGame::setListening(bool listening){
	listener->setEnabled(listening);
}
