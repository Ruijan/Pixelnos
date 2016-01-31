#include "StoryMenu.h"
#include "Config/Config.h"
#include "AppDelegate.h"
#include "SceneManager.h"
#include "Lib/Functions.h"
#include "Lib/FadeMusic.h"

USING_NS_CC;

bool StoryMenu::init(){
	if (!Scene::init()){ return false; }
	Size visibleSize = Director::getInstance()->getVisibleSize();
	addChild(Sprite::create("res/background/levels.png"),1,"background");
	getChildByName("background")->setAnchorPoint(Vec2(0.5,0.5));
	getChildByName("background")->setPosition(visibleSize.width/2,visibleSize.height/2);
	double ratioX = visibleSize.width / 960;
	double ratioY = visibleSize.height / 640;
	getChildByName("background")->setScaleX(ratioX);
	getChildByName("background")->setScaleY(ratioY);
	
	menu = Menu::create();
	Json::Value levels = ((AppDelegate*)Application::getInstance())->getConfig()["levels"];
	int cLevel = ((AppDelegate*)Application::getInstance())->getSave()["level"].asInt();
	for(unsigned int i(0); i < levels.size(); ++i){
		std::string filename;
		Color3B color;
		bool enable(true);
		if(i > cLevel){
			filename = "res/buttons/level_button_disable.png";
			color = Color3B(50,50,50);
			enable = false;
		}
		else if(i == cLevel){
			filename = "res/buttons/level_button.png";
			color = Color3B(220,168,17);
		}
		else if(i < cLevel){
			filename = "res/buttons/level_button_done.png";
			color = Color3B::WHITE;
		}
		Sprite* sprite = Sprite::create(filename);
		sprite->setAnchorPoint(Point(0.5f, 0.5f));
		
		MenuItemSprite* level = MenuItemSprite::create(sprite, sprite, CC_CALLBACK_1(StoryMenu::selectLevelCallBack, this, i));
		level->setPosition(Vec2(levels[i]["x"].asInt() * ratioX, levels[i]["y"].asInt() * ratioY));
		level->setEnabled(enable);
		level->setScale(ratioX);

		Label* level_label = Label::createWithTTF(to_string(i+1), "fonts/LICABOLD.ttf",  ratioX * 35.f);
		level_label->setPosition(Vec2(sprite->getContentSize().width/2.0  * ratioX, sprite->getContentSize().height * ratioY));
		level_label->setAnchorPoint(Vec2(0.5,0));
		level_label->setColor(color);
		level_label->enableOutline(Color4B::BLACK,1);
		level->addChild(level_label);
		menu->addChild(level);
	}
	menu->setPosition(0, 0);
	addChild(menu,2);
	
	return true;
}

void StoryMenu::selectLevelCallBack(Ref* sender, int level_id){
	SceneManager::getInstance()->getGame()->initLevel(level_id);
	SceneManager::getInstance()->setScene(SceneManager::GAME);
}

void StoryMenu::onEnterTransitionDidFinish(){
	Scene::onEnterTransitionDidFinish();
	removeAllChildren();
	init();
}
