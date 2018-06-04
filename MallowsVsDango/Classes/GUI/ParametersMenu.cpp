#include "ParametersMenu.h"
#include "../AppDelegate.h"

ParametersMenu* ParametersMenu::create(MyGame* game)
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
	addPanel(visibleSize, 0.45);
	addTitle(buttons, language, visibleSize);
	addBottomButtons(buttons, language);
	addSoundController(buttons["music"][language].asString(), visibleSize, AudioController::SOUNDTYPE::MUSIC);
	addSoundController(buttons["effects"][language].asString(), visibleSize, AudioController::SOUNDTYPE::EFFECT);
	addMusicLoopCheckBox(buttons, language, visibleSize, audioController);
	addGlobalSettings(buttons, language, visibleSize, config);
	return initialized;
}

void ParametersMenu::addBottomButtons(Json::Value &buttons, std::string &language)
{
	addLeftButton(buttons["resume"][language].asString());
	addRightButton(buttons["main_menu"][language].asString());
}

void ParametersMenu::addMusicLoopCheckBox(Json::Value &buttons, std::string &language, const cocos2d::Size &visibleSize, AudioController * audioController)
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

	auto checkboxAlwaysGrid = createNormalCheckBox(visibleSize, cocos2d::Vec2(always_show_grid->getPosition().x, show_grid->getPosition().y));
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

	auto checkboxMovingGrid = createNormalCheckBox(visibleSize, cocos2d::Vec2(moving_show_grid->getPosition().x, show_grid->getPosition().y));
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

void ParametersMenu::rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		game->updateTracker("left");
		auto callbackmainmenu = cocos2d::CallFunc::create([&]() {
			SceneManager::getInstance()->setScene(SceneManager::LEVELS);
		});
		this->runAction(cocos2d::Sequence::create(createHideAction(this), callbackmainmenu, nullptr));
		blackMask->setVisible(false);
	}
}

void ParametersMenu::leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		this->runAction(createHideAction(this));
		blackMask->setVisible(false);
		game->resume();
	}
}

void ParametersMenu::addBlackMask(const cocos2d::Size &visibleSize)
{
	blackMask = cocos2d::ui::Layout::create();
	addChild(blackMask, 0, "black_mask");
	cocos2d::ui::Button* mask = cocos2d::ui::Button::create("res/buttons/mask.png");
	mask->setScaleX(3 * visibleSize.width / mask->getContentSize().width);
	mask->setScaleY(3 * visibleSize.height / mask->getContentSize().height);
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

ParametersMenu::~ParametersMenu() {}
