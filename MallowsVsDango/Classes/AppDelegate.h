#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_
#define COCOS2D_DEBUG 1

#include "cocos2d.h"
#include "SceneManager.h"
#include "Config/Config.h"
#include "Config/AudioController.h"
#include "Lib/AudioSlider.h"
#include "Config/json.h"

/**
@brief    The cocos2d Application.

The reason for implement as private inheritance is to hide some interface call by Director.
*/
class  AppDelegate : private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    virtual void initGLContextAttrs();

    /**
    @brief    Implement Director and Scene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  The function be called when the application enter background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  The function be called when the application enter foreground
    @param  the pointer of the application
    */
    virtual void applicationWillEnterForeground();

	Json::Value const getConfig() const;
	Json::Value const getSave() const;
	Config* getConfigClass();
	bool isSaveFile() const;

	AudioController* getAudioController();
	void addAudioSlider(AudioSlider* slider, AudioController::SOUNDTYPE type);
	void setVolumeMusic(double volume);

	/*static AppDelegate* getInstance(){
		if (!appDelegate)
			appDelegate = new AppDelegate();
		return appDelegate;
	}*/

protected:
	static AppDelegate* appDelegate;
	SceneManager* manager;
	Config* config;
	AudioController* controller;

};

#endif // _APP_DELEGATE_H_

