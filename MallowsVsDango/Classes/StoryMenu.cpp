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
	level_editor->setScale(visibleSize.width / 15 / level_editor->getContentSize().width);
	level_editor->setPosition(Vec2(visibleSize.width, 0));
	level_editor->setAnchorPoint(Vec2(1.f, 0.3f));
	getChildByName("interface")->addChild(level_editor);

	auto settings = ui::Layout::create();
	getChildByName("interface")->addChild(settings, 2, "settings");
	ui::Button* panel = ui::Button::create("res/buttons/centralMenuPanel.png");
	panel->setZoomScale(0);
	settings->addChild(panel, 1, "panel");
	panel->setScale9Enabled(true);
	panel->setScaleX(0.45*visibleSize.width / panel->getContentSize().width);
	panel->setScaleY(0.8*visibleSize.width / panel->getContentSize().width);

	settings->setPosition(Vec2(visibleSize.width / 2, visibleSize.height +
		getChildByName("interface")->getChildByName("settings")->getChildByName("panel")->getContentSize().height *
		getChildByName("interface")->getChildByName("settings")->getChildByName("panel")->getScaleY()));

	auto close = ui::Button::create("res/buttons/close.png");
	close->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if(type == ui::Widget::TouchEventType::ENDED){
			Size visibleSize = Director::getInstance()->getVisibleSize();
			auto* showAction = EaseBackIn::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height + 
				getChildByName("interface")->getChildByName("settings")->getChildByName("panel")->getContentSize().width *
				getChildByName("interface")->getChildByName("settings")->getChildByName("panel")->getScaleY() / 2)));
			getChildByName("interface")->getChildByName("settings")->runAction(showAction);
		}
	});
	close->setScale(panel->getContentSize().width*panel->getScaleX() / 11 / close->getContentSize().width);
	close->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() / 2 - close->getContentSize().width*close->getScale() / 3,
		panel->getContentSize().height*panel->getScaleY() / 2 - close->getContentSize().height*close->getScale() / 3));
	settings->addChild(close, 5, "close");

	Label* title = Label::createWithTTF("Settings", "fonts/LICABOLD.ttf", 45.0f * visibleSize.width / 1280);
	title->setColor(Color3B::BLACK);
	title->setPosition(0, panel->getContentSize().height*panel->getScaleY() / 2 - title->getContentSize().height);
	settings->addChild(title, 2, "title");


	Label* music = Label::createWithTTF("Music", "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	Label* effects = Label::createWithTTF("Effects", "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	Label* loop = Label::createWithTTF("Loop Music", "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	music->setColor(Color3B::BLACK);
	effects->setColor(Color3B::BLACK);
	loop->setColor(Color3B::BLACK);
	music->setPosition(-panel->getContentSize().width * panel->getScaleX() * 2 / 5 +
		music->getContentSize().width / 2, title->getPosition().y - title->getContentSize().height -
		music->getContentSize().height * 3 /4);
	effects->setPosition(-panel->getContentSize().width*panel->getScaleX() * 2 / 5 +
		effects->getContentSize().width / 2, music->getPosition().y - music->getContentSize().height -
		effects->getContentSize().height * 3 / 4);
	loop->setPosition(-panel->getContentSize().width*panel->getScaleX() * 2 / 5 +
		loop->getContentSize().width / 2, effects->getPosition().y - effects->getContentSize().height -
		loop->getContentSize().height * 3 / 4);
	settings->addChild(music, 2);
	settings->addChild(effects, 2);
	settings->addChild(loop, 2);

	auto checkbox_music = cocos2d::ui::CheckBox::create("res/buttons/music.png", "res/buttons/music.png",
		"res/buttons/disable.png", "res/buttons/music.png", "res/buttons/music.png");
	auto checkbox_effects = cocos2d::ui::CheckBox::create("res/buttons/music.png", "res/buttons/music.png",
		"res/buttons/disable.png", "res/buttons/music.png", "res/buttons/music.png");
	auto checkbox_loop = cocos2d::ui::CheckBox::create("res/buttons/checkbox.png", "res/buttons/checkbox.png",
		"res/buttons/filled.png", "res/buttons/checkbox.png", "res/buttons/checkbox.png");

	AudioSlider* sliderMusicVolume = AudioSlider::create(AudioSlider::Horizontal);
	sliderMusicVolume->setValue(0, 1, ((AppDelegate*)Application::getInstance())->getAudioController()
		->getMaxMusicVolume());
	sliderMusicVolume->setPosition(panel->getContentSize().width*panel->getScaleX() / 7,
		music->getPosition().y);
	((AppDelegate*)Application::getInstance())->addAudioSlider(sliderMusicVolume,
		AudioController::SOUNDTYPE::MUSIC);
	sliderMusicVolume->setScale(visibleSize.width / 1280);

	AudioSlider* sliderEffectsVolume = AudioSlider::create(AudioSlider::Horizontal);
	sliderEffectsVolume->setValue(0, 1, ((AppDelegate*)Application::getInstance())->getAudioController()
		->getMaxEffectsVolume());
	sliderEffectsVolume->setPosition(panel->getContentSize().width*panel->getScaleX() / 7, effects->getPosition().y);
	((AppDelegate*)Application::getInstance())->addAudioSlider(sliderEffectsVolume,
		AudioController::SOUNDTYPE::EFFECT);
	sliderEffectsVolume->setScale(visibleSize.width / 1280);

	checkbox_music->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() * 2 / 5,
		music->getPosition().y));
	checkbox_music->setScale(visibleSize.width / 1280);
	checkbox_effects->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() * 2 / 5, 
		effects->getPosition().y));
	checkbox_effects->setScale(visibleSize.width / 1280);
	checkbox_loop->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() / 7,
		loop->getPosition().y));
	checkbox_loop->setScale(visibleSize.width / 1280);

	((AppDelegate*)Application::getInstance())->getAudioController()->addButton(checkbox_music,
		AudioController::SOUNDTYPE::MUSIC);
	((AppDelegate*)Application::getInstance())->getAudioController()->addButton(checkbox_effects,
		AudioController::SOUNDTYPE::EFFECT);
	((AppDelegate*)Application::getInstance())->getAudioController()->addButtonLoop(checkbox_loop);


	checkbox_music->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		switch (type) {
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
	settings->addChild(sliderEffectsVolume, 5, "EffectsVolume");
	settings->addChild(sliderMusicVolume, 5, "MusicVolume");
	settings->addChild(checkbox_music, 6, "MusicEnable");
	settings->addChild(checkbox_effects, 6, "EffectsEnable");
	settings->addChild(checkbox_loop, 6, "LoopEnable");

	// GLOBAL SETTINGS
	Label* always_show_grid = Label::createWithTTF("Always", "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	Label* moving_show_grid = Label::createWithTTF("On Move", "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	Label* never_show_grid = Label::createWithTTF("Never", "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);

	Label* show_grid = Label::createWithTTF("Show Grid", "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	Label* auto_limit = Label::createWithTTF("Auto Limit", "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	Label* skip_dialogues = Label::createWithTTF("Play Dialogues", "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	always_show_grid->setColor(Color3B::BLACK);
	moving_show_grid->setColor(Color3B::BLACK);
	never_show_grid->setColor(Color3B::BLACK);
	show_grid->setColor(Color3B::BLACK);
	auto_limit->setColor(Color3B::BLACK);
	skip_dialogues->setColor(Color3B::BLACK);

	double offset = panel->getContentSize().width * panel->getScaleX() / 40;

	never_show_grid->setPosition(0, loop->getPosition().y - loop->getContentSize().height / 2 -
		always_show_grid->getContentSize().height * 3 / 4);
	moving_show_grid->setPosition(never_show_grid->getPosition().x + never_show_grid->getContentSize().width / 2 +
		moving_show_grid->getContentSize().width / 2 + offset, loop->getPosition().y - loop->getContentSize().height / 2 -
		always_show_grid->getContentSize().height * 3 / 4);
	checkbox_loop->setPosition(Vec2(moving_show_grid->getPosition().x, checkbox_loop->getPosition().y));
	always_show_grid->setPosition(moving_show_grid->getPosition().x + moving_show_grid->getContentSize().width / 2 +
		always_show_grid->getContentSize().width / 2 + offset, loop->getPosition().y - loop->getContentSize().height / 2 -
		always_show_grid->getContentSize().height * 3 / 4);
	show_grid->setPosition(-panel->getContentSize().width * panel->getScaleX() * 2 / 5 +
		show_grid->getContentSize().width / 2, always_show_grid->getPosition().y - always_show_grid->getContentSize().height / 2 -
		show_grid->getContentSize().height * 3 / 4);
	auto_limit->setPosition(-panel->getContentSize().width*panel->getScaleX() * 2 / 5 +
		auto_limit->getContentSize().width / 2, show_grid->getPosition().y - show_grid->getContentSize().height / 2 -
		auto_limit->getContentSize().height * 3 / 4);
	skip_dialogues->setPosition(-panel->getContentSize().width*panel->getScaleX() * 2 / 5 +
		skip_dialogues->getContentSize().width / 2, auto_limit->getPosition().y - auto_limit->getContentSize().height / 2 -
		skip_dialogues->getContentSize().height * 3 / 4);

	settings->addChild(always_show_grid, 2);
	settings->addChild(moving_show_grid, 2);
	settings->addChild(never_show_grid, 2);
	settings->addChild(show_grid, 2);
	settings->addChild(auto_limit, 2);
	settings->addChild(skip_dialogues, 2);

	auto checkbox_grid = cocos2d::ui::CheckBox::create("res/buttons/checkbox.png", "res/buttons/checkbox.png",
		"res/buttons/filled.png", "res/buttons/checkbox.png", "res/buttons/checkbox.png");
	((AppDelegate*)Application::getInstance())->getConfigClass()->addGridButton(checkbox_grid);
	checkbox_grid->setPosition(Vec2(always_show_grid->getPosition().x,
		show_grid->getPosition().y));
	checkbox_grid->setSelected(((AppDelegate*)Application::getInstance())->getConfigClass()->isAlwaysGridEnabled());
	checkbox_grid->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			((AppDelegate*)Application::getInstance())->getConfigClass()->enableMovingGrid(false);
			((AppDelegate*)Application::getInstance())->getConfigClass()->enableAlwaysGrid(true);
			((AppDelegate*)Application::getInstance())->getConfigClass()->enableNeverGrid(false);
		}
	});

	auto checkbox_never_grid = cocos2d::ui::CheckBox::create("res/buttons/checkbox.png", "res/buttons/checkbox.png",
		"res/buttons/filled.png", "res/buttons/checkbox.png", "res/buttons/checkbox.png");
	((AppDelegate*)Application::getInstance())->getConfigClass()->addNeverGridButton(checkbox_never_grid);
	checkbox_never_grid->setPosition(Vec2(never_show_grid->getPosition().x,
		show_grid->getPosition().y));
	checkbox_never_grid->setSelected(((AppDelegate*)Application::getInstance())->getConfigClass()->isNeverGridEnabled());
	checkbox_never_grid->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			((AppDelegate*)Application::getInstance())->getConfigClass()->enableMovingGrid(false);
			((AppDelegate*)Application::getInstance())->getConfigClass()->enableAlwaysGrid(false);
			((AppDelegate*)Application::getInstance())->getConfigClass()->enableNeverGrid(true);
		}
	});

	auto checkbox_moving_grid = cocos2d::ui::CheckBox::create("res/buttons/checkbox.png", "res/buttons/checkbox.png",
		"res/buttons/filled.png", "res/buttons/checkbox.png", "res/buttons/checkbox.png");
	((AppDelegate*)Application::getInstance())->getConfigClass()->addMovingGridButton(checkbox_moving_grid);
	checkbox_moving_grid->setPosition(Vec2(moving_show_grid->getPosition().x,
		show_grid->getPosition().y));
	checkbox_moving_grid->setSelected(((AppDelegate*)Application::getInstance())->getConfigClass()->isMovingGridEnabled());
	checkbox_moving_grid->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			((AppDelegate*)Application::getInstance())->getConfigClass()->enableMovingGrid(true);
			((AppDelegate*)Application::getInstance())->getConfigClass()->enableAlwaysGrid(false);
			((AppDelegate*)Application::getInstance())->getConfigClass()->enableNeverGrid(false);
		}
	});

	auto checkbox_limit = cocos2d::ui::CheckBox::create("res/buttons/checkbox.png", "res/buttons/checkbox.png",
		"res/buttons/filled.png", "res/buttons/checkbox.png", "res/buttons/checkbox.png");
	((AppDelegate*)Application::getInstance())->getConfigClass()->addLimitButton(checkbox_limit);
	checkbox_limit->setPosition(Vec2(moving_show_grid->getPosition().x,
		auto_limit->getPosition().y));
	checkbox_limit->setSelected(((AppDelegate*)Application::getInstance())->getConfigClass()->isLimitEnabled());
	checkbox_limit->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if(type == ui::Widget::TouchEventType::ENDED){
			((AppDelegate*)Application::getInstance())->getConfigClass()->enableAutoLimit(
				((cocos2d::ui::CheckBox*)sender)->isSelected());
		}
	});

	auto checkbox_dialogues = cocos2d::ui::CheckBox::create("res/buttons/checkbox.png", "res/buttons/checkbox.png",
		"res/buttons/filled.png", "res/buttons/checkbox.png", "res/buttons/checkbox.png");
	((AppDelegate*)Application::getInstance())->getConfigClass()->addDialogueButton(checkbox_dialogues);
	checkbox_dialogues->setPosition(Vec2(moving_show_grid->getPosition().x,
		skip_dialogues->getPosition().y));
	bool selected = ((AppDelegate*)Application::getInstance())->getConfigClass()->isDialoguesEnabled();
	checkbox_dialogues->setSelected(selected);
	checkbox_dialogues->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			((AppDelegate*)Application::getInstance())->getConfigClass()->enableDialogues(
				((cocos2d::ui::CheckBox*)sender)->isSelected());
		}
	});

	settings->addChild(checkbox_grid, 6, "GridEnable");
	settings->addChild(checkbox_never_grid, 6, "NeverGridEnable");
	settings->addChild(checkbox_moving_grid, 6, "MovingGridEnable");
	settings->addChild(checkbox_limit, 6, "LimitEnable");
	settings->addChild(checkbox_dialogues, 6, "DialogueEnable");
	
	auto credits = ui::Button::create("res/buttons/buttonCredits.png");
	credits->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			((SceneManager*)SceneManager::getInstance())->setScene(SceneManager::SceneType::CREDIT);
		}
	});
	credits->setScale(panel->getContentSize().width*panel->getScaleX() / 2 / credits->getContentSize().width);
	credits->setPosition(Vec2(0,
		-panel->getContentSize().height*panel->getScaleY() / 2 +
		credits->getContentSize().height*credits->getScaleY() / 5));
	settings->addChild(credits, 5, "credits");
	//settings->setVisible(false);

	ui::Button* show_setting = ui::Button::create("res/buttons/settings.png");
	show_setting->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Size visibleSize = Director::getInstance()->getVisibleSize();
			auto* showAction = EaseBackOut::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height / 2)));
			getChildByName("interface")->getChildByName("settings")->runAction(showAction);
		}
	});
	show_setting->setScale(visibleSize.width / 15 / show_setting->getContentSize().width);
	show_setting->setPosition(Vec2(visibleSize.width - show_setting->getContentSize().width * show_setting->getScale() / 2,
		visibleSize.height - show_setting->getContentSize().height * show_setting->getScale() / 2));
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
			//color = Color3B::RED;
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

		Label* level_label = Label::createWithTTF(to_string(i + 1), "fonts/LICABOLD.ttf", 35.f);
		level_label->setPosition(Vec2(level->getContentSize().width / 2.0, level->getContentSize().height));
		level_label->setAnchorPoint(Vec2(0.5, 0));
		level_label->setColor(color);
		level_label->enableOutline(Color4B::BLACK, 1);
		level->addChild(level_label);
		getChildByName("levels")->addChild(level);
	}
}