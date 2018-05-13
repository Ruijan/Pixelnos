#pragma once
#include "ParametersMenu.h"

class StoryParametersMenu : public ParametersMenu {
public:
	static StoryParametersMenu* create(MyGame* game);
	virtual ~StoryParametersMenu();
	virtual bool init(MyGame* game);

protected:
	virtual void addRightButton(Json::Value buttons, const std::string& language);
	virtual void addLeftButton(Json::Value buttons, const std::string& language);
};

