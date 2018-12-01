#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "GUI/GUISettings.h"

class ValidationLayout : public cocos2d::ui::Layout {
public:
	static ValidationLayout * create(GUISettings* settings, const ccWidgetTouchCallback& validationCallback);
	void init(GUISettings* settings, const ccWidgetTouchCallback& validationCallback);
	void hide();
	void show(Json::Value currentTalent);

protected:
	void addPanel();
	void addCloseButton();
	void addConfirmationText();
	void addValidationButton(const ccWidgetTouchCallback & validationCallback);
	void addBlackMask();

private:
	cocos2d::ui::RichText* validation_text;
	cocos2d::ui::Button* panel;
	GUISettings* settings;
	cocos2d::ui::Button* mask;
};