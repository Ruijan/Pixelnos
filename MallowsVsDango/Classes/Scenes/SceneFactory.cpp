#include "SceneFactory.h"
#include "MainMenu.h"
#include "StoryMenu.h"
#include "CreditScreen.h"
#include "LevelEditor.h"
#include "Skills.h"
#include "MyGame.h"
#include "Shop.h"
#include "../Config/Config.h"

cocos2d::Scene * SceneFactory::createScene(SceneType type, Config* config)
{
	cocos2d::Scene * scene(nullptr);
	switch (type) {
	case MENU:
		scene = MainMenu::create(config);
		break;
	case GAME:
		scene = MyGame::create(config);
		break;
	case LEVELS:
		scene = StoryMenu::create(config);
		break;
	case CREDIT:
		scene = CreditScreen::create();
		break;
	case EDITOR:
		scene = LevelEditor::create();
		break;
	case SKILLS:
		scene = Skills::create(config);
		break;
	case SHOP:
		scene = Shop::create();
		break;
	}
	return scene;
}

std::string SceneFactory::getStringFromSceneType(SceneFactory::SceneType type) {
	switch (type) {
	case SceneFactory::SceneType::CREDIT:
		return "credit";
	case SceneFactory::SceneType::LEVELS:
		return "levels";
	case SceneFactory::SceneType::MENU:
		return "menu";
	case SceneFactory::SceneType::GAME:
		return "game";
	case SceneFactory::SceneType::SKILLS:
		return "skills";
	case SceneFactory::SceneType::SHOP:
		return "shop";
	case SceneFactory::SceneType::EDITOR:
		return "editor";
	case SceneFactory::SceneType::LOADING:
		return "loading";
	case SceneFactory::SceneType::PAUSE:
		return "pause";
	case SceneFactory::SceneType::STOP:
		return "stop";
	case SceneFactory::SceneType::START:
		return "start";
	default:
		return "error";
	}
}