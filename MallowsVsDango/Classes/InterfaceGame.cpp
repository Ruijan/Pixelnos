#include "InterfaceGame.h"
#include "SceneManager.h"
#include "AppDelegate.h"
#include "Game.h"
#include "Towers/Grill.h"
#include "Towers/Scorpion.h"
#include <iostream>
#include <string>
#include <sstream>


USING_NS_CC;


template <typename T>
std::string to_string(T value)
{
  //create an output string stream
  std::ostringstream os ;

  //throw the value into the string stream
  os << value ;

  //convert the string stream into a string and return
  return os.str() ;
}

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

InterfaceGame* InterfaceGame::create(Game* ngame){

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
    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [&](cocos2d::Touch* touch, cocos2d::Event* event){
        cocos2d::Vec2 p = touch->getLocation();
        cocos2d::Rect rect = this->getBoundingBox();

        if(rect.containsPoint(p)){
			
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
			}
			auto item = getTowerFromPoint(touch->getStartLocation());
			if (item.first != "nullptr"){
				if (state == TURRET_SELECTED){
					selectedTurret = nullptr;
					selectedTurret->displayRange(false);
				}
				state = State::TOUCHED;
				menuPanel->getChildByName("toolsMenu")->setVisible(true);
				displayTowerInfos(item.first);
			}
			if (selectedTurret != nullptr && !game->isPaused() && state == TURRET_CHOSEN){
				Vec2 pos = touch->getLocation();
				moveSelectedTurret(pos);
			}
			return true; // to indicate that we have consumed it.
        }
        return false; // we did not consume this event, pass thru.
    };

    listener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event){
        if(state == State::TOUCHED || state == State::SCROLL){
			auto item = getTowerFromPoint(touch->getStartLocation());
			if (item.first == "nullptr"){
				//displayTowerInfos(item.first);
			}
			state = State::IDLE;
		}
		else if (state == TURRET_SELECTED){
			Tower* tower = game->getLevel()->touchingTower(touch->getLocation());
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
		}
    };

    listener->onTouchMoved = [&](cocos2d::Touch* touch, cocos2d::Event* event){
		if(state == State::TOUCHED){
			double dist = touch->getLocation().distanceSquared(touch->getStartLocation());
			if(dist > 100){

				auto item = getTowerFromPoint(touch->getStartLocation());
				if (item.first != "nullptr"){
					if (game->getLevel()->getQuantity() >= Tower::getConfig()[item.first]["cost"].asDouble()){
						state = State::TURRET_CHOSEN;
						Tower::TowerType tape = Tower::getTowerTypeFromString(item.first);
						menuTurretTouchCallback(Tower::getTowerTypeFromString(item.first));
						moveSelectedTurret(touch->getLocation());
						selectedTurret->displayRange(true);
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

void InterfaceGame::setGame(Game* ngame){
	game = ngame;
}

void InterfaceGame::menuGameCallback(Ref* sender){
	setMenuVisibleCallback(sender,0);

}

void InterfaceGame::menuMainCallback(Ref* sender){
	SceneManager::getInstance()->setScene(SceneManager::MENU);
}

void InterfaceGame::menuSaveCallback(Ref* sender){
	//SceneManager::sharedManager()->setScene(type);
}

void InterfaceGame::reloadCallback(Ref* sender){
	game->setReloading(true);
}
void InterfaceGame::accelerateOnOffCallback(Ref* sender){
	if (game->isAccelerated()){
		game->setNormalSpeed();
	}
	else{
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

void InterfaceGame::update(){
	updateButtonDisplay();

	for (auto& tower : turretsMenu){
		if (game->getLevel()->getQuantity() < Tower::getConfig()[tower.first]["cost"].asDouble()){
			tower.second.first->setVisible(false);
			tower.second.second->setVisible(true);
		}
		else{
			tower.second.first->setVisible(true);
			tower.second.second->setVisible(false);
		}
	}

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
		else if (turret == Tower::TowerType::GRILL){
			Grill* createdTurret = Grill::create();
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
}

void InterfaceGame::initParametersMenu(){
	Size visibleSize = Director::getInstance()->getVisibleSize();

	menuPause = Node::create();
	cocos2d::Menu* menu = Menu::create();
	menu->setPosition(Vec2());
	menuPause->setPosition(Vec2(Point(visibleSize.width / 2, visibleSize.height / 2)));

	Vector<MenuItem*> items;
	Label* label1 = Label::createWithSystemFont("Continue", "fonts/ChalkDust.ttf", 45.f);
	label1->enableShadow(Color4B::BLACK, Size(0, 0), 1);
	items.pushBack(MenuItemLabel::create(label1, CC_CALLBACK_1(InterfaceGame::menuGameCallback, this)));
	Label* label2 = Label::createWithSystemFont("Save", "fonts/ChalkDust.ttf", 45.f);
	label2->enableShadow(Color4B::BLACK, Size(0, 0), 1);
	items.pushBack(MenuItemLabel::create(label2, CC_CALLBACK_1(InterfaceGame::menuSaveCallback, this)));
	Label* label3 = Label::createWithSystemFont("Main Menu", "fonts/ChalkDust.ttf", 45.f);
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

	Label* label = Label::createWithSystemFont("", "fonts/ChalkDust.ttf", round(visibleSize.width /27.0));
	Label* label_life = Label::createWithSystemFont("", "fonts/ChalkDust.ttf", round(visibleSize.width / 27.0));
	infos["sugar"] = label;
	infos["life"] = label_life;
	label->setPosition(Point(sizeButton * 1.35, visibleSize.height - sizeButton * 0.25));
	label_life->setPosition(Point(2 * sizeButton * 1.35 + sizeButton * 0.75, visibleSize.height - sizeButton * 0.25));

	Sprite* sugar = Sprite::create("res/buttons/sugar.png");
	sugar->setPosition(Point(sizeButton * 0.25, visibleSize.height - sizeButton * 0.25));
	sugar->setAnchorPoint(Point(0.0f, 1.0f));
	sugar->setScale(sizeButton / sugar->getContentSize().width);
	addChild(sugar, 4);
	Sprite* life = Sprite::create("res/buttons/life.png");
	life->setPosition(Point(sizeButton * 2.5, visibleSize.height - sizeButton * 0.25));
	life->setAnchorPoint(Point(0.0f, 1.0f));
	life->setScale(sizeButton / life->getContentSize().width);
	addChild(life, 4);
	
	for (auto el : infos){
		el.second->setColor(Color3B::WHITE);
		el.second->setAnchorPoint(Point(0.0f, 1.0f));
		addChild(el.second);
	}
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
	Label* youloose = Label::createWithSystemFont("YOU LOOSE !", "fonts/ChalkDust.ttf", 45.f);
	youloose->enableShadow(Color4B::BLACK, Size(0, 0), 1);
	youloose->setPosition(0, 20);
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

	MenuItemSprite* next = MenuItemSprite::create(sprite, sprite, CC_CALLBACK_1(InterfaceGame::menuMainCallback, this));
	MenuItemSprite* mainMenu = MenuItemSprite::create(sprite2, sprite2, CC_CALLBACK_1(InterfaceGame::menuMainCallback, this));
	menuForWin->addChild(next);
	menuForWin->addChild(mainMenu);
	menuForWin->alignItemsHorizontallyWithPadding(20);
	menuForWin->setPosition(0, -75);

	Sprite* mask2 = Sprite::create("res/buttons/centralMenuPanel.png");
	menuWin->addChild(mask2);
	menuWin->addChild(menuForWin);
	Label* youwin = Label::createWithSystemFont("Level Cleared !", "fonts/ChalkDust.ttf", 28.f);
	youwin->enableShadow(Color4B::BLACK, Size(0, 0), 1);
	youwin->setPosition(0, 20);
	menuWin->addChild(youwin);
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
	//panel->setScaleX(visibleSize.width / panel->getTextureRect().size.width / 4);

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
	MenuItemToggle* itemToggle = MenuItemToggle::createWithCallback(CC_CALLBACK_1(InterfaceGame::accelerateOnOffCallback, this), acc, play, NULL);
	reload->setScale(1 / (reload->getBoundingBox().size.width / sizeButton));
	parameters->setScale(1 / (parameters->getBoundingBox().size.width / sizeButton));
	itemToggle->setScale(1 / (itemToggle->getBoundingBox().size.width / sizeButton));
	menuOpen = Menu::create();
	menuOpen->addChild(reload);
	menuOpen->addChild(parameters);
	menuOpen->addChild(itemToggle);
	menuOpen->alignItemsHorizontally();
	Size size = parameters->getBoundingBox().size;
	size.width += reload->getBoundingBox().size.width + itemToggle->getBoundingBox().size.width + 45;
	menuOpen->setPosition(0, visibleSize.height / 2 - size.height * 3 / 4.0);

	// DISPLAY TOWERS
	int j(0);
	for (int i(0); i < Tower::getConfig().size(); ++i){
		std::string sprite3_filename = Tower::getConfig()[Tower::getConfig().getMemberNames()[i]]["image_menu"].asString();
		std::string sprite3_disable_filename = Tower::getConfig()[Tower::getConfig().getMemberNames()[i]]["image_menu_disable"].asString();

		Sprite* sprite3 = Sprite::create(sprite3_filename);
		sprite3->setScale(1 / (sprite3->getContentSize().width / sizeTower));

		Sprite* sprite4 = Sprite::create(sprite3_disable_filename);
		sprite4->setScale(1 / (sprite4->getContentSize().width / sizeTower));
		sprite4->setVisible(false);

		size = sprite3->getContentSize();
		size.width *= sprite3->getScale();
		size.height *= sprite3->getScale();

		sprite3->setPosition(Vec2((j % 2 - (j + 1) % 2)*(size.width / 2 + visibleSize.width / 96.0), 
			visibleSize.height / 2 - size.height / 2 - visibleSize.width / 9.6 - (j / 2) * (size.height + visibleSize.width / 64)));
		sprite4->setPosition(sprite3->getPosition());

		menuPanel->addChild(sprite3, 2);
		menuPanel->addChild(sprite4, 2);
		turretsMenu[Tower::getConfig().getMemberNames()[i]] = std::make_pair(sprite3, sprite4);
		++j;
	}

	// PANEL INFOS
	informationPanel = Node::create();

	Sprite* background = Sprite::create("res/buttons/infopanel.png");
	background->setScale(panel->getTextureRect().size.width * panel->getScaleX() / background->getContentSize().width * 90.0 / 100.0);

	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("res/turret/animations/archer.plist", "res/turret/animations/archer.png");
	cache->addSpriteFramesWithFile("res/turret/animations/grill.plist", "res/turret/animations/grill.png");

	Sprite* image = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("archer_movement1.png"));
	image->setScale((sizeButton / image->getBoundingBox().size.width));
	SpriteBatchNode* spriteBatchNode = SpriteBatchNode::create("res/turret/animations/archer.png");
	spriteBatchNode->addChild(image);
	//spriteBatchNode->setScale();

	spriteBatchNode->setPosition(Vec2(-background->getContentSize().width*background->getScale() / 2 + 5,
		round(visibleSize.width / 32.0)));

	cocos2d::Vector<SpriteFrame*> animFrames;

	char str[100] = { 0 };
	for (int i = 1; i < 16; ++i)
	{
		sprintf(str, "archer_movement%d.png", i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.05f);
	image->runAction(RepeatForever::create(Animate::create(animation)));

	Label* infoTower = Label::createWithSystemFont("Damages: \nSpeed: \nRange:", "fonts/Love Is Complicated Again.ttf", round(visibleSize.width / 53.0));
	infoTower->setColor(Color3B::BLACK);
	infoTower->setAnchorPoint(Vec2(0, 0));
	infoTower->setPosition(-round(visibleSize.width / 32.0), 0);

	Label* descriptionTower = Label::createWithSystemFont("\n Cost:", "fonts/Love Is Complicated Again.ttf", round(visibleSize.width / 53.0));
	descriptionTower->setDimensions(round(visibleSize.width / 4.44), round(visibleSize.width / 12.8));
	descriptionTower->setColor(Color3B::BLACK);
	descriptionTower->setAnchorPoint(Vec2(0, 0));
	descriptionTower->setPosition(Vec2(-background->getContentSize().width*background->getScale() / 2 + visibleSize.width / 192.0,
		-image->getContentSize().width * image->getScale() - visibleSize.width / 96.0));

	informationPanel->addChild(background,0,"background");
	informationPanel->addChild(infoTower,1,"infoTower");
	informationPanel->addChild(descriptionTower, 1, "DescriptionTower");
	informationPanel->addChild(spriteBatchNode, 1, "Animation");
	informationPanel->setPosition(0, -visibleSize.height / 4.0 );
	informationPanel->setVisible(false);

	Menu* toolsMenu = Menu::create();
	Sprite* button = Sprite::create("res/buttons/delete_wood.png");
	button->setAnchorPoint(Point(0.5f, 0.5f));

	Sprite* button1 = Sprite::create("res/buttons/build_wood.png");
	button1->setAnchorPoint(Point(0.5f, 0.5f));

	Sprite* button2 = Sprite::create("res/buttons/upgrade_wood.png");
	button2->setAnchorPoint(Point(0.5f, 0.5f));

	MenuItemSprite* destroy = MenuItemSprite::create(button, button, CC_CALLBACK_1(InterfaceGame::destroyCallback, this));
	MenuItemSprite* built = MenuItemSprite::create(button1, button1, CC_CALLBACK_1(InterfaceGame::builtCallback, this));
	MenuItemSprite* upgrade = MenuItemSprite::create(button2, button2, CC_CALLBACK_1(InterfaceGame::upgradeCallback, this));
	destroy->setScale(1 / (destroy->getBoundingBox().size.width / sizeButton));
	built->setScale(1 / (built->getBoundingBox().size.width / sizeButton));
	upgrade->setScale(1 / (upgrade->getBoundingBox().size.width / sizeButton));
	toolsMenu->addChild(destroy,1,"destroy");
	toolsMenu->addChild(built,1,"built");
	toolsMenu->addChild(upgrade,1,"upgrade");
	toolsMenu->alignItemsHorizontally();
	toolsMenu->setPosition(0, -visibleSize.height / 2 + destroy->getContentSize().width / 2 * destroy->getScale() + visibleSize.width / 64.0);
	upgrade->setPosition(built->getPosition());
	toolsMenu->setVisible(false);


	menuPanel->addChild(panel);
	menuPanel->addChild(menuOpen);
	menuPanel->addChild(informationPanel);
	menuPanel->addChild(toolsMenu,1,"toolsMenu");
}

void InterfaceGame::displayTowerInfos(std::string itemName){
	informationPanel->setVisible(true);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	std::string str1 = Tower::getConfig()[itemName]["damages"].asString();
	((Label*)informationPanel->getChildByName("infoTower"))->setString("Damages: " +
		Tower::getConfig()[itemName]["damages"].asString() + " \nSpeed: " +
		Tower::getConfig()[itemName]["attack_speed"].asString() + "\nRange: " +
		Tower::getConfig()[itemName]["range"].asString());

	((Label*)informationPanel->getChildByName("DescriptionTower"))->setString(
		Tower::getConfig()[itemName]["description"].asString() + "\nCost: " +
		Tower::getConfig()[itemName]["cost"].asString());

	SpriteBatchNode* batch = (SpriteBatchNode*)informationPanel->getChildByName("Animation");
	batch->removeAllChildren();
	informationPanel->removeChild(batch, true);

	Sprite* image = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(itemName + "_movement1.png"));

	SpriteBatchNode* spriteBatchNode = SpriteBatchNode::create(Tower::getConfig()[itemName]["animation"].asString());
	spriteBatchNode->addChild(image);
	image->setScale((sizeButton / image->getBoundingBox().size.width) );
	//spriteBatchNode->setScale(0.5);

	Node* background = informationPanel->getChildByName("background");

	spriteBatchNode->setPosition(Vec2(-background->getContentSize().width*background->getScale() / 2 + visibleSize.width / 192.0,
		0) + image->getContentSize() * image->getScale() / 2);

	cocos2d::Vector<SpriteFrame*> animFrames;
	cocos2d::Vector<SpriteFrame*> steadyFrames;
	cocos2d::Vector<SpriteFrame*> staticFrames;

	char str[100] = { 0 };

	sprintf(str, "%s_steady_movement1.png", itemName.c_str());
	staticFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(str));
	staticFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(str));

	
	unsigned int animation_size = Tower::getConfig()[itemName]["animation_size"].asInt();
	unsigned int animation_steady_size = Tower::getConfig()[itemName]["animation_steady_size"].asInt();
	for (int i = 1; i <= animation_size; ++i)
	{
		sprintf(str, "%s_movement%d.png",itemName.c_str(), i);
		SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	for (int i = 1; i <= animation_steady_size; ++i)
	{
		sprintf(str, "%s_steady_movement%d.png", itemName.c_str(), i);
		SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		steadyFrames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.075f);
	Animation* animation2 = Animation::createWithSpriteFrames(steadyFrames, 0.125f);
	Animation* animation3 = Animation::createWithSpriteFrames(staticFrames, 1.0f);
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


void InterfaceGame::destroyCallback(Ref* sender){
	selectedTurret->destroyCallback(sender);
	selectedTurret = nullptr;
	state = State::IDLE;
}

void InterfaceGame::builtCallback(Ref* sender){
	selectedTurret->builtCallback(sender);
	selectedTurret->setFixed(true);
	game->getLevel()->decreaseQuantity(selectedTurret->getCost());
	informationPanel->setVisible(false);
	selectedTurret->displayRange(false);
	selectedTurret = nullptr;
	state = State::IDLE;
}

void InterfaceGame::upgradeCallback(Ref* sender){
	selectedTurret->upgradeCallback(sender);
}

void InterfaceGame::updateButtonDisplay(){
	if (selectedTurret != nullptr){
		menuPanel->getChildByName("toolsMenu")->setVisible(true);
		if (selectedTurret->isFixed()){
			menuPanel->getChildByName("toolsMenu")->getChildByName("built")->setVisible(false);
			menuPanel->getChildByName("toolsMenu")->getChildByName("upgrade")->setVisible(true);
		}
		else{
			menuPanel->getChildByName("toolsMenu")->getChildByName("built")->setVisible(true);
			menuPanel->getChildByName("toolsMenu")->getChildByName("upgrade")->setVisible(false);
		}
	}
	else{
		menuPanel->getChildByName("toolsMenu")->setVisible(false);
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
	std::pair<std::string, std::pair<cocos2d::Sprite*, cocos2d::Sprite*>> item;
	item.first = "nullptr";
	return item;
}
