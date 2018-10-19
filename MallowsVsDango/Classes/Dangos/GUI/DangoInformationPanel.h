#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"

class Dango;

class DangoInformationPanel : public cocos2d::ui::Layout {
public:
	static DangoInformationPanel * DangoInformationPanel::create(Dango* dango);
	void init(Dango * nDango);
	void update();
	void setDango(Dango* dango);

private:
	Dango * dango;
	cocos2d::ui::LoadingBar* lifeBar;
	cocos2d::Sprite* lifeBarBackground;
	double percentLife;
};