#include "BuyLayout.h"

BuyLayout * BuyLayout::create(GUISettings* settings, const ccWidgetTouchCallback& buyButtonCallBack)
{
	BuyLayout* buyingLayout = new BuyLayout();
	buyingLayout->setPosition(cocos2d::Vec2(settings->getVisibleSize().width / 8, settings->getVisibleSize().height * 0.1));
	buyingLayout->init(settings, buyButtonCallBack);
	return buyingLayout;
}

void BuyLayout::init(GUISettings* settings, const ccWidgetTouchCallback& buyButtonCallBack) {
	cocos2d::Size visibleSize = settings->getVisibleSize();
	addBuyButton(visibleSize, buyButtonCallBack);
	addSugarSprite(visibleSize);
	addSugarCost(visibleSize);
	addUnlockedText(settings);
}

void BuyLayout::addBuyButton(const cocos2d::Size& visibleSize, const ccWidgetTouchCallback& buyButtonCallBack) {
	buy_button = cocos2d::ui::Button::create("res/buttons/shop.png");
	buy_button->setScale(visibleSize.width / buy_button->getContentSize().width / 8);
	buy_button->addTouchEventListener(buyButtonCallBack);
	addChild(buy_button, 1, "buy_button");
	buy_button->setVisible(false);
}

void BuyLayout::addSugarSprite(const cocos2d::Size& visibleSize) {
	sugar_sprite2 = cocos2d::Sprite::create("res/buttons/holy_sugar.png");
	sugar_sprite2->setScale(visibleSize.width / sugar_sprite2->getContentSize().width / 20);
	sugar_sprite2->setPosition(cocos2d::Vec2(-buy_button->getContentSize().width * buy_button->getScaleX() / 5, 0));
	addChild(sugar_sprite2, 1, "sugar_sprite2");
	sugar_sprite2->setVisible(false);
}

void BuyLayout::addSugarCost(const cocos2d::Size& visibleSize) {
	skill_cost = cocos2d::Label::createWithTTF("5", "fonts/LICABOLD.ttf", round(visibleSize.width / 30));
	skill_cost->setColor(cocos2d::Color3B::WHITE);
	skill_cost->enableOutline(cocos2d::Color4B::BLACK, 1);
	skill_cost->setPosition(cocos2d::Vec2(buy_button->getContentSize().width * buy_button->getScaleX() / 5, 0));
	addChild(skill_cost, 1, "skill_cost");
	skill_cost->setVisible(false);
}

void BuyLayout::addUnlockedText(GUISettings* settings) {
	std::string txt = settings->getButton("skill_bought");
	txt[0] = toupper(txt[0]);
	unlocked_txt = cocos2d::Label::createWithTTF(txt, "fonts/LICABOLD.ttf", round(settings->getVisibleSize().width / 25));
	unlocked_txt->setColor(cocos2d::Color3B::YELLOW);
	unlocked_txt->enableOutline(cocos2d::Color4B::BLACK, 1);
	addChild(unlocked_txt, 1, "unlocked_txt");
	unlocked_txt->setVisible(false);
}

void BuyLayout::enableBuy()
{
	skill_cost->setColor(cocos2d::Color3B::WHITE);
	buy_button->setEnabled(true);
}

void BuyLayout::disableBuy()
{
	skill_cost->setColor(cocos2d::Color3B::RED);
	buy_button->setEnabled(false);
}

void BuyLayout::updateCost(int cost)
{
	std::string costText = Json::Value(cost).asString();
	skill_cost->setString(costText);
}

void BuyLayout::showLockedText()
{
	skill_cost->setVisible(false);
	sugar_sprite2->setVisible(false);
	buy_button->setVisible(false);
	unlocked_txt->setVisible(true);
}

void BuyLayout::showBuyButton()
{
	skill_cost->setVisible(true);
	sugar_sprite2->setVisible(true);
	buy_button->setVisible(true);
	unlocked_txt->setVisible(false);
}
