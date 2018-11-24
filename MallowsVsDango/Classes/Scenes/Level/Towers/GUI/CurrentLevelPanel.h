#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../../../../Config/Config.h"
#include "../../../../GUI/GUISettings.h"
#include "../Tower.h"

class CurrentLevelPanel : public cocos2d::ui::Layout {
public:
	static CurrentLevelPanel* create(float spriteWidth, Tower* tower, GUISettings* settings);
	virtual void updateLabel();
	const cocos2d::Vec2& getRangePosition();
	const cocos2d::Vec2& getLevelLabelPosition();
	const cocos2d::Size& getLevelLabelSize();
	void setTower(Tower* tower);
	virtual bool init(float spriteWidth, Tower* tower, GUISettings* settings);

protected:
	void addLevelInfo();
	void addAttackInfo(float spriteWidth);
	void addRangeInfo(float spriteWidth);
	void addSpeedInfo(float spriteWidth);
	virtual void setSpritesPositions(float spriteWidth);
	virtual void setLabelsPositions(float spriteWidth);

protected:
	cocos2d::Label* levelLabel;
	cocos2d::Label* attackLabel;
	cocos2d::Label* speedLabel;
	cocos2d::Label* rangeLabel;

	cocos2d::Sprite* attack;
	cocos2d::Sprite* speed;
	cocos2d::Sprite* range;

	GUISettings* guiSettings;
	Tower* tower;
};