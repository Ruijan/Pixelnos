#pragma once
#include "cocos2d.h"
#include "../../../Config/json.h"
#include "ui/CocosGUI.h"
#include "../Towers/Tower.h"

class Monkey : public cocos2d::ui::Layout{
public:
	static Monkey * create(std::vector<Tower*>& towers, std::vector<int>& tower_indices);
	void init(std::vector<Tower*>& towers, std::vector<int>& tower_indices);
	void goAway();
	bool hasToBeRemoved();
	Tower * getFreeTowerToBlock(std::vector<Tower*>& towers, std::vector<int>& tower_indices);
private:
	cocos2d::Sprite * liane;
	cocos2d::ui::Button* monkey;
	cocos2d::Sprite* shadow;
	Tower* targetTower;

	bool toRemove;
};