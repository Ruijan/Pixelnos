#pragma once
#include "ui/CocosGUI.h"
#include "cocos2d.h"

class CentralMenu : public cocos2d::ui::Layout {
protected:
	virtual void addPanel(const cocos2d::Size & visibleSize, double percentageOfVisibleSize);
	virtual void addRightButton(const std::string& buttonLabel);
	virtual void addLeftButton(const std::string& buttonLabel);

	virtual void rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	virtual void leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	cocos2d::Sprite * createButtonShadow(cocos2d::ui::Button * button);
	cocos2d::ui::Button * createDefaultButton(const std::string & buttonTitle, const std::string & buttonImage, int panelWidth);
	cocos2d::TargetedAction * createHideAction(cocos2d::Node * target);

protected:
	cocos2d::ui::Button* panel;
	cocos2d::Size panelSize;
};