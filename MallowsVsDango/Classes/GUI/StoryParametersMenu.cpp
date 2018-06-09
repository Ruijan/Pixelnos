#include "StoryParametersMenu.h"
#include "../SceneManager.h"

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

void StoryParametersMenu::addBottomButtons(Json::Value & buttons)
{
	addLeftButton(buttons["credits"][settings->getLanguage()].asString());
	addRightButton(buttons["title_menu"][settings->getLanguage()].asString());
}

void StoryParametersMenu::rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		auto callbackTitleScreen = cocos2d::CallFunc::create([&]() {
			blackMask->setVisible(false);
			SceneManager::getInstance()->setScene(SceneManager::MENU);
		});
		this->runAction(cocos2d::Sequence::create(createHideAction(this), callbackTitleScreen, nullptr));
	}
}

void StoryParametersMenu::leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		auto callbackCredits = cocos2d::CallFunc::create([&]() {
			blackMask->setVisible(false);
			SceneManager::getInstance()->setScene(SceneManager::CREDIT);
		});
		this->runAction(cocos2d::Sequence::create(createHideAction(this), callbackCredits, nullptr));
	}
}

StoryParametersMenu::~StoryParametersMenu(){}