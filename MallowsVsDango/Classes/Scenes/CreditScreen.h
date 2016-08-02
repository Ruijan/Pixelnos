#ifndef CREDITSCREEN_HPP
#define CREDITSCREEN_HPP

#include "cocos2d.h"

class CreditScreen : public cocos2d::Scene
{
public:
	virtual bool init();
	CREATE_FUNC(CreditScreen);
	virtual void onEnterTransitionDidFinish();
	virtual void onExitTransitionDidStart();
};

#endif
