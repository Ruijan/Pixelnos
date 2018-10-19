#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"

class SpecialAttackAnnouncement : public cocos2d::ui::Layout {
public:
	static SpecialAttackAnnouncement* create(std::vector<std::string> textToDisplay);
	bool init(std::vector<std::string> textToDisplay);
	void initBackground();
	void initDotsAnimation();
	cocos2d::Sprite* generateRandomDot(std::uniform_real_distribution<> &scaleXDistribution, std::mt19937 &generator, 
		std::uniform_real_distribution<> &horizontalDistribution, std::uniform_real_distribution<> &verticalDistribution, 
		std::vector<float> &positions_y);
	float getRandomYPosition(std::uniform_real_distribution<> & verticalDistribution, std::mt19937 & generator, 
		std::vector<float> & positions_y);
	double getRandomXPosition(std::uniform_real_distribution<> & horizontalDistribution, std::mt19937 & generator, 
		cocos2d::Sprite * dot);
	double getRandomXScale(std::uniform_real_distribution<> & scaleXDistribution, std::mt19937 & generator, cocos2d::Sprite * dot);
	void initDangorillaSprite();
	void initLabelsToDisplay(std::vector<std::string> &textToDisplay);
	void start();
	cocos2d::FiniteTimeAction * getAnnoucementAction();
	void animateWords(int index);
	std::vector<cocos2d::Label*> getTextLabels();

protected:
	cocos2d::ui::Button* background_mask;
	cocos2d::Sprite* background;
	cocos2d::Sprite* dangorille;
	std::vector<std::string> textToDisplay;
	std::vector<cocos2d::Label*> textLabels;
	cocos2d::Size visibleSize;
};