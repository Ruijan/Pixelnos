#pragma once
#include "cocos2d.h"
#include "GUI/GUISettings.h"

class AdvancedScene: public cocos2d::Scene{
public:
	bool init(GUISettings * settings);
	GUISettings* getGUISettings();
protected:
	GUISettings * settings;
};