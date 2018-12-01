#include "StoryParametersMenu.h"
#include "../../SceneManager.h"

StoryParametersMenu * StoryParametersMenu::create(Config* config)
{
	StoryParametersMenu* menu = new (std::nothrow) StoryParametersMenu(config);
	if (menu && menu->init())
	{
		menu->autorelease();
		return menu;
	}
	CC_SAFE_DELETE(menu);
	return nullptr;
}

StoryParametersMenu::StoryParametersMenu(Config* config) :
	ParametersMenu(config->getSettings(), config->getGUISettings()) {}

bool StoryParametersMenu::init() {
	bool initialized = ParametersMenu::init();
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
	addLeftButton(buttons["credits"][gameSettings->getLanguage()].asString());
	addRightButton(buttons["title_menu"][gameSettings->getLanguage()].asString());
}

void StoryParametersMenu::rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		auto callbackTitleScreen = cocos2d::CallFunc::create([&]() {
			blackMask->setVisible(false);
			SceneManager::getInstance()->setScene(SceneFactory::MENU);
		});
		this->runAction(cocos2d::Sequence::create(createHideAction(this), callbackTitleScreen, nullptr));
	}
}

void StoryParametersMenu::leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		auto callbackCredits = cocos2d::CallFunc::create([&]() {
			blackMask->setVisible(false);
			SceneManager::getInstance()->setScene(SceneFactory::CREDIT);
		});
		this->runAction(cocos2d::Sequence::create(createHideAction(this), callbackCredits, nullptr));
	}
}

StoryParametersMenu::~StoryParametersMenu(){}