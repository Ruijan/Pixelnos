#pragma once
#include "CurrentLevelPanel.h"

class NextLevelPanel : public CurrentLevelPanel {
public:
	static NextLevelPanel* create(Config* config, float spriteWidth, Tower* tower, cocos2d::Size size);
	void updateLabel();
	void updateCost(int currentSugar);
	virtual bool init(Config* config, float spriteWidth, Tower* tower, cocos2d::Size size);

protected:
	virtual void setSpritesPositions(float spriteWidth);

protected:
	cocos2d::Label* costLabel;
};