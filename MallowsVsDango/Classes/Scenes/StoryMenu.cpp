#include "StoryMenu.h"
#include "../Config/Config.h"
#include "../AppDelegate.h"
#include "MyGame.h"
#include "../SceneManager.h"
#include "../Lib/Functions.h"
#include "../Lib/FadeMusic.h"
#include "../Lib/AudioSlider.h"
#include "../Config/AudioController.h"


USING_NS_CC;

bool StoryMenu::init(){
	if (!Scene::init()){ return false; }
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Json::Value root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file

	tutorial_running = false;

	std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();

	addChild(ui::Layout::create(), 3, "black_mask");
	ui::Button* mask = ui::Button::create("res/buttons/mask.png");
	mask->setScaleX(visibleSize.width / mask->getContentSize().width);
	mask->setScaleY(visibleSize.height / mask->getContentSize().height);
	mask->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	getChildByName("black_mask")->addChild(mask);
	getChildByName("black_mask")->setVisible(false);

	// Background
	addChild(Sprite::create("res/background/space.png"),0,"background");
	getChildByName("background")->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	getChildByName("background")->setScaleX(visibleSize.width / getChildByName("background")->getContentSize().width);
	getChildByName("background")->setScaleY(visibleSize.height / getChildByName("background")->getContentSize().height);

	double ratioX = visibleSize.width / 1280;
	double ratioY = visibleSize.height / 720;

	Sprite* sugar_sprite1 = Sprite::create("res/buttons/holy_sugar.png");
	sugar_sprite1->setScale(visibleSize.width / sugar_sprite1->getContentSize().width / 12);
	sugar_sprite1->setPosition(Vec2(visibleSize.width / 20, visibleSize.height -
		sugar_sprite1->getContentSize().height * sugar_sprite1->getScaleY() / 2 - visibleSize.height / 40));
	addChild(sugar_sprite1, 2);

	//current sugar info
	Label* sugar_amount = Label::createWithTTF("x " + root["holy_sugar"].asString(), "fonts/LICABOLD.ttf", round(visibleSize.width / 25));
	sugar_amount->setColor(Color3B::YELLOW);
	sugar_amount->enableOutline(Color4B::BLACK, 2);
	sugar_amount->setPosition(sugar_sprite1->getPosition() +
		Vec2(sugar_sprite1->getContentSize().width * sugar_sprite1->getScaleX() / 2 + visibleSize.width / 20, 0));
	addChild(sugar_amount, 2, "sugar_amount");

	// Pages
	ui::PageView* worlds = ui::PageView::create();
	worlds->setContentSize(Size(visibleSize.width,visibleSize.height));
	worlds->setPosition(Vec2(visibleSize.width/2 - worlds->getContentSize().width / 2,
		visibleSize.height/2 - worlds->getContentSize().height / 2));
	worlds->setCustomScrollThreshold(visibleSize.width * 0.1f);

	Json::Value level_config = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::LEVEL);
	Json::Value buttons = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::BUTTON);

	int worlds_count = level_config["worlds"].size();

	for (int i = 0; i < worlds_count; ++i){
		ui::Layout* page = ui::Layout::create();
		ui::Layout* layout = ui::Layout::create();
		page->setContentSize(Size(worlds->getContentSize().width*0.6, worlds->getContentSize().height));
		layout->setPosition(Vec2(worlds->getContentSize().width / 2.0f, worlds->getContentSize().height * 0.45f));

		ui::Button* world = ui::Button::create("res/background/levels2.png");
		world->setZoomScale(0);
		world->addTouchEventListener([&, layout, world](Ref* sender, ui::Widget::TouchEventType type) {
			if (type == ui::Widget::TouchEventType::ENDED) {
				if (layout->getScale() == 1) {
					layout->runAction(ScaleTo::create(0.25f, 1.2f));
				}
				else {
					layout->setScale(1.2f);
					layout->runAction(ScaleTo::create(0.25f, 1.f));
				}
			}
		});
		if (level_config["worlds"][i]["levels"].size() == 0) {
			world->setEnabled(false);
			ui::Text* label = ui::Text::create(
				buttons["soon"][language].asString(),
				"fonts/Marker Felt.ttf", visibleSize.width / 20);
			label->setColor(Color3B(192, 192, 192));
			label->enableOutline(Color4B::BLACK, 5);
			label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
			page->addChild(label, 2, "label");
		}
		else {
			ui::Text* label = ui::Text::create(level_config["worlds"][i]["name_" + language].asString(),
				"fonts/LICABOLD.ttf", visibleSize.width / 20);
			label->setColor(Color3B::YELLOW);
			label->enableOutline(Color4B::BLACK, 5);
			label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 0.90));
			page->addChild(label,2,"label");
		}
		world->setScale9Enabled(true);
		world->setScale(page->getContentSize().width / world->getContentSize().width);
		layout->addChild(world,0,"world");

		// selection of levels
		ui::Layout* levels = ui::Layout::create();
		levels->setPosition(Vec2(world->getContentSize().width / 2,
			world->getContentSize().height / 2));
		levels->setContentSize(world->getContentSize());

		world->addChild(levels, 1, "levels");
		initLevels(levels, i);
		page->addChild(layout,1,"layout");
		worlds->insertPage(page, i);
	}
	ui::Button* previous = ui::Button::create("res/buttons/next_world_button.png");
	previous->setScale(visibleSize.width / 15 / previous->getContentSize().width);
	previous->setPosition(Vec2(visibleSize.width * 0.05 + previous->getContentSize().width * abs(previous->getScale()) / 2, visibleSize.height / 2));

	ui::Button* next = ui::Button::create("res/buttons/next_world_button.png");
	next->setScale(-visibleSize.width / 15 / next->getContentSize().width);
	next->setPosition(Vec2(visibleSize.width * 0.95 - next->getContentSize().width * abs(next->getScale()) / 2, visibleSize.height / 2));
	previous->addTouchEventListener([&, next, previous, worlds](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			worlds->scrollToPage(worlds->getCurPageIndex() - 1);
		}
	});
	next->addTouchEventListener([&, next, previous, worlds](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			worlds->scrollToPage(worlds->getCurPageIndex() + 1);
		}
	});
	addChild(next, 2, "next_world");
	addChild(previous, 2, "previous_world");
	addChild(worlds, 1, "worlds");

	worlds->addEventListener(CC_CALLBACK_2(StoryMenu::changeWorld, this));
	worlds->setCurPageIndex(((AppDelegate*)Application::getInstance())->getSave()["c_world"].asInt());
	changeWorld(nullptr, ui::PageView::EventType::TURNING);
	

	double offset_horizontal = visibleSize.width / 32;
	double offset_vertical = 0;
	
	// Interface with buttons and settings
	addChild(ui::Layout::create(), 2, "interface");
	ui::Button* level_editor = ui::Button::create("res/buttons/menu_button_editor.png");
	level_editor->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			((SceneManager*)SceneManager::getInstance())->setScene(SceneManager::SceneType::EDITOR);
		}
	});
	level_editor->setScale(visibleSize.width / 8 / level_editor->getContentSize().width);
	level_editor->setPosition(Vec2(visibleSize.width - offset_horizontal, offset_vertical));
	level_editor->setAnchorPoint(Vec2(1.f, 0.f));
	getChildByName("interface")->addChild(level_editor);

	ui::Button* achivements = ui::Button::create("res/buttons/menu_button_score.png");
	achivements->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			//((SceneManager*)SceneManager::getInstance())->setScene(SceneManager::SceneType::EDITOR);
		}
	});
	achivements->setScale(visibleSize.width / 8 / achivements->getContentSize().width);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	achivements->setPosition(Vec2(level_editor->getPosition().x -
		level_editor->getContentSize().width * level_editor->getScaleX() - offset_horizontal, offset_vertical));
#else
	achivements->setPosition(Vec2(visibleSize.width - offset_horizontal, offset_vertical));
	level_editor->setVisible(false);
#endif
	achivements->setAnchorPoint(Vec2(1.f, 0.f));
	getChildByName("interface")->addChild(achivements);

	ui::Button* skill_tree = ui::Button::create("res/buttons/menu_button_skill_tree.png");
	skill_tree->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			((SceneManager*)SceneManager::getInstance())->setScene(SceneManager::SceneType::SKILLS);
		}
	});
	skill_tree->setScale(visibleSize.width / 8 / skill_tree->getContentSize().width);
	skill_tree->setPosition(Vec2(achivements->getPosition().x -
		achivements->getContentSize().width * achivements->getScaleX() - offset_horizontal, offset_vertical));
	skill_tree->setAnchorPoint(Vec2(1.f, 0.f));
	getChildByName("interface")->addChild(skill_tree,1,"skill_tree");

	ui::Button* shop = ui::Button::create("res/buttons/menu_button_shop.png");
	shop->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			((SceneManager*)SceneManager::getInstance())->setScene(SceneManager::SceneType::SHOP);
		}
	});
	shop->setScale(visibleSize.width / 8 / skill_tree->getContentSize().width);
	shop->setPosition(Vec2(skill_tree->getPosition().x -
		skill_tree->getContentSize().width * skill_tree->getScaleX() - offset_horizontal, offset_vertical));
	shop->setAnchorPoint(Vec2(1.f, 0.f));
	getChildByName("interface")->addChild(shop);

	auto settings = ui::Layout::create();
	addChild(settings, 3, "settings");
	ui::Button* panel = ui::Button::create("res/buttons/centralMenuPanel2.png");
	panel->setZoomScale(0);
	settings->addChild(panel, 1, "panel");
	panel->setScale9Enabled(true);
	panel->setScale(0.45*visibleSize.width / panel->getContentSize().width);
	//panel->setScaleY(0.6*visibleSize.height / panel->getContentSize().height);

	settings->setPosition(Vec2(visibleSize.width / 2, visibleSize.height +
		getChildByName("settings")->getChildByName("panel")->getContentSize().height *
		getChildByName("settings")->getChildByName("panel")->getScaleY()));

	auto close = ui::Button::create("res/buttons/close2.png");
	close->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if(type == ui::Widget::TouchEventType::ENDED){
			Size visibleSize = Director::getInstance()->getVisibleSize();
			auto* showAction = EaseBackIn::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height + 
				getChildByName("settings")->getChildByName("panel")->getContentSize().width *
				getChildByName("settings")->getChildByName("panel")->getScaleY() * 0.6)));
			getChildByName("settings")->runAction(showAction);
			getChildByName("black_mask")->setVisible(false);
		}
	});
	close->setScale(panel->getContentSize().width*panel->getScaleX() / 8 / close->getContentSize().width);
	close->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() / 2,
		panel->getContentSize().height*panel->getScaleY() / 2));
	Sprite* close_shadow = Sprite::create("res/buttons/close2_shadow.png");
	close_shadow->setScale(close->getScale() * 1.05);
	close_shadow->setPosition(close->getPosition());
	settings->addChild(close_shadow, -1);
	settings->addChild(close, 5, "close");

	Label* title = Label::createWithTTF(buttons["settings"][language].asString(), "fonts/LICABOLD.ttf", 45.0f * visibleSize.width / 1280);
	title->setColor(Color3B::WHITE);
	title->enableOutline(Color4B::BLACK, 2);
	title->setPosition(0, panel->getContentSize().height*panel->getScaleY() / 2 - title->getContentSize().height);
	settings->addChild(title, 2, "title");


	Label* music = Label::createWithTTF(buttons["music"][language].asString(), "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	Label* effects = Label::createWithTTF(buttons["effects"][language].asString(), "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	Label* loop = Label::createWithTTF(buttons["loop_music"][language].asString(), "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
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
	settings->addChild(music, 2, "music");
	settings->addChild(effects, 2, "effects");
	settings->addChild(loop, 2, "loop");

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
	Label* always_show_grid = Label::createWithTTF(buttons["grid_always"][language].asString(), "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	Label* moving_show_grid = Label::createWithTTF(buttons["grid_move"][language].asString(), "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	Label* never_show_grid = Label::createWithTTF(buttons["grid_never"][language].asString(), "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);

	Label* show_grid = Label::createWithTTF(buttons["show_grid"][language].asString(), "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	Label* auto_limit = Label::createWithTTF(buttons["auto_limit"][language].asString(), "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	Label* skip_dialogues = Label::createWithTTF(buttons["play_dialogues"][language].asString(), "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
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

	settings->addChild(always_show_grid, 2, "always_show_grid");
	settings->addChild(moving_show_grid, 2, "moving_show_grid");
	settings->addChild(never_show_grid, 2, "never_show_grid");
	settings->addChild(show_grid, 2, "show_grid");
	settings->addChild(auto_limit, 2, "auto_limit");
	settings->addChild(skip_dialogues, 2, "skip_dialogues");

	auto checkbox_grid = cocos2d::ui::CheckBox::create("res/buttons/checkbox.png", "res/buttons/checkbox.png",
		"res/buttons/filled.png", "res/buttons/checkbox.png", "res/buttons/checkbox.png");
	((AppDelegate*)Application::getInstance())->getConfigClass()->addGridButton(checkbox_grid);
	checkbox_grid->setPosition(Vec2(always_show_grid->getPosition().x,
		show_grid->getPosition().y));
	checkbox_grid->setSelected(((AppDelegate*)Application::getInstance())->getConfigClass()->isAlwaysGridEnabled());
	checkbox_grid->setScale(visibleSize.width / 1280);
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
	checkbox_never_grid->setScale(visibleSize.width / 1280);
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
	checkbox_moving_grid->setScale(visibleSize.width / 1280);
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
	checkbox_limit->setScale(visibleSize.width / 1280);
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
	checkbox_dialogues->setScale(visibleSize.width / 1280);
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
	
	//auto credits = ui::Button::create("res/buttons/buttonCredits.png");
	auto credits = ui::Button::create("res/buttons/yellow_button.png");
	credits->setTitleText(buttons["credits"][language].asString());
	credits->setTitleFontName("fonts/LICABOLD.ttf");
	credits->setTitleFontSize(60.f);
	Label* credits_label = credits->getTitleRenderer();
	credits_label->setColor(Color3B::WHITE);
	credits_label->enableOutline(Color4B::BLACK,2);
	credits_label->setPosition(credits->getContentSize() / 2);

	credits->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Size visibleSize = Director::getInstance()->getVisibleSize();
			auto* hideAction = TargetedAction::create(getChildByName("settings"),
				EaseBackIn::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height * 1.5))));
			auto callbackmainmenu = CallFunc::create([&]() {
				getChildByName("black_mask")->setVisible(false);
				SceneManager::getInstance()->setScene(SceneManager::CREDIT);
			});
			getChildByName("settings")->runAction(Sequence::create(hideAction, callbackmainmenu, nullptr));

		}
	});
	credits->setScale(panel->getContentSize().width*panel->getScaleX() / 2 / credits->getContentSize().width);
	credits->setPosition(Vec2(-panel->getContentSize().width*panel->getScaleX() / 4,
		-panel->getContentSize().height*panel->getScaleY() / 2 -
		credits->getContentSize().height*credits->getScaleY() * 0.41));
	Sprite* credits_shadow = Sprite::create("res/buttons/shadow_button.png");
	credits_shadow->setScale(credits->getScale());
	credits_shadow->setPosition(credits->getPosition());
	settings->addChild(credits_shadow, -1);
	settings->addChild(credits, 5, "credits");

	auto title_menu = ui::Button::create("res/buttons/red_button.png");
	title_menu->setTitleText(buttons["title_menu"][language].asString());
	title_menu->setTitleFontName("fonts/LICABOLD.ttf");
	title_menu->setTitleFontSize(60.f);
	Label* title_menu_label = title_menu->getTitleRenderer();
	title_menu_label->setColor(Color3B::WHITE);
	title_menu_label->enableOutline(Color4B::BLACK, 2);
	title_menu_label->setPosition(title_menu->getContentSize() / 2);

	title_menu->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Size visibleSize = Director::getInstance()->getVisibleSize();
			auto* hideAction = TargetedAction::create(getChildByName("settings"),
				EaseBackIn::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height * 1.5))));
			auto callbackmainmenu = CallFunc::create([&]() {
				getChildByName("black_mask")->setVisible(false);
				SceneManager::getInstance()->setScene(SceneManager::MENU);
			});
			getChildByName("settings")->runAction(Sequence::create(hideAction, callbackmainmenu, nullptr));
		}
	});
	title_menu->setScale(panel->getContentSize().width*panel->getScaleX() / 2 / title_menu->getContentSize().width);
	title_menu->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() / 4,
		-panel->getContentSize().height*panel->getScaleY() / 2 -
		title_menu->getContentSize().height*title_menu->getScaleY() * 0.41));
	Sprite* title_menu_shadow = Sprite::create("res/buttons/shadow_button.png");
	title_menu_shadow->setScale(title_menu->getScale());
	title_menu_shadow->setPosition(title_menu->getPosition());
	settings->addChild(title_menu_shadow, -1);
	settings->addChild(title_menu, 5, "title_menu");

	ui::Button* show_setting = ui::Button::create("res/buttons/settings2.png");
	show_setting->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Size visibleSize = Director::getInstance()->getVisibleSize();
			auto* showAction = EaseBackOut::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height / 2)));
			getChildByName("black_mask")->setVisible(true);
			getChildByName("settings")->runAction(showAction);
		}
	});
	show_setting->setScale(visibleSize.width / 15 / show_setting->getContentSize().width);
	show_setting->setPosition(Vec2(visibleSize.width - show_setting->getContentSize().width * show_setting->getScale() / 2,
		visibleSize.height - show_setting->getContentSize().height * show_setting->getScale() / 2));
	getChildByName("interface")->addChild(show_setting);

	return true;
}

void StoryMenu::changeWorld(cocos2d::Ref* sender, cocos2d::ui::PageView::EventType type) {
	int a = ((ui::PageView*)getChildByName("worlds"))->getCurPageIndex();
	if (a == 0) {
		((ui::Button*)getChildByName("previous_world"))->setVisible(false);
		((ui::Button*)getChildByName("next_world"))->setVisible(true);
	}
	else if (a == ((ui::PageView*)getChildByName("worlds"))->getPages().size() - 1) {
		((ui::Button*)getChildByName("next_world"))->setVisible(false);
		((ui::Button*)getChildByName("previous_world"))->setVisible(true);
	}
	else {
		((ui::Button*)getChildByName("previous_world"))->setVisible(true);
		((ui::Button*)getChildByName("next_world"))->setVisible(true);
	}
}

void StoryMenu::selectLevelCallBack(Ref* sender, ui::Widget::TouchEventType type, int level_id, int world_id){
	if (type == ui::Widget::TouchEventType::ENDED) {
		SceneManager::getInstance()->getGame()->initLevel(level_id,world_id);
		SceneManager::getInstance()->setScene(SceneManager::GAME);
	}
}

void StoryMenu::onEnterTransitionDidFinish(){
	Scene::onEnterTransitionDidFinish();
	scheduleUpdate();
	Size visibleSize = Director::getInstance()->getVisibleSize();

	for (unsigned int i(0); i < ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::LEVEL)["worlds"].size(); ++i) {
		((ui::PageView*)getChildByName("worlds"))->getPage(i)->getChildByName("layout")->getChildByName("world")->
			getChildByName("levels")->removeAllChildren();
		initLevels((ui::Layout*)((ui::PageView*)getChildByName("worlds"))->getPage(i)->getChildByName("layout")->getChildByName("world")->
			getChildByName("levels"),i);
		((ui::PageView*)getChildByName("worlds"))->getPage(i)->getChildByName("layout")->getChildByName("world")->stopAllActions();
		((ui::PageView*)getChildByName("worlds"))->getPage(i)->getChildByName("layout")->getChildByName("world")->
			runAction(RepeatForever::create(
				Sequence::create(MoveTo::create(5.f, Vec2(0, -visibleSize.height / 40)),
			MoveTo::create(5.f, Vec2(0, visibleSize.height / 40)), nullptr)));
	}
}

void StoryMenu::showCredit(Ref* sender){
	SceneManager::getInstance()->setScene(SceneManager::CREDIT);
}

void StoryMenu::initLevels(ui::Layout* page, int id_world) {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	double ratioX = visibleSize.width / 1280;
	double ratioY = visibleSize.height / 720;
	Json::Value levels = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::LEVEL)["worlds"][id_world]["levels"];
	int cLevel = ((AppDelegate*)Application::getInstance())->getConfigClass()->getSaveValues()["c_level"].asInt();
	int cWorld = ((AppDelegate*)Application::getInstance())->getConfigClass()->getSaveValues()["c_world"].asInt();

	for (unsigned int i(0); i < levels.size(); ++i) {
		std::string filename;
		Color3B color;
		bool enable(true);
		if ((int)i > cLevel || id_world > cWorld) {
			filename = "res/buttons/level_button_disable.png";
			color = Color3B(50, 50, 50);
			enable = false;
		}
		else if ((int)i == cLevel && id_world == cWorld) {
			filename = "res/buttons/level_button.png";
			color = Color3B(220, 168, 17);
		}
		else if ((int)i < cLevel && id_world <= cWorld) {
			filename = "res/buttons/level_button_done.png";
			color = Color3B::WHITE;
		}
		ui::Layout* level_layout = ui::Layout::create();
		ui::Button* level = ui::Button::create(filename);
		level->addTouchEventListener(CC_CALLBACK_2(StoryMenu::selectLevelCallBack, this, i, id_world));
		//level->setPosition(Vec2(levels[i]["x"].asInt() * ratioX, levels[i]["y"].asInt() * ratioY));
		Vec2 pos_level = Vec2(levels[i]["x"].asFloat() * page->getContentSize().width,
			levels[i]["y"].asFloat() * page->getContentSize().height);
		//level->setPosition(pos_level);
		level->setEnabled(enable);
		level->setScale(page->getContentSize().width * 0.065 / level->getContentSize().width);
		level_layout->addChild(level);
		level_layout->setPosition(pos_level);

		int nb_challenges = ((AppDelegate*)Application::getInstance())->getConfigClass()->getNbLevelChallenges(id_world, i);
		int side = (1 - 2 * (i % 2));

		Sprite* star_left = Sprite::create(nb_challenges >= 1 ? "res/buttons/small_star_full.png" : "res/buttons/small_star_empty.png");
		star_left->setScale(level->getContentSize().height * level->getScaleY() / 2 / star_left->getContentSize().width);
		star_left->setPosition(Vec2(-star_left->getContentSize().width * star_left->getScaleX(),
			side * (level->getContentSize().height * level->getScaleY() * 2 / 3 + star_left->getContentSize().height * star_left->getScaleY() / 2)));
		star_left->setRotation(-35);
		Sprite* star_middle = Sprite::create(nb_challenges >= 2 ? "res/buttons/small_star_full.png" : "res/buttons/small_star_empty.png");
		star_middle->setScale(level->getContentSize().height * level->getScaleY() / 2 / star_middle->getContentSize().width);
		star_middle->setPosition(Vec2(0, star_left->getPosition().y + side * star_left->getContentSize().width * star_left ->getScale() / 3));
		Sprite* star_right = Sprite::create(nb_challenges == 3 ? "res/buttons/small_star_full.png" : "res/buttons/small_star_empty.png");
		star_right->setScale(level->getContentSize().height * level->getScaleY() / 2 / star_right->getContentSize().width);
		star_right->setRotation(35);
		star_right->setPosition(Vec2(star_left->getContentSize().width * star_left->getScaleX(), star_left->getPosition().y));
		level_layout->addChild(star_left, 2, "star_left");
		level_layout->addChild(star_middle, 2, "star_middle");
		level_layout->addChild(star_right, 2, "star_right");

		/*Label* level_label = Label::createWithTTF(to_string(i + 1), "fonts/LICABOLD.ttf", 40);
		level_label->setPosition(Vec2(0, level->getContentSize().height * level->getScaleY() / 2));
		level_label->setAnchorPoint(Vec2(0.5, 0));
		level_label->setColor(color);
		level_label->enableOutline(Color4B::BLACK, 1);
		level_layout->addChild(level_label);*/
		page->addChild(level_layout);
	}
}

void StoryMenu::switchLanguage() {
}

void StoryMenu::update(float dt) {
	updateTutorial(dt);
}

void StoryMenu::updateTutorial(float dt) {
	auto save = ((AppDelegate*)Application::getInstance())->getConfigClass()->getSaveValues();
	auto config = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::TUTORIAL);

	if (!((AppDelegate*)Application::getInstance())->getConfigClass()->isTutorialComplete("skills") &&
		save["c_level"].asInt() >= config["skills"]["level"].asInt() &&
		save["c_world"].asInt() >= config["skills"]["world"].asInt()) {
		
		if (getChildByName("dialogue") == nullptr && !tutorial_running && 
			!((AppDelegate*)Application::getInstance())->getConfigClass()->isTutorialRunning("skills")) {
			Json::Value save = ((AppDelegate*)Application::getInstance())->getSave();
			addChild(Dialogue::createFromConfig(config["skills"]["dialogue"]), 3, "dialogue");
			((Dialogue*)getChildByName("dialogue"))->launch();
			tutorial_running = true;
			Size visibleSize = Director::getInstance()->getVisibleSize();

			((AppDelegate*)Application::getInstance())->getConfigClass()->startTutorial("skills");

			// mask to prevent any action from the player
			addChild(ui::Layout::create(), 2, "invisble_mask");
			ui::Button* mask = ui::Button::create("res/buttons/tranparent_mask.png");
			mask->setScaleX(visibleSize.width / mask->getContentSize().width);
			mask->setScaleY(visibleSize.height / mask->getContentSize().height);
			mask->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
			getChildByName("invisble_mask")->addChild(mask);
		}
		else if (getChildByName("dialogue") != nullptr){
			((Dialogue*)getChildByName("dialogue"))->update();
			if (((Dialogue*)getChildByName("dialogue"))->hasFinished()) {
				removeChildByName("dialogue");
				Size visibleSize = Director::getInstance()->getVisibleSize();

				// hand showing what to do
				Sprite* hand = Sprite::create("res/buttons/hand.png");
				hand->setAnchorPoint(Vec2(0.15f, 0.5f));
				hand->setScale(visibleSize.width / 10 / hand->getContentSize().width);
				hand->setPosition(visibleSize / 2);
				addChild(hand, 3, "hand");
				hand->setOpacity(0.f);
				auto switch_to_skills = CallFunc::create([this]() {
					this->removeChildByName("invisble_mask");
					this->removeChildByName("hand");
					tutorial_running = false;
					((SceneManager*)SceneManager::getInstance())->setScene(SceneManager::SceneType::SKILLS);
				});
				hand->runAction(Sequence::create(
					FadeIn::create(0.5f),
					DelayTime::create(0.5f),
					Spawn::createWithTwoActions(
						MoveBy::create(1.5f, Vec2(getChildByName("interface")->getChildByName("skill_tree")->getPosition().x -
							getChildByName("interface")->getChildByName("skill_tree")->getContentSize().width *
							getChildByName("interface")->getChildByName("skill_tree")->getScaleX() / 2 -
							visibleSize.width / 2, 0)),
						EaseBackOut::create(MoveBy::create(1.5f, Vec2(0, getChildByName("interface")->getChildByName("skill_tree")->getPosition().y - visibleSize.height / 2)))),
					DelayTime::create(0.5f),
					FadeOut::create(0.5f),
					switch_to_skills,
					nullptr)
				);
			}
		}
	}
}

