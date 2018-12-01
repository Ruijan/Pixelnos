#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "GUI/GUISettings.h"
#include "ValidationLayout.h"

class BuyLayout : public cocos2d::ui::Layout {
public:
	static BuyLayout* create(GUISettings* settings, const ccWidgetTouchCallback& buyButtonCallBack);
	void init(GUISettings * settings, const ccWidgetTouchCallback& buyButtonCallBack);
	void enableBuy();
	void disableBuy();
	void updateCost(int cost);
	void showLockedText();
	void showBuyButton();

protected:
	void addBuyButton(const cocos2d::Size& visibleSize, const ccWidgetTouchCallback& buyButtonCallBack);
	void addSugarSprite(const cocos2d::Size& visibleSize);
	void addSugarCost(const cocos2d::Size & visibleSize);
	void addUnlockedText(GUISettings * settings);

private:
	cocos2d::ui::Button* buy_button;
	cocos2d::Label* skill_cost;
	cocos2d::Sprite* sugar_sprite2;
	cocos2d::Label* unlocked_txt;
};