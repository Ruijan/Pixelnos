#ifndef REWARD_TOWER_HPP
#define REWARD_TOWER_HPP

#include "cocos2d.h"
#include "VisualReward.h"
#include "ui/CocosGUI.h"
#include "../../../GUI/GUISettings.h"

USING_NS_CC;

class RewardTower : public VisualReward {
private:
	std::string tower_name;

public:
	/**
	* @brief Constructor of the class, set the current hp and the maximum hp of the wall.
	* Useful when you want to start with a shitty wall.
	* @param n_hp number or hitpoints the wall can take
	* @param n_max_hp number or hitpoints the wall can take at maximum
	*/
	RewardTower(std::string towername);

	/**
	* @brief Destructor. Nothing to be done here.
	*/
	virtual ~RewardTower();

	/**
	* @brief Create a Wall from the configuration file and the save file. Needs
	* to be improved when we will have the skill tree and real save file.
	* @return A pointer to the wall.
	*/
	static RewardTower* create(std::string file, std::string towername, cocos2d::Vec2 pos,
		const cocos2d::ui::Widget::ccWidgetTouchCallback& callback, GUISettings* settings);

	/**
	* @brief Decrease the hps and update the display.
	* @param damages amount of damages that will be inflicted.
	*/
	virtual void animate();
};

#endif