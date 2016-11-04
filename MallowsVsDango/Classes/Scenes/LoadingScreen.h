#ifndef LOADINGSCREEN_HPP
#define LOADINGSCREEN_HPP

#include "cocos2d.h"
#include "../Lib/Translationable.h"


class LoadingScreen : public cocos2d::LayerGradient, public Translationable
{
public:
	LoadingScreen();
	virtual ~LoadingScreen();
	static LoadingScreen* create();
	virtual void switchLanguage();
	bool init();

	void start();
	void stop();

	void setLoadingPercent(double percent);
};

#endif
