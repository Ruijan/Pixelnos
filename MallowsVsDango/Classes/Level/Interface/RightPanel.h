#pragma once
#include "ui/CocosGUI.h"
#include "cocos2d.h"
#include "../../Config/json.h"


class MyGame;
class RightPanel : public cocos2d::ui::Layout {
public:
	static RightPanel* create(MyGame *game, const std::string& language, const Json::Value& buttons);
	void resetAnimations();
	void reset();
	void update();

	std::pair<std::string, cocos2d::Sprite*> getTowerFromPoint(cocos2d::Vec2 location);
	void displayTowerInfos(std::string item_name, const std::string& language);
	cocos2d::Vec2 getAbsoluteMenuTowerPosition(std::string towerName);
	cocos2d::Sprite* getTower(std::string towerName);

protected:
	virtual bool init(MyGame* game, const std::string& language, const Json::Value& buttons);
	void createPanel();
	void createButtons();
	void createTowersLayout();
	void createTowerInfo();


protected:
	MyGame * game;
	std::unordered_map<std::string, std::pair<cocos2d::Sprite*, double>> towers;
	cocos2d::Layer* towerLayout;
	double towerSize;
};