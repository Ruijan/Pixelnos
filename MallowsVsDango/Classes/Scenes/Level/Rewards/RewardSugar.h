#ifndef REWARD_SUGAR_HPP
#define REWARD_SUGAR_HPP

#include "cocos2d.h"
#include "VisualReward.h"

class RewardSugar : public VisualReward {
public:
	/**
	* @brief Constructor of the class
	*/
	RewardSugar();

	/**
	* @brief Destructor. Nothing to be done here.
	*/
	virtual ~RewardSugar();

	/**
	* @brief Create a Wall from the configuration file and the save file. Needs
	* to be improved when we will have the skill tree and real save file.
	* @return A pointer to the wall.
	*/
	static RewardSugar* create(cocos2d::Vec2 pos);

	/**
	* @brief Decrease the hps and update the display.
	* @param damages amount of damages that will be inflicted.
	*/
	virtual void animate();
};

#endif