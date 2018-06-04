#include "CentralMenu.h"

void CentralMenu::addPanel(const cocos2d::Size &visibleSize, double percentageOfVisibleSize)
{
	panel = cocos2d::ui::Button::create("res/buttons/centralMenuPanel2.png");
	panel->setZoomScale(0);
	panel->setScale(percentageOfVisibleSize * visibleSize.width / panel->getContentSize().width);
	addChild(panel, 1, "panel");
	panelSize = cocos2d::Size(panel->getContentSize().width * panel->getScaleX(), panel->getContentSize().height * panel->getScaleY());
}


void CentralMenu::addRightButton(const std::string& buttonLabel)
{
	cocos2d::ui::Button* rightButton = createDefaultButton(buttonLabel, "res/buttons/red_button.png", panel->getContentSize().width*panel->getScale());
	rightButton->addTouchEventListener(CC_CALLBACK_2(CentralMenu::rightButtonCallback, this));
	rightButton->setPosition(cocos2d::Vec2(panel->getContentSize().width*panel->getScale() / 4, -panel->getContentSize().height*panel->getScale() / 2 -
		rightButton->getContentSize().height*rightButton->getScaleY() * 0.41));
	addChild(createButtonShadow(rightButton), -1);
	addChild(rightButton, 1, "rightButton");
}

void CentralMenu::addLeftButton(const std::string& buttonLabel)
{
	cocos2d::ui::Button* leftButton = createDefaultButton(buttonLabel, "res/buttons/yellow_button.png", panel->getContentSize().width*panel->getScale());
	leftButton->addTouchEventListener(CC_CALLBACK_2(CentralMenu::leftButtonCallback, this));
	leftButton->setPosition(cocos2d::Vec2(-panel->getContentSize().width*panel->getScale() / 4, -panel->getContentSize().height*panel->getScale() / 2 -
		leftButton->getContentSize().height*leftButton->getScaleY() * 0.41));
	addChild(createButtonShadow(leftButton), -1);
	addChild(leftButton, 1, "leftButton");
}

void CentralMenu::rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
}

void CentralMenu::leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
}

cocos2d::Sprite* CentralMenu::createButtonShadow(cocos2d::ui::Button* button) {
	cocos2d::Sprite* buttonShadow = cocos2d::Sprite::create("res/buttons/shadow_button.png");
	buttonShadow->setScale(button->getScale());
	buttonShadow->setPosition(button->getPosition());
	return buttonShadow;
}

cocos2d::ui::Button* CentralMenu::createDefaultButton(const std::string& buttonTitle, const std::string& buttonImage, int panelWidth) {
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

cocos2d::TargetedAction* CentralMenu::createHideAction(cocos2d::Node* target) {
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	return cocos2d::TargetedAction::create(target,
		cocos2d::EaseBackIn::create(
			cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(visibleSize.width / 2, visibleSize.height * 1.5))));
}