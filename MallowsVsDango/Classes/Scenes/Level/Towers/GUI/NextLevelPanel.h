#pragma once
#include "CurrentLevelPanel.h"

class NextLevelPanel : public CurrentLevelPanel {
public:
	static NextLevelPanel* create(float spriteWidth, Tower* tower, cocos2d::Size size, GUISettings* guiSettings);
	void updateLabel();
	void updateCost(int currentSugar);
	virtual bool init(float spriteWidth, Tower* tower, cocos2d::Size size, GUISettings* guiSettings);

protected:
	virtual void setSpritesPositions(float spriteWidth);

protected:
	cocos2d::Label* costLabel;
	TowerSettings* settings;
};