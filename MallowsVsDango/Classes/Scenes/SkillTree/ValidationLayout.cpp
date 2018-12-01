#include "ValidationLayout.h"

ValidationLayout * ValidationLayout::create(GUISettings* settings, const ccWidgetTouchCallback& validationCallback)
{
	ValidationLayout* validationLayout = new ValidationLayout();
	validationLayout->init(settings, validationCallback);
	
	return validationLayout;
}

void ValidationLayout::init(GUISettings* settings, const ccWidgetTouchCallback& validationCallback) {
	this->settings = settings;
	addBlackMask();
	addPanel();
	addCloseButton();
	addConfirmationText();
	addValidationButton(validationCallback);
}

void ValidationLayout::addPanel() {
	panel = cocos2d::ui::Button::create("res/buttons/centralMenuPanel2.png");
	panel->setZoomScale(0);
	addChild(panel, 1, "panel");
	panel->setScale9Enabled(true);
	panel->setScale(0.4*settings->getVisibleSize().width / panel->getContentSize().width);
	setPosition(cocos2d::Vec2(settings->getVisibleSize().width / 2, settings->getVisibleSize().height +
		panel->getContentSize().height * panel->getScaleY()));
}

void ValidationLayout::addCloseButton() {
	auto close = cocos2d::ui::Button::create("res/buttons/close2.png");
	close->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			hide();
		}
	});
	close->setScale(panel->getContentSize().width*panel->getScaleX() / 8 / close->getContentSize().width);
	close->setPosition(cocos2d::Vec2(panel->getContentSize().width*panel->getScaleX() / 2,
		panel->getContentSize().height*panel->getScaleY() / 2));
	cocos2d::Sprite* close_shadow = cocos2d::Sprite::create("res/buttons/close2_shadow.png");
	close_shadow->setScale(close->getScale() * 1.05);
	close_shadow->setPosition(close->getPosition());
	addChild(close_shadow, -1);
	addChild(close, 5, "close");
}

void ValidationLayout::addConfirmationText() {
	cocos2d::Label* confirm_text = cocos2d::Label::createWithTTF(settings->getButton("confirm"),
		"fonts/LICABOLD.ttf", round(settings->getVisibleSize().width / 30.f));
	confirm_text->setColor(cocos2d::Color3B::YELLOW);
	confirm_text->enableOutline(cocos2d::Color4B::BLACK, 2);
	confirm_text->setPosition(cocos2d::Vec2(0, panel->getContentSize().height * panel->getScaleY() / 3 -
		settings->getVisibleSize().height / 25));
	confirm_text->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
	addChild(confirm_text, 1, "confirm_text");
}

void ValidationLayout::addValidationButton(const ccWidgetTouchCallback& validationCallback) {
	auto validate_button = cocos2d::ui::Button::create("res/buttons/yellow_button.png");
	validate_button->setTitleText(settings->getButton("validate"));
	validate_button->setTitleFontName("fonts/LICABOLD.ttf");
	validate_button->setTitleFontSize(60.f);
	cocos2d::Label* validate_label = validate_button->getTitleRenderer();
	validate_label->setColor(cocos2d::Color3B::WHITE);
	validate_label->enableOutline(cocos2d::Color4B::BLACK, 2);
	validate_label->setPosition(validate_button->getContentSize() / 2);
	validate_button->setScale(panel->getContentSize().width*panel->getScaleX() / 2 / validate_button->getContentSize().width);
	validate_button->setPosition(cocos2d::Vec2(0,
		-panel->getContentSize().height*panel->getScaleY() / 2));
	validate_button->addTouchEventListener(validationCallback);

	cocos2d::Sprite* validate_shadow = cocos2d::Sprite::create("res/buttons/shadow_button.png");
	validate_shadow->setScale(validate_button->getScale());
	validate_shadow->setPosition(validate_button->getPosition());
	addChild(validate_shadow, -1);
	addChild(validate_button, 5, "validate_button");
}

void ValidationLayout::hide() {
	cocos2d::Size visibleSize = settings->getVisibleSize();
	auto* hideAction = cocos2d::EaseBackIn::create(cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(visibleSize.width / 2, visibleSize.height +
		panel->getContentSize().width *
		panel->getScaleY() * 0.6)));
	runAction(hideAction);
	mask->setVisible(false);
}

void ValidationLayout::show(Json::Value currentTalent) {
	cocos2d::Size visibleSize = settings->getVisibleSize();
	mask->setVisible(true);

	if (validation_text != nullptr) {
		removeChild(validation_text);
	}

	validation_text = cocos2d::ui::RichText::create();
	validation_text->setContentSize(cocos2d::Size(panel->getContentSize().width * panel->getScaleX() * 0.85,
		panel->getContentSize().height * panel->getScaleY() * 2 / 3));
	validation_text->setPosition(cocos2d::Vec2(0, -validation_text->getContentSize().height / 2));
	validation_text->ignoreContentAdaptWithSize(false);
	addChild(validation_text, 1, "validation_text");
	int font_size = round(visibleSize.width / 35.f);
	validation_text->pushBackElement(cocos2d::ui::RichElementText::create(1, cocos2d::Color3B::BLACK, 255, settings->getButton("validate_part_1"),
		"fonts/LICABOLD.ttf", font_size));
	validation_text->pushBackElement(cocos2d::ui::RichElementText::create(2, cocos2d::Color3B::YELLOW, 255, currentTalent["name_" + settings->getLanguage()].asString(),
		"fonts/LICABOLD.ttf", font_size));
	validation_text->pushBackElement(cocos2d::ui::RichElementText::create(3, cocos2d::Color3B::BLACK, 255, settings->getButton("validate_part_2"),
		"fonts/LICABOLD.ttf", font_size));
	validation_text->pushBackElement(cocos2d::ui::RichElementText::create(4, cocos2d::Color3B::YELLOW, 255, currentTalent["cost"].asString(),
		"fonts/LICABOLD.ttf", font_size));
	validation_text->pushBackElement(cocos2d::ui::RichElementText::create(5, cocos2d::Color3B::BLACK, 255, settings->getButton("validate_part_3"),
		"fonts/LICABOLD.ttf", font_size));
	auto* showAction = cocos2d::EaseBackOut::create(cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2)));
	runAction(showAction);
}

void ValidationLayout::addBlackMask()
{
	mask = cocos2d::ui::Button::create("res/buttons/mask.png");
	mask->setScaleX(settings->getVisibleSize().width / mask->getContentSize().width);
	mask->setScaleY(settings->getVisibleSize().height / mask->getContentSize().height);
	mask->setPosition(cocos2d::Vec2(settings->getVisibleSize().width / 2, settings->getVisibleSize().height / 2));
	addChild(mask, -1);
	mask->setVisible(false);
}
