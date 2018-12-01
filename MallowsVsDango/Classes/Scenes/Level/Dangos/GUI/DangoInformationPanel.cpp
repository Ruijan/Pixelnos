#include "DangoInformationPanel.h"
#include "../../Cell.h"
#include "../Dango.h" 

DangoInformationPanel * DangoInformationPanel::create(Dango* dango)
{
	
	DangoInformationPanel* informationPanel = new DangoInformationPanel();
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	//informationPanel->setPosition(cocos2d::Vec2(-Cell::getCellWidth() / 2, -Cell::getCellWidth() / 2));
	informationPanel->init(dango);
	informationPanel->update();

	return informationPanel;
}

void DangoInformationPanel::init(Dango* nDango) {
	dango = nDango;
	lifeBar = cocos2d::ui::LoadingBar::create("res/buttons/sliderProgress.png");
	lifeBar->setScaleX(Cell::getCellWidth() / 2 / lifeBar->getContentSize().width);
	lifeBar->setScaleY(Cell::getCellWidth() / 10 / lifeBar->getContentSize().height);
	addChild(lifeBar, 2, "lifeBar");

	lifeBarBackground = cocos2d::Sprite::create("res/buttons/loaderBackground.png");
	lifeBarBackground->setScaleX(Cell::getCellWidth() / 2 / lifeBarBackground->getContentSize().width);
	lifeBarBackground->setScaleY(Cell::getCellWidth() / 10 / lifeBarBackground->getContentSize().height);
	addChild(lifeBarBackground, 1, "lifeBarBackground");
}

void DangoInformationPanel::update() {
	percentLife = (double)dango->getHitPoints() / dango->getSpecConfig()["hitpoints"][dango->getLevel()].asDouble() * 100;
	lifeBar->setPercent(percentLife);
}

void DangoInformationPanel::setDango(Dango * dango)
{
	this->dango = dango;
}
