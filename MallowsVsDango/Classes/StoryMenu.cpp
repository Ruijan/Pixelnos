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

	// Background
	addChild(Sprite::create("res/background/levels.png"),1,"background");
	getChildByName("background")->setAnchorPoint(Vec2(0.5,0.5));
	getChildByName("background")->setPosition(visibleSize.width/2,visibleSize.height/2);
	double ratioX = visibleSize.width / 960;
	double ratioY = visibleSize.height / 640;
	getChildByName("background")->setScaleX(ratioX);
	getChildByName("background")->setScaleY(ratioY);
	
	// Interface with buttons and settings
	addChild(ui::Layout::create(), 2, "interface");
	ui::Button* level_editor = ui::Button::create("res/buttons/level_editor.png", "res/buttons/level_editor.png");
	level_editor->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			((SceneManager*)SceneManager::getInstance())->setScene(SceneManager::SceneType::EDITOR);
		}
	});
	level_editor->setPosition(Vec2(visibleSize.width - level_editor->getContentSize().width, level_editor->getContentSize().height/3));
	getChildByName("interface")->addChild(level_editor);

	auto settings = ui::Layout::create();
	getChildByName("interface")->addChild(settings, 2, "settings");
	ui::Button* panel = ui::Button::create("res/buttons/centralMenuPanel.png");

	panel->setZoomScale(0);
	settings->addChild(panel, 1, "panel");
	panel->setScaleX(0.45*visibleSize.width / panel->getContentSize().width);
	panel->setScaleY(0.45*visibleSize.width / panel->getContentSize().width);
	settings->setPosition(Vec2(visibleSize.width / 2, visibleSize.height +
		getChildByName("interface")->getChildByName("settings")->getChildByName("panel")->getContentSize().height *
		getChildByName("interface")->getChildByName("settings")->getChildByName("panel")->getScaleY()));

	Label* music = Label::createWithTTF("Music", "fonts/ChalkDust.ttf", 30.f);
	Label* effects = Label::createWithTTF("Effects", "fonts/ChalkDust.ttf", 30.f);
	Label* loop = Label::createWithTTF("Loop Music", "fonts/ChalkDust.ttf", 25.f);
	music->setColor(Color3B::BLACK);
	effects->setColor(Color3B::BLACK);
	loop->setColor(Color3B::BLACK);
	music->setPosition(-panel->getContentSize().width*panel->getScaleX() * 2 / 5 +
		music->getContentSize().width / 2, 45);
	effects->setPosition(-panel->getContentSize().width*panel->getScaleX() * 2 / 5 +
		effects->getContentSize().width / 2, 0);
	loop->setPosition(-panel->getContentSize().width*panel->getScaleX() * 2 / 5 + 
		loop->getContentSize().width / 2, -50);
	settings->addChild(music,2);
	settings->addChild(effects,2);
	settings->addChild(loop,2);

	auto close = ui::Button::create("res/buttons/close.png");
	close->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if(type == ui::Widget::TouchEventType::ENDED){
			Size visibleSize = Director::getInstance()->getVisibleSize();
			//getChildByName("interface")->getChildByName("settings")->setVisible(false);
			auto* showAction = EaseBackIn::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height + 
				getChildByName("interface")->getChildByName("settings")->getChildByName("panel")->getContentSize().width *
				getChildByName("interface")->getChildByName("settings")->getChildByName("panel")->getScaleY() / 2)));
			getChildByName("interface")->getChildByName("settings")->runAction(showAction);
			((AudioSlider*)getChildByName("interface")->getChildByName("settings")->
				getChildByName("EffectsVolume"))->enable(false);
			((AudioSlider*)getChildByName("interface")->getChildByName("settings")->
				getChildByName("MusicVolume"))->enable(false);
			((ui::CheckBox*)getChildByName("interface")->getChildByName("settings")->
				getChildByName("MusicEnable"))->setEnabled(false);
			((ui::CheckBox*)getChildByName("interface")->getChildByName("settings")->
				getChildByName("EffectsEnable"))->setEnabled(false);
			((ui::CheckBox*)getChildByName("interface")->getChildByName("settings")->
				getChildByName("LoopEnable"))->setEnabled(false);
		}
	});
	close->setScale(panel->getContentSize().width*panel->getScaleX() / 11 / close->getContentSize().width);
	close->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() / 2 - close->getContentSize().width*close->getScale() / 3,
		panel->getContentSize().height*panel->getScaleX() / 2 - close->getContentSize().height*close->getScale() / 3));
	settings->addChild(close, 5, "close");

	auto checkbox_music = cocos2d::ui::CheckBox::create("res/buttons/music.png", "res/buttons/music.png",
		"res/buttons/disable.png", "res/buttons/music.png", "res/buttons/music.png");
	auto checkbox_effects = cocos2d::ui::CheckBox::create("res/buttons/music.png", "res/buttons/music.png",
		"res/buttons/disable.png", "res/buttons/music.png", "res/buttons/music.png");
	auto checkbox_loop = cocos2d::ui::CheckBox::create("res/buttons/checkbox.png", "res/buttons/checkbox.png",
		"res/buttons/filled.png", "res/buttons/checkbox.png", "res/buttons/checkbox.png");
	checkbox_music->setEnabled(false);
	checkbox_effects->setEnabled(false);
	checkbox_loop->setEnabled(false);

	AudioSlider* sliderMusicVolume = AudioSlider::create(AudioSlider::Horizontal);
	sliderMusicVolume->setValue(0, 1, ((AppDelegate*)Application::getInstance())->getAudioController()
		->getMaxMusicVolume());
	sliderMusicVolume->setPosition(panel->getContentSize().width*panel->getScaleX() / 7, 45);
	((AppDelegate*)Application::getInstance())->addAudioSlider(sliderMusicVolume,
		AudioController::SOUNDTYPE::MUSIC);
	sliderMusicVolume->enable(false);

	AudioSlider* sliderEffectsVolume = AudioSlider::create(AudioSlider::Horizontal);
	sliderEffectsVolume->setValue(0, 1, ((AppDelegate*)Application::getInstance())->getAudioController()
		->getMaxEffectsVolume());
	sliderEffectsVolume->setPosition(panel->getContentSize().width*panel->getScaleX() / 7, 0);
	((AppDelegate*)Application::getInstance())->addAudioSlider(sliderEffectsVolume,
		AudioController::SOUNDTYPE::EFFECT);
	sliderEffectsVolume->enable(false);

	checkbox_music->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() * 2 / 5, 45));
	checkbox_effects->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() * 2 / 5, 0));
	checkbox_loop->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() * 2 / 5, -50));

	((AppDelegate*)Application::getInstance())->getAudioController()->addButton(checkbox_music,
		AudioController::SOUNDTYPE::MUSIC);
	((AppDelegate*)Application::getInstance())->getAudioController()->addButton(checkbox_effects,
		AudioController::SOUNDTYPE::EFFECT);
	((AppDelegate*)Application::getInstance())->getAudioController()->addButtonLoop(checkbox_loop);


	checkbox_music->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED){
			((AppDelegate*)Application::getInstance())->getAudioController()->enableMusic(
				!((cocos2d::ui::CheckBox*)sender)->isSelected());
		}
	});
	checkbox_effects->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			((AppDelegate*)Application::getInstance())->getAudioController()->enableEffects(
				!((cocos2d::ui::CheckBox*)sender)->isSelected());
		}
	});
	checkbox_loop->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			((AppDelegate*)Application::getInstance())->getAudioController()->enableLoop(
				((cocos2d::ui::CheckBox*)sender)->isSelected());
		}
	});

	settings->addChild(sliderEffectsVolume, 5, "EffectsVolume");
	settings->addChild(sliderMusicVolume, 5, "MusicVolume");
	settings->addChild(checkbox_music, 6, "MusicEnable");
	settings->addChild(checkbox_effects, 6, "EffectsEnable");
	settings->addChild(checkbox_loop, 6, "LoopEnable");
	
	auto credits = ui::Button::create("res/buttons/buttonCredits.png");
	credits->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			((SceneManager*)SceneManager::getInstance())->setScene(SceneManager::SceneType::CREDIT);
		}
	});
	credits->setPosition(Vec2(0,
		-panel->getContentSize().height*panel->getScaleY() / 2 + 
		credits->getContentSize().height*credits->getScaleY() / 4));
	settings->addChild(credits, 5, "credits");
	//settings->setVisible(false);

	ui::Button* show_setting = ui::Button::create("res/buttons/settings.png");
	show_setting->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Size visibleSize = Director::getInstance()->getVisibleSize();
			bool enable = !getChildByName("interface")->getChildByName("settings")->isVisible();
			auto* showAction = EaseBackOut::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height / 2)));
			getChildByName("interface")->getChildByName("settings")->runAction(showAction);


			((AudioSlider*)getChildByName("interface")->getChildByName("settings")->
				getChildByName("EffectsVolume"))->enable(enable);
			((AudioSlider*)getChildByName("interface")->getChildByName("settings")->
				getChildByName("MusicVolume"))->enable(enable);
			((ui::CheckBox*)getChildByName("interface")->getChildByName("settings")->
				getChildByName("MusicEnable"))->setEnabled(enable);
			((ui::CheckBox*)getChildByName("interface")->getChildByName("settings")->
				getChildByName("EffectsEnable"))->setEnabled(enable);
			((ui::CheckBox*)getChildByName("interface")->getChildByName("settings")->
				getChildByName("LoopEnable"))->setEnabled(enable);
		}
	});
	show_setting->setPosition(Vec2(visibleSize.width - show_setting->getContentSize().width / 2,
		visibleSize.height - show_setting->getContentSize().height / 2));
	getChildByName("interface")->addChild(show_setting);

	// selection of levels
	addChild(ui::Layout::create(), 1, "levels");
	
	getChildByName("levels")->setPosition(0, 0);
	initLevels();
	
	return true;
}

void StoryMenu::selectLevelCallBack(Ref* sender, ui::Widget::TouchEventType type, int level_id){
	if (type == ui::Widget::TouchEventType::ENDED) {
		SceneManager::getInstance()->getGame()->initLevel(level_id);
		SceneManager::getInstance()->setScene(SceneManager::GAME);
	}
}

void StoryMenu::onEnterTransitionDidFinish(){
	Scene::onEnterTransitionDidFinish();
	getChildByName("levels")->removeAllChildren();
	initLevels();
}

void StoryMenu::showCredit(Ref* sender){
	SceneManager::getInstance()->setScene(SceneManager::CREDIT);
}

void StoryMenu::initLevels() {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	double ratioX = visibleSize.width / 960;
	double ratioY = visibleSize.height / 640;
	Json::Value levels = ((AppDelegate*)Application::getInstance())->getConfig()["levels"];
	int cLevel = ((AppDelegate*)Application::getInstance())->getSave()["level"].asInt();
	for (unsigned int i(0); i < levels.size(); ++i) {
		std::string filename;
		Color3B color;
		bool enable(true);
		if ((int)i > cLevel) {
			filename = "res/buttons/level_button_disable.png";
			color = Color3B(50, 50, 50);
			enable = false;
		}
		else if ((int)i == cLevel) {
			filename = "res/buttons/level_button.png";
			color = Color3B(220, 168, 17);
		}
		else if ((int)i < cLevel) {
			filename = "res/buttons/level_button_done.png";
			color = Color3B::WHITE;
		}

		ui::Button* level = ui::Button::create(filename);
		level->addTouchEventListener(CC_CALLBACK_2(StoryMenu::selectLevelCallBack, this, i));
		level->setPosition(Vec2(levels[i]["x"].asInt() * ratioX, levels[i]["y"].asInt() * ratioY));
		level->setEnabled(enable);
		level->setScale(ratioX);

		Label* level_label = Label::createWithTTF(to_string(i + 1), "fonts/LICABOLD.ttf", ratioX * 35.f);
		level_label->setPosition(Vec2(level->getContentSize().width / 2.0  * ratioX, level->getContentSize().height * ratioY));
		level_label->setAnchorPoint(Vec2(0.5, 0));
		level_label->setColor(color);
		level_label->enableOutline(Color4B::BLACK, 1);
		level->addChild(level_label);
		getChildByName("levels")->addChild(level);
	}
}