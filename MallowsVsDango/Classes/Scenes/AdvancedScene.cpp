#include "AdvancedScene.h"

bool AdvancedScene::init(GUISettings * settings)
{
	if (!Scene::init()) { return false; }
	if (settings == nullptr) { return false; }
	this->settings = settings;
	return true;
}

GUISettings * AdvancedScene::getGUISettings()
{
	return settings;
}
