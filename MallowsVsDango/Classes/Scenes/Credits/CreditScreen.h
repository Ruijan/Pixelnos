#ifndef CREDITSCREEN_HPP
#define CREDITSCREEN_HPP

#include "cocos2d.h"
#include "../../Lib/Translationable.h"

class CreditScreen : public cocos2d::Scene, public Translationable
{
public:
	virtual bool init();
	CREATE_FUNC(CreditScreen);
	virtual void switchLanguage();
	virtual void onEnterTransitionDidFinish();
	virtual void onExitTransitionDidStart();
};

#endif
