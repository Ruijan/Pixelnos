#pragma once
#include "ui/CocosGUI.h"
#include "cocos2d.h"


class ChallengeHandler;
class LevelInfo : public cocos2d::ui::Layout {
public:
	static LevelInfo * create(ChallengeHandler* challenges);
	void resetAnimations();
	void reset();
	void update(int sugarQuantity, int lifeQuantity, double progress);
	void showProgress();

protected:
	virtual bool init(ChallengeHandler* challenges);
	cocos2d::Label* sugarLabel;
	cocos2d::Label* lifeLabel;
	cocos2d::ui::LoadingBar* loadingBar;
};