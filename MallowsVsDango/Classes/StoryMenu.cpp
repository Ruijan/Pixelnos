#include "StoryMenu.h"
#include "Config/Config.h"
#include "AppDelegate.h"
#include "SceneManager.h"
#include "Lib/Functions.h"
#include "Lib/FadeMusic.h"
#include "Lib/AudioSlider.h"
#include "Config/AudioController.h"

USING_NS_CC;

bool StoryMenu::init(){
	if (!Scene::init()){ return false; }
	Size visibleSize = Director::getInstance()->getVisibleSize();

	//generating regions with missions/levels
	addChild(Sprite::create("res/background/levels.png"),1,"background"); 
	getChildByName("background")->setAnchorPoint(Vec2(0.5,0.5));
	getChildByName("background")->setPosition(visibleSize.width/2,visibleSize.height/2);
	double ratioX = visibleSize.width / 960;
	double ratioY = visibleSize.height / 640;
	getChildByName("background")->setScaleX(ratioX);
	getChildByName("background")->setScaleY(ratioY);
	
	//keep track of the progression and colorcode the levels (done, current, next)
	ui::Layout* menu = ui::Layout::create();
	menu->setPosition(Vec2(0, 0));
	addChild(menu, 2);
	Json::Value levels = ((AppDelegate*)Application::getInstance())->getConfig()["levels"];
	unsigned int cLevel = ((AppDelegate*)Application::getInstance())->getSave()["level"].asInt();
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
		//creating levels buttons
		ui::Button* bouton = ui::Button::create(filename); 
		bouton->setPosition(Vec2(levels[i]["x"].asInt() * ratioX, levels[i]["y"].asInt() * ratioY));
		bouton->setEnabled(enable);
		bouton->setScale(ratioX);
		bouton->addTouchEventListener([i](Ref* sender, ui::Widget::TouchEventType type) {
			if (type == ui::Widget::TouchEventType::ENDED) {
				SceneManager::getInstance()->getGame()->initLevel(i);
				SceneManager::getInstance()->setScene(SceneManager::GAME);
			}
		});
		//naming levels with colorcode
		Label* level_label = Label::createWithTTF(to_string(i+1), "fonts/LICABOLD.ttf",  ratioX * 35.f); 
		level_label->setPosition(Vec2(bouton->getContentSize().width/2.0  * ratioX, bouton->getContentSize().height * ratioY));
		level_label->setAnchorPoint(Vec2(0.5,0));
		level_label->setColor(color);
		level_label->enableOutline(Color4B::BLACK,1);
		bouton->addChild(level_label);
		menu->addChild(bouton);
	}
		
	//create settings button on the world map
	cocos2d::ui::Button* parametre = ui::Button::create("res/buttons/settings.png");
	cocos2d::ui::Layout* layout = ui::Layout::create();
	addChild(layout,1);
	layout->addChild(parametre);
	//put settings on the top right edge
	parametre->setPosition(Vec2(visibleSize.width - parametre->getContentSize().width / 2, 
		visibleSize.height - parametre->getContentSize().height / 2));
	//add buttons for volume control, credits...
	Sprite* option = Sprite::create("res/buttons/centralMenuPanel.png");
	option->setAnchorPoint(Vec2(0.5, 0.5));
	option->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	option->setScale(1.5);
	option->setVisible(false);
	addChild(option, 3);

	cocos2d::ui::Button* credits = ui::Button::create("res/buttons/plus.png");
	credits->setAnchorPoint(Vec2(0.5, 0.5));
	credits->setPosition(Vec2(option->getContentSize().width / 4, option->getContentSize().height / 2));
	option->addChild(credits, 4);

	cocos2d::ui::Button* json = ui::Button::create("res/buttons/music.png");
	json->setAnchorPoint(Vec2(0.5, 0.5));
	json->setPosition(Vec2(option->getContentSize().width / 2, option->getContentSize().height / 2));
	option->addChild(json, 4);

	cocos2d::ui::Button* autre = ui::Button::create("res/buttons/minus.png");
	autre->setAnchorPoint(Vec2(0.5, 0.5));
	autre->setPosition(Vec2(3*option->getContentSize().width / 4, option->getContentSize().height / 2));
	option->addChild(autre, 4);

	parametre->addTouchEventListener([option](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			option->setVisible(! option->isVisible());
		}
	});

/*	//checkbox son
	auto checkbox_music = cocos2d::ui::CheckBox::create("res/buttons/music.png", "res/buttons/music.png",
		"res/buttons/disable.png", "res/buttons/music.png", "res/buttons/music.png");
	auto checkbox_effects = cocos2d::ui::CheckBox::create("res/buttons/music.png", "res/buttons/music.png",
		"res/buttons/disable.png", "res/buttons/music.png", "res/buttons/music.png");
	auto checkbox_loop = cocos2d::ui::CheckBox::create("res/buttons/checkbox.png", "res/buttons/checkbox.png",
		"res/buttons/filled.png", "res/buttons/checkbox.png", "res/buttons/checkbox.png");

	Sprite* mask = Sprite::create("res/buttons/centralMenuPanel.png");
	mask->setScaleX((size_menu.width * 1.15) / mask->getContentSize().width);
	mask->setScaleY((size_menu.height * 1.75) / mask->getContentSize().height);

	AudioSlider* sliderMusicVolume = AudioSlider::create(AudioSlider::Horizontal);
	sliderMusicVolume->setValue(0, 1, ((AppDelegate*)Application::getInstance())->getAudioController()
		->getMaxMusicVolume());
	sliderMusicVolume->setPosition(Vec2(option->getContentSize().width*option->getScaleX() / 7, 45));
	((AppDelegate*)Application::getInstance())->addAudioSlider(sliderMusicVolume,
		AudioController::SOUNDTYPE::MUSIC);
	sliderMusicVolume->enable(false);

	AudioSlider* sliderEffectsVolume = AudioSlider::create(AudioSlider::Horizontal);
	sliderEffectsVolume->setValue(0, 1, ((AppDelegate*)Application::getInstance())->getAudioController()
		->getMaxEffectsVolume());
	sliderEffectsVolume->setPosition(Vec2(option->getContentSize().width*option->getScaleX() / 7, 0));
	((AppDelegate*)Application::getInstance())->addAudioSlider(sliderEffectsVolume,
		AudioController::SOUNDTYPE::EFFECT);
	sliderEffectsVolume->enable(false);

	checkbox_music->setPosition(Vec2(option->getContentSize().width*option->getScaleX() * 2 / 5, 45));
	checkbox_effects->setPosition(Vec2(option->getContentSize().width*option->getScaleX() * 2 / 5, 0));
	checkbox_loop->setPosition(Vec2(option->getContentSize().width*option->getScaleX() * 2 / 5, -50));

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

	menu->setPosition(Vec2(0, -option->getContentSize().height*option->getScaleY() / 2));

	layout->addChild(option, 4);
	option->addChild(sliderEffectsVolume, 5, "EffectsVolume");
	option->addChild(sliderMusicVolume, 5, "MusicVolume");
	option->addChild(checkbox_music, 6, "MusicEnable");
	option->addChild(checkbox_effects, 6, "EffectsEnable");
	option->addChild(checkbox_loop, 6, "LoopEnable");*/
/*	std::string credit_icon = "res/buttons/settings.png";
	Color3B credit_icon_color = Color3B::WHITE;
		
	credit_sprite->setAnchorPoint(Point(0.5f, 0.5f));

	MenuItemSprite* credit_item = MenuItemSprite::create(credit_sprite, credit_sprite, CC_CALLBACK_1(StoryMenu::showCredit, this));
	credit_item->setPosition(Vec2(visibleSize.width - credit_item->getContentSize().width / 2, 
		visibleSize.height - credit_item->getContentSize().height / 2));
	credit_item->setEnabled(true);
	credit_item->setScale(ratioX);

	menu->addChild(credit_item);
	menu->setPosition(0, 0);*/
	//Fin fonction appelle crédit



	/*Ajout boutons : music, credit*/

	
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

void StoryMenu::showCredit(Ref* sender){
	SceneManager::getInstance()->setScene(SceneManager::CREDIT);
}
