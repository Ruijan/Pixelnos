#include "LevelParametersMenu.h"
#include "../../../SceneManager.h"

LevelParametersMenu * LevelParametersMenu::create(MyGame * game, Config * config)
{
	LevelParametersMenu* menu = new (std::nothrow) LevelParametersMenu(game, config);
	if (menu && menu->init())
	{
		menu->autorelease();
		return menu;
	}
	CC_SAFE_DELETE(menu);
	return nullptr;
}

LevelParametersMenu::LevelParametersMenu(MyGame * game, Config * config):
	ParametersMenu(config), game(game)
{
}

void LevelParametersMenu::rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		game->updateTracker("left");
	}
	ParametersMenu::rightButtonCallback(pSender, type);
}

void LevelParametersMenu::leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		this->runAction(createHideAction(this));
		blackMask->setVisible(false);
		game->resume();
	}
}

void LevelParametersMenu::handleAlwaysShowGridButton(cocos2d::ui::Widget::TouchEventType type)
{
	ParametersMenu::handleAlwaysShowGridButton(type);
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		if (game != nullptr) {
			game->getLevel()->showGrid(true);
		}
	}
}

void LevelParametersMenu::handleMovingGridButton(cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		config->getSettings()->enableMovingGrid(true);
		if (game != nullptr) {
			game->getLevel()->showGrid(false);
		}
	}
}

void LevelParametersMenu::handleNeverShowGridButton(cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		config->getSettings()->enableNeverGrid(true);
		if (game != nullptr) {
			game->getLevel()->showGrid(false);
		}
	}
}
