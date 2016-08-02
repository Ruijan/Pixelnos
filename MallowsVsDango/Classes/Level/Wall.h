#ifndef WALL_HPP
#define WALL_HPP

#include "cocos2d.h"

class Wall : public cocos2d::Node {
private:
	unsigned int hp;
	unsigned int max_hp;

public:
	/**
	* @brief Constructor of the class, set the current hp and the maximum hp of the wall.
	* Useful when you want to start with a shitty wall.
	* @param n_hp number or hitpoints the wall can take
	* @param n_max_hp number or hitpoints the wall can take at maximum
	*/
	Wall(unsigned int n_hp, unsigned int n_max_hp);
	/**
	* @brief Destructor. Nothing to be done here.
	*/
	virtual ~Wall();
	/**
	* @brief Create a Wall from the configuration file and the save file. Needs
	* to be improved when we will have the skill tree and real save file.
	* @return A pointer to the wall.
	*/
	static Wall* create();

	/**
	* @brief Create a Wall from the configuration file and the save file. Needs
	* to be improved when we will have the skill tree and real save file.
	* @return A pointer to the wall.
	*/
	static Wall* create(unsigned int n_max_hp);

	/**
	* @brief Decrease the hps and update the display.
	* @param damages amount of damages that will be inflicted.
	*/
	void takeDamages(unsigned int damages);
	/**
	* @return If the HP are at 0, return true. Else return false
	*/
	bool isDestroyed();

	/**
	* @brief Update the image of the wall depending its state. For now only 2 images are available.
	*/
	void updateDisplay();

};

#endif