#ifndef VISUAL_REWARD_HPP
#define VISUAL_REWARD_HPP

#include "cocos2d.h"

class VisualReward : public cocos2d::Node {
public:
	/**
	* @brief Constructor of the class
	*/
	VisualReward();

	/**
	* @brief Destructor. Nothing to be done here.
	*/
	virtual ~VisualReward();

	/**
	* @brief Animate the reward
	*/
	virtual void animate() = 0;
};

#endif