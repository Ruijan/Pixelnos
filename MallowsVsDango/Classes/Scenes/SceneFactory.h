#pragma once
#include "cocos2d.h"


class SceneFactory {
public:
	enum SceneType
	{
		MENU = 0,
		GAME = 1,
		LEVELS = 2,
		LOADING = 3,
		CREDIT = 4,
		EDITOR = 5,
		SKILLS = 6,
		SHOP = 7,
		PAUSE = 8,
		STOP = 9,
		START = 10
	};

	static cocos2d::Scene * createScene(SceneType type);
	static std::string getStringFromSceneType(SceneFactory::SceneType type);
};