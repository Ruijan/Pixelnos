#ifndef LOADINGSCREEN_HPP
#define LOADINGSCREEN_HPP

#include "cocos2d.h"
#include "../../Lib/Translationable.h"
#include "../../GUI/GUISettings.h"
#include "ui/CocosGUI.h"
#include <spine/spine-cocos2dx.h>

class LoadingScreen : public cocos2d::LayerGradient, public Translationable
{
public:
	LoadingScreen(GUISettings * settings);
	virtual ~LoadingScreen();
	static LoadingScreen* create(GUISettings * settings);
	virtual void switchLanguage();
	bool init();

	void start();
	void stop();

	void setLoadingPercent(double percent);

protected:
	GUISettings * settings;
	spine::SkeletonAnimation* dangobeseSkeleton;
	cocos2d::ui::LoadingBar* loadingBar;
};

#endif
