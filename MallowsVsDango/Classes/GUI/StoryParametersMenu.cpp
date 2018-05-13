#include "StoryParametersMenu.h"

StoryParametersMenu * StoryParametersMenu::create(MyGame* game)
{
	StoryParametersMenu* menu = new (std::nothrow) StoryParametersMenu();
	if (menu && menu->init(game))
	{
		menu->autorelease();
		return menu;
	}
	CC_SAFE_DELETE(menu);
	return nullptr;
}

bool StoryParametersMenu::init(MyGame* game) {
	bool initialized = ParametersMenu::init(game);
	cocos2d::ui::Button* close = cocos2d::ui::Button::create("res/buttons/close2.png");
	close->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			this->runAction(createHideAction(this));
			blackMask->setVisible(false);
		}
	});
	close->setScale(panelSize.width / 8 / close->getContentSize().width);
	close->setPosition(cocos2d::Vec2(panelSize.width / 2, panelSize.height / 2));
	cocos2d::Sprite* close_shadow = cocos2d::Sprite::create("res/buttons/close2_shadow.png");
	close_shadow->setScale(close->getScale() * 1.05);
	close_shadow->setPosition(close->getPosition());
	addChild(close_shadow, -1);
	addChild(close, 5, "close");
	return initialized;
}

void StoryParametersMenu::addRightButton(Json::Value buttons, const std::string& language) {
	cocos2d::ui::Button* titleScreenButton = createDefaultButton(buttons["title_menu"][language].asString(), 
		"res/buttons/red_button.png", panelSize.width);

	titleScreenButton->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			auto callbackTitleScreen = cocos2d::CallFunc::create([&]() {
				blackMask->setVisible(false);
				SceneManager::getInstance()->setScene(SceneManager::MENU);
			});
			this->runAction(cocos2d::Sequence::create(createHideAction(this), callbackTitleScreen, nullptr));
		}
	});
	titleScreenButton->setPosition(cocos2d::Vec2(panelSize.width / 4, 
		-panelSize.height / 2 - titleScreenButton->getContentSize().height*titleScreenButton->getScaleY() * 0.41));
	addChild(createButtonShadow(titleScreenButton), -1);
	addChild(titleScreenButton, 1, "titleScreenButton");
}
void StoryParametersMenu::addLeftButton(Json::Value buttons, const std::string& language) {
	cocos2d::ui::Button* creditsButton = createDefaultButton(buttons["credits"][language].asString(),
		"res/buttons/yellow_button.png", panelSize.width);

	creditsButton->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			auto callbackCredits = cocos2d::CallFunc::create([&]() {
				blackMask->setVisible(false);
				SceneManager::getInstance()->setScene(SceneManager::CREDIT);
			});
			this->runAction(cocos2d::Sequence::create(createHideAction(this), callbackCredits, nullptr));
		}
	});
	creditsButton->setPosition(cocos2d::Vec2(-panelSize.width / 4, 
		-panelSize.height / 2 - creditsButton->getContentSize().height*creditsButton->getScaleY() * 0.41));
	addChild(createButtonShadow(creditsButton), -1);
	addChild(creditsButton, 1, "titleScreenButton");
}

StoryParametersMenu::~StoryParametersMenu(){}