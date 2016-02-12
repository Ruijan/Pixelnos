#include "MainMenu.h"
#include "SceneManager.h"
#include "AppDelegate.h"

USING_NS_CC;


bool MainMenu::init()
{
	if(!Scene::init()){ return false; }
	Size visibleSize = Director::getInstance()->getVisibleSize();
	/*
	Sprite* loadingBackground = Sprite::create("res/background/crissXcross.png");
	loadingBackground->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	loadingBackground->setScale(visibleSize.width / loadingBackground->getContentSize().width);
	addChild(loadingBackground);

	Sprite* bglogo = Sprite::create("res/background/logo.png");
	addChild(bglogo);
	bglogo->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	bglogo->setScale((visibleSize.width / 2) / bg1->getContentSize().width);

	bglogo->setOpacity(0.0);
	FadeIn* fadein = FadeIn::create(1.0);
	DelayTime* delay = DelayTime::create(1.0);
	FadeOut* fadeout = FadeOut::create(1.0);
	auto sequence = Sequence::create(fadein, delay, fadeout, nullptr);
	c_action = bglogo->runAction(sequence);
	c_action->retain();
	*/

	
	Label* start_label = Label::createWithTTF("START", "fonts/LICABOLD.ttf", 75.f * visibleSize.width / 960);
	MenuItemLabel* start = MenuItemLabel::create(start_label, CC_CALLBACK_1(MainMenu::menuContinueCallback, this));
	start_label->setColor(Color3B::YELLOW);
	start_label->enableOutline(Color4B::ORANGE,3);
	
	Point origin = Director::getInstance()->getVisibleOrigin();

	menu = Menu::createWithItem(start);
	menu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 4 ));

	menu->setVisible(false);
	FadeOut* hideAction = FadeOut::create(0.01f);
	menu->runAction(hideAction);

	bg1 = Sprite::create("res/background/menu_background1.png");
	bg1->setScaleX(visibleSize.width / bg1->getTextureRect().size.width);
	bg1->setScaleY(visibleSize.height / bg1->getTextureRect().size.height);
	bg1->setAnchorPoint(Point(0, 0));
	bg2 = Sprite::create("res/background/menu_background_battle.png");
	bg2->setScaleX(visibleSize.width / bg2->getTextureRect().size.width);
	bg2->setScaleY(visibleSize.height / bg2->getTextureRect().size.height);
	bg2->setAnchorPoint(Point(0, 0));
	bg2->setVisible(false);
	flash = Sprite::create("res/background/flash.png");
	flash->setScaleX(visibleSize.width / flash->getTextureRect().size.width);
	flash->setScaleY(visibleSize.height / flash->getTextureRect().size.height);
	flash->setAnchorPoint(Point(0, 0));
	flash->setVisible(true);
	flash->setOpacity(0);


	// TITLE OF THE GAME
	marshmallow = Sprite::create("res/background/mallowstextgreen.png");
	marshmallow->setPosition(visibleSize.width / 2, visibleSize.height * (2.0 / 3.0 + 1.0 / 5.0));
	marshmallow->setScale(0.0f);
	//marshmallow->setScale(visibleSize.width / marshmallow->getTextureRect().size.width * 0.8);
	dango = Sprite::create("res/background/dangotext2.png");
	dango->setPosition(visibleSize.width / 2, visibleSize.height * (1.0 / 2.0 + 1.0 / 7.0));
	dango->setScale(marshmallow->getTextureRect().size.height * marshmallow->getScale() / dango->getTextureRect().size.height);
	dango->setScale(0.0f);
	vs = Sprite::create("res/background/vs_bubble.png");
	//vs->setPosition(visibleSize.width / 2, visibleSize.height * (9.0 / 12.0));
	vs->setPosition(-50, visibleSize.height * (9.0 / 12.0));
	vs->setScale(0.0f);

	addChild(menu,1);
	addChild(bg1, 0);
	addChild(bg2, 0);
	addChild(dango, 2);
	addChild(marshmallow, 2);
	addChild(vs, 1);
	addChild(flash, 3);
	state = State::marshmallowStart;
	return true;
}

void MainMenu::update(float dt){
	/*if (c_action != nullptr && c_action->isDone()) {
		c_action->release();
		c_action = nullptr;
		SceneManager::getInstance()->setScene(SceneManager::LEVELS);
	}
	*/
	Size visibleSize = Director::getInstance()->getVisibleSize();
	double scale = 0.6;
	if (marshmallow->getScale() < visibleSize.width / marshmallow->getTextureRect().size.width * scale * 1.1 && state == State::marshmallowStart){
		marshmallow->setScale(marshmallow->getScale() + 2 * dt);
	}
	else if (marshmallow->getScale() > visibleSize.width / marshmallow->getTextureRect().size.width * scale * 1.1  && state == State::marshmallowStart){
		state = marshmallowBouncing1;
	}
	if (marshmallow->getScale() > visibleSize.width / marshmallow->getTextureRect().size.width * scale * 0.95  && state == State::marshmallowBouncing1){
		marshmallow->setScale(marshmallow->getScale() - 2 * dt);
	}
	else if (marshmallow->getScale() < visibleSize.width / marshmallow->getTextureRect().size.width * scale * 0.95 && state == State::marshmallowBouncing1){
		state = marshmallowBouncing2;
	}
	if (marshmallow->getScale() < visibleSize.width / marshmallow->getTextureRect().size.width * scale * 1.05 && state == State::marshmallowBouncing2){
		marshmallow->setScale(marshmallow->getScale() + 2.5 * dt);
	}
	else if (marshmallow->getScale() > visibleSize.width / marshmallow->getTextureRect().size.width * scale * 1.05 && state == State::marshmallowBouncing2){
		state = marshmallowBouncing3;
	}
	if (marshmallow->getScale() > visibleSize.width / marshmallow->getTextureRect().size.width * scale && state == State::marshmallowBouncing3){
		marshmallow->setScale(marshmallow->getScale() - 3.5 * dt);
	}
	else if (marshmallow->getScale() < visibleSize.width / marshmallow->getTextureRect().size.width * scale && state == State::marshmallowBouncing3){
		state = dangoStart;
	}
	if (dango->getScale() <marshmallow->getTextureRect().size.height * marshmallow->getScale() / dango->getTextureRect().size.height * 1.1 && state == State::dangoStart){
		dango->setScale(dango->getScale() + 2 * dt);
	}
	else if (dango->getScale() >marshmallow->getTextureRect().size.height * marshmallow->getScale() / dango->getTextureRect().size.height * 1.1 && state == State::dangoStart){
		state = dangoBouncing1;
	}
	if (dango->getScale() >marshmallow->getTextureRect().size.height * marshmallow->getScale() / dango->getTextureRect().size.height * 0.95 && state == State::dangoBouncing1){
		dango->setScale(dango->getScale() - 2 * dt);
	}
	else if (dango->getScale() <marshmallow->getTextureRect().size.height * marshmallow->getScale() / dango->getTextureRect().size.height * 0.95 && state == State::dangoBouncing1){
		state = dangoBouncing2;
	}
	if (dango->getScale() <marshmallow->getTextureRect().size.height * marshmallow->getScale() / dango->getTextureRect().size.height * 1.05 && state == State::dangoBouncing2){
		dango->setScale(dango->getScale() + 2.5 * dt);
	}
	else if (dango->getScale() >marshmallow->getTextureRect().size.height * marshmallow->getScale() / dango->getTextureRect().size.height * 1.05 && state == State::dangoBouncing2){
		state = dangoBouncing3;
	}
	if (dango->getScale() >marshmallow->getTextureRect().size.height * marshmallow->getScale() / dango->getTextureRect().size.height && state == State::dangoBouncing3){
		dango->setScale(dango->getScale() - 3.5 * dt);
	}
	else if (dango->getScale() < marshmallow->getTextureRect().size.height * marshmallow->getScale() / dango->getTextureRect().size.height && state == State::dangoBouncing3){
		state = vsStart;
		vs->setScale(marshmallow->getTextureRect().size.height * marshmallow->getScale() / vs->getTextureRect().size.height * 1.5);
	}

	double dist = vs->getPosition().distanceSquared(Vec2(visibleSize.width * 2 / 3, visibleSize.height * (9.0 / 12.0)));
	
	if (dist > 20 && state == State::vsStart){
		double speed = visibleSize.width * 2 / 3 + 50;
		double angularSpeed = 4 * 180;
		Vec2 direction = Vec2(visibleSize.width * 2 / 3, visibleSize.height * (9.0 / 12.0)) - vs->getPosition();
		direction.normalize();
		vs->setPosition(vs->getPosition() + direction * dt * speed);
		vs->setRotation(vs->getRotation() + dt*angularSpeed);
	}
	else if (dist < 20 && state == State::vsStart){
		state = flashStart;
		
	}
	if (state == State::flashStart && flash->getOpacity() < 245){
		double speed = 600;
		int opacity = floor(flash->getOpacity() - dt * speed);
		if (opacity > 246){
			opacity = 246;
		}
		flash->setOpacity(opacity);
	}
	else if (state == State::flashStart && flash->getOpacity() >= 245){
		state = flashEnd;
		bg2->setVisible(true);
	}
	if (state == State::flashEnd && flash->getOpacity() > 2){
		double speed = 400;
		int opacity = floor(flash->getOpacity() - dt * speed);
		if (opacity < 0){ 
			opacity = 0;
		}
		flash->setOpacity(opacity);
	}
	else if (state == State::flashEnd && flash->getOpacity() <= 2){
		state = menuShow;
	}
	if (state == State::menuShow){
		menu->setVisible(true);
		FadeIn* showAction = FadeIn::create(0.5f);
		menu->runAction(showAction);
		state = finished;
		
	}
}

void MainMenu::menuContinueCallback(Ref* sender){
	SceneManager::getInstance()->setScene(SceneManager::LEVELS);
}


void MainMenu::onEnter(){
	Scene::onEnter();
}

void MainMenu::onEnterTransitionDidFinish(){
	Scene::onEnterTransitionDidFinish();
	scheduleUpdate();
}
