#include "ParametersMenu.h"
#include "../AppDelegate.h"

ParametersMenu * ParametersMenu::create(MyGame* game)
{
	ParametersMenu* menu = new (std::nothrow) ParametersMenu();
	if (menu && menu->init(game))
	{
		menu->autorelease();
		return menu;
	}
	CC_SAFE_DELETE(menu);
	return nullptr;
}

bool ParametersMenu::init(MyGame* game) {
	bool initialized = cocos2d::ui::Layout::init();
	this->game = game;
	const cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	setPosition(cocos2d::Vec2(visibleSize.width / 2, visibleSize.height * 1.5));
	Config* config = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass();
	std::string language = config->getLanguage();
	Json::Value buttons = config->getConfigValues(Config::ConfigType::BUTTON);
	AudioController* audioController = ((AppDelegate*)cocos2d::Application::getInstance())->getAudioController();

	addBlackMask(visibleSize);
	addPanel(visibleSize);
	addTitle(buttons, language, visibleSize);
	addLeftButton(buttons, language);
	addRightButton(buttons, language);
	addSoundController(buttons["music"][language].asString(), visibleSize, AudioController::SOUNDTYPE::MUSIC);
	addSoundController(buttons["effects"][language].asString(), visibleSize, AudioController::SOUNDTYPE::EFFECT);
	addLoopCheckBox(buttons, language, visibleSize, audioController);
	addGlobalSettings(buttons, language, visibleSize, config);
	return initialized;
}

void ParametersMenu::addLoopCheckBox(Json::Value &buttons, std::string &language, const cocos2d::Size &visibleSize, AudioController * audioController)
{
	auto checkboxLoop = createCheckBoxWithLabel(buttons["loop_music"][language].asString(), visibleSize, 0);
	audioController->addButtonLoop(checkboxLoop);
	addChild(checkboxLoop, 6, "LoopEnable");
}

void ParametersMenu::addTitle(Json::Value &buttons, std::string &language, const cocos2d::Size &visibleSize)
{
	cocos2d::Label* title = cocos2d::Label::createWithTTF(buttons["settings"][language].asString(), "fonts/LICABOLD.ttf", 45.0f * visibleSize.width / 1280);
	title->setColor(cocos2d::Color3B::BLACK);
	title->setPosition(0, panelSize.height / 2 - title->getContentSize().height);
	addChild(title, 2, "title");
	lastObjectPosition = title->getPosition();
	lastObjectSize = title->getContentSize();
}

void ParametersMenu::addPanel(const cocos2d::Size &visibleSize)
{
	cocos2d::ui::Button* panel = cocos2d::ui::Button::create("res/buttons/centralMenuPanel2.png");
	panel->setZoomScale(0);
	panel->setScale(0.45*visibleSize.width / panel->getContentSize().width);
	addChild(panel, 1, "panel");
	panelSize = cocos2d::Size(panel->getContentSize().width * panel->getScaleX(), panel->getContentSize().height * panel->getScaleY());
}

cocos2d::Label* ParametersMenu::createLabelForGridCheckBox(std::string title, double fontSize) {
	cocos2d::Label* label = cocos2d::Label::createWithTTF(title, "fonts/LICABOLD.ttf", fontSize);
	label->setColor(cocos2d::Color3B::BLACK);
	return label;
}

void ParametersMenu::addGlobalSettings(Json::Value &buttons, std::string &language, const cocos2d::Size &visibleSize, Config * config)
{
	double fontSize = 30.f * visibleSize.width / 1280;
	cocos2d::Label* always_show_grid = createLabelForGridCheckBox(buttons["grid_always"][language].asString(), fontSize);
	cocos2d::Label* moving_show_grid = createLabelForGridCheckBox(buttons["grid_move"][language].asString(), fontSize);
	cocos2d::Label* never_show_grid = createLabelForGridCheckBox(buttons["grid_never"][language].asString(), fontSize);
	cocos2d::Label* show_grid = createLabelForGridCheckBox(buttons["show_grid"][language].asString(), fontSize);
	addChild(always_show_grid, 2);
	addChild(moving_show_grid, 2);
	addChild(never_show_grid, 2);
	addChild(show_grid, 2);

	double offset = panelSize.width / 40;

	never_show_grid->setPosition(0, lastObjectPosition.y - lastObjectSize.height / 2 -
		always_show_grid->getContentSize().height * 3 / 4);
	moving_show_grid->setPosition(never_show_grid->getPosition().x + never_show_grid->getContentSize().width / 2 +
		moving_show_grid->getContentSize().width / 2 + offset, lastObjectPosition.y - lastObjectSize.height / 2 -
		always_show_grid->getContentSize().height * 3 / 4);
	int posXBox = moving_show_grid->getPosition().x;
	always_show_grid->setPosition(moving_show_grid->getPosition().x + moving_show_grid->getContentSize().width / 2 +
		always_show_grid->getContentSize().width / 2 + offset, lastObjectPosition.y - lastObjectSize.height / 2 -
		always_show_grid->getContentSize().height * 3 / 4);
	show_grid->setPosition(-panelSize.width * 2 / 5 +
		show_grid->getContentSize().width / 2, always_show_grid->getPosition().y - always_show_grid->getContentSize().height / 2 -
		show_grid->getContentSize().height * 3 / 4);

	auto checkboxAlwaysGrid = createNormalCheckBox(visibleSize,	cocos2d::Vec2(always_show_grid->getPosition().x, show_grid->getPosition().y));
	config->addGridButton(checkboxAlwaysGrid);
	checkboxAlwaysGrid->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->enableAlwaysGrid(true);
			if (game != nullptr) {
				game->getLevel()->showGrid(true);
			}
		}
	});

	auto checkboxNeverGrid = createNormalCheckBox(visibleSize, cocos2d::Vec2(never_show_grid->getPosition().x, show_grid->getPosition().y));
	config->addNeverGridButton(checkboxNeverGrid);
	checkboxNeverGrid->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->enableNeverGrid(true);
			if (game != nullptr) {
				game->getLevel()->showGrid(false);
			}
		}
	});

	auto checkboxMovingGrid = createNormalCheckBox(visibleSize,	cocos2d::Vec2(moving_show_grid->getPosition().x, show_grid->getPosition().y));
	config->addMovingGridButton(checkboxMovingGrid);
	checkboxMovingGrid->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->enableMovingGrid(true);
			if (game != nullptr) {
				game->getLevel()->showGrid(false);
			}
		}
	});
	addChild(checkboxAlwaysGrid, 6, "GridEnable");
	addChild(checkboxNeverGrid, 6, "NeverGridEnable");
	addChild(checkboxMovingGrid, 6, "MovingGridEnable");
	lastObjectPosition = show_grid->getPosition();
	lastObjectSize = show_grid->getContentSize();

	auto checkBoxLimit = createCheckBoxWithLabel(buttons["auto_limit"][language].asString(), visibleSize, 0);
	config->addLimitButton(checkBoxLimit);

	auto checkBoxDialogues = createCheckBoxWithLabel(buttons["play_dialogues"][language].asString(), visibleSize, 0);
	config->addDialogueButton(checkBoxDialogues);

	addChild(checkBoxLimit, 6, "LimitEnable");
	addChild(checkBoxDialogues, 6, "DialogueEnable");
}

cocos2d::ui::CheckBox* ParametersMenu::createCheckBoxWithLabel(std::string label, const cocos2d::Size& visibleSize, int posXBox) {
	cocos2d::Label* labelObject = cocos2d::Label::createWithTTF(label, "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	labelObject->setColor(cocos2d::Color3B::BLACK);
	labelObject->setPosition(-panelSize.width * 2 / 5 +
		labelObject->getContentSize().width / 2, lastObjectPosition.y - lastObjectSize.height / 2 -
		labelObject->getContentSize().height * 3 / 4);
	addChild(labelObject, 2);
	lastObjectPosition = labelObject->getPosition();
	lastObjectSize = labelObject->getContentSize();
	return createNormalCheckBox(visibleSize, cocos2d::Vec2(posXBox, labelObject->getPosition().y));
}

cocos2d::ui::CheckBox* ParametersMenu::createNormalCheckBox(const cocos2d::Size& visibleSize, cocos2d::Vec2 position) {
	auto checkBox = cocos2d::ui::CheckBox::create("res/buttons/checkbox.png", "res/buttons/checkbox.png",
		"res/buttons/filled.png", "res/buttons/checkbox.png", "res/buttons/checkbox.png");
	checkBox->setPosition(position);
	checkBox->setScale(visibleSize.width / 1280);
	return checkBox;
}

void ParametersMenu::addSoundController(const std::string& title, cocos2d::Size visibleSize, AudioController::SOUNDTYPE type) {
	cocos2d::Label* controllerLabel = cocos2d::Label::createWithTTF(title, "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	controllerLabel->setColor(cocos2d::Color3B::BLACK);
	controllerLabel->setPosition(-panelSize.width * 2 / 5 +
		controllerLabel->getContentSize().width / 2, lastObjectPosition.y - lastObjectSize.height -
		controllerLabel->getContentSize().height * 3 / 4);

	auto controllerEnableCheckbox = cocos2d::ui::CheckBox::create("res/buttons/music.png", "res/buttons/music.png",
		"res/buttons/disable.png", "res/buttons/music.png", "res/buttons/music.png");
	controllerEnableCheckbox->setPosition(cocos2d::Vec2(panelSize.width * 2 / 5,
		controllerLabel->getPosition().y));
	controllerEnableCheckbox->setScale(visibleSize.width / 1280);
	((AppDelegate*)cocos2d::Application::getInstance())->getAudioController()->addButton(controllerEnableCheckbox, type);

	AudioSlider* controllerSlider = AudioSlider::create(AudioSlider::Horizontal);
	controllerSlider->setValue(0, 1, ((AppDelegate*)cocos2d::Application::getInstance())->getAudioController()->getMaxVolume(type));
	controllerSlider->setPosition(panelSize.width / 7, controllerLabel->getPosition().y);
	((AppDelegate*)cocos2d::Application::getInstance())->addAudioSlider(controllerSlider, type);
	controllerSlider->setScale(visibleSize.width / 1280);

	addChild(controllerLabel, 2);
	addChild(controllerSlider, 5);
	addChild(controllerEnableCheckbox, 6);
	lastObjectPosition = controllerLabel->getPosition();
	lastObjectSize = controllerLabel->getContentSize();
}

void ParametersMenu::addRightButton(Json::Value buttons, const std::string& language)
{
	cocos2d::ui::Button* mainMenuButton = createDefaultButton(buttons["main_menu"][language].asString(), "res/buttons/red_button.png", panelSize.width);
	mainMenuButton->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			game->updateTracker("left");
			auto callbackmainmenu = cocos2d::CallFunc::create([&]() {
				SceneManager::getInstance()->setScene(SceneManager::LEVELS);
			});
			this->runAction(cocos2d::Sequence::create(createHideAction(this), callbackmainmenu, nullptr));
			blackMask->setVisible(false);
		}
	});
	mainMenuButton->setPosition(cocos2d::Vec2(panelSize.width / 4, -panelSize.height / 2 -
		mainMenuButton->getContentSize().height*mainMenuButton->getScaleY() * 0.41));
	addChild(createButtonShadow(mainMenuButton), -1);
	addChild(mainMenuButton, 1, "main_menu_back");
}

void ParametersMenu::addLeftButton(Json::Value buttons, const std::string& language)
{
	cocos2d::ui::Button* resume = createDefaultButton(buttons["resume"][language].asString(), "res/buttons/yellow_button.png", panelSize.width);
	
	resume->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			this->runAction(createHideAction(this));
			blackMask->setVisible(false);
			game->resume();
		}
	});
	resume->setPosition(cocos2d::Vec2(-panelSize.width / 4, -panelSize.height / 2 - resume->getContentSize().height*resume->getScaleY() * 0.41));
	addChild(createButtonShadow(resume), -1);
	addChild(resume, 1, "resume");
}

cocos2d::TargetedAction* createHideAction(cocos2d::Node* target) {
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	return cocos2d::TargetedAction::create(target,
		cocos2d::EaseBackIn::create(
			cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(visibleSize.width / 2, visibleSize.height * 1.5))));
}


cocos2d::ui::Button* createDefaultButton(const std::string& buttonTitle, const std::string& buttonImage, int panelWidth) {
	cocos2d::ui::Button* button = cocos2d::ui::Button::create(buttonImage);
	button->setTitleText(buttonTitle);
	button->setTitleFontName("fonts/LICABOLD.ttf");
	button->setTitleFontSize(60.f);
	button->setScale(panelWidth * 0.4 / button->getContentSize().width);

	cocos2d::Label* button_label = button->getTitleRenderer();
	button_label->setColor(cocos2d::Color3B::WHITE);
	button_label->enableOutline(cocos2d::Color4B::BLACK, 2);
	button_label->setPosition(button->getContentSize() / 2);
	return button;
}

cocos2d::Sprite* createButtonShadow(cocos2d::ui::Button* button) {
	cocos2d::Sprite* buttonShadow = cocos2d::Sprite::create("res/buttons/shadow_button.png");
	buttonShadow->setScale(button->getScale());
	buttonShadow->setPosition(button->getPosition());
	return buttonShadow;
}

void ParametersMenu::addBlackMask(const cocos2d::Size &visibleSize)
{
	blackMask = cocos2d::ui::Layout::create();
	addChild(blackMask, 0, "black_mask");
	cocos2d::ui::Button* mask = cocos2d::ui::Button::create("res/buttons/mask.png");
	mask->setScaleX(3*visibleSize.width / mask->getContentSize().width);
	mask->setScaleY(3*visibleSize.height / mask->getContentSize().height);
	blackMask->addChild(mask);
	blackMask->setVisible(false);
}

void ParametersMenu::displayWithAnimation() {
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	auto* showAction = cocos2d::TargetedAction::create(this,
		cocos2d::EaseBackOut::create(cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2))));
	this->runAction(showAction);
	blackMask->setVisible(true);
}

ParametersMenu::~ParametersMenu(){}
