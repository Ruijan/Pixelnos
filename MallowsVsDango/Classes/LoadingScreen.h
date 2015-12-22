#ifndef LOADINGSCREEN_HPP
#define LOADINGSCREEN_HPP

#include "cocos2d.h"

class LoadingScreen : public cocos2d::LayerGradient
{
public:
	LoadingScreen();
	virtual ~LoadingScreen();
	static LoadingScreen* create();
	bool init();

	void start();
	void stop();

	void setLoadingPercent(double percent);
};

#endif
