#include "AppDelegate.h"
#include "audio/include/AudioEngine.h"

//#include "PluginIAP/PluginIAP.h"


USING_NS_CC;
using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocos2d::experimental;

static cocos2d::Size verysmallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size smallResolutionSize = cocos2d::Size(960, 540);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1280, 720);
static cocos2d::Size largeResolutionSize = cocos2d::Size(1920, 1200);
static cocos2d::Size designResolutionSize = cocos2d::Size(1280, 720);
//static cocos2d::Size designResolutionSize = cocos2d::Size(960, 640);


AppDelegate::AppDelegate() : config(new Config("res/config.json", "MvDSave")), controller(new AudioController()) {
}

AppDelegate::~AppDelegate()
{
	TrackingEvent c_event;
	c_event.from_scene = Config::getStringFromSceneType(manager->getCurrentSceneIndex());
	c_event.to_scene = Config::getStringFromSceneType(SceneManager::SceneType::STOP);
	c_event.time = time(0);

	((AppDelegate*)Application::getInstance())->getConfigClass()->addTrackingEvent(c_event);
	AudioEngine::end();
	delete config;
	config = nullptr;
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
	//set OpenGL context attributions,now can only set six attributions:
	//red,green,blue,alpha,depth,stencil
	GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };

	GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages,
// don't modify or remove this function
static int register_all_packages()
{
	return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
	//In-App Purchase
	//sdkbox::IAP::init();

	// initialize director
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		glview = GLViewImpl::createWithRect("Mallows Vs Dangos", Rect(0, 0, 1280, 800));
#else
		glview = GLViewImpl::create("Mallows Vs Dangos");
#endif
		director->setOpenGLView(glview);
	}

	// turn on display FPS
	director->setDisplayStats(false);

	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0f / 60.0f);

	// Set the design resolution
	/*glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
	Size frameSize = glview->getFrameSize();
	// if the frame's height is larger than the height of medium size.
	if(frameSize.height != designResolutionSize.height){
		if (frameSize.height > mediumResolutionSize.height)
		{
			director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
		}
		// if the frame's height is larger than the height of small size.
		else if (frameSize.height > smallResolutionSize.height)
		{
			director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
		}
		// if the frame's height is smaller than the height of medium size.
		else
		{
			director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
		}
	}*/



	auto fileUtils = FileUtils::getInstance();
	config->init();
	controller->initFromConfig();

	register_all_packages();

	// create a scene. it's an autorelease object
	//auto scene = HelloWorld::createScene();
	//director->runWithScene(scene);
	manager = SceneManager::getInstance();
	/*if (COCOS2D_DEBUG == 1) {
		g_screenLog = new ScreenLog();
		g_screenLog->setLevelMask(LL_DEBUG | LL_INFO | LL_WARNING | LL_ERROR | LL_FATAL);
		g_screenLog->setFontFile("UbuntuMono-R.ttf");
		g_screenLog->setTimeoutSeconds(15);
		g_screenLog->attachToScene(manager->getGame());
		g_screenLog->log(LL_DEBUG, "A debug message... ");
		g_screenLog->log(LL_DEBUG, "A debug message...");
		g_screenLog->log(LL_DEBUG, "A debug message...");

	}*/
	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
	Director::getInstance()->stopAnimation();
	AudioEngine::pauseAll();
	TrackingEvent c_event;
	c_event.from_scene = Config::getStringFromSceneType(manager->getCurrentSceneIndex());
	c_event.to_scene = Config::getStringFromSceneType(SceneManager::SceneType::PAUSE);
	c_event.time = time(0);

	((AppDelegate*)Application::getInstance())->getConfigClass()->addTrackingEvent(c_event);
	// if you use SimpleAudioEngine, it must be pause
	// SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
	Director::getInstance()->startAnimation();
	AudioEngine::resumeAll();
	TrackingEvent c_event;
	c_event.from_scene = Config::getStringFromSceneType(SceneManager::SceneType::PAUSE);
	c_event.to_scene = Config::getStringFromSceneType(manager->getCurrentSceneIndex());
	c_event.time = time(0);

	((AppDelegate*)Application::getInstance())->getConfigClass()->addTrackingEvent(c_event);
	// if you use SimpleAudioEngine, it must resume here
	// SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

Json::Value const AppDelegate::getSave() const {
	return config->getSaveValues();
}
Config* AppDelegate::getConfigClass() {
	return config;
}

bool AppDelegate::isSaveFile() const {
	return config->isSaveFile();
}

void AppDelegate::addAudioSlider(AudioSlider* slider, AudioController::SOUNDTYPE type) {
	controller->addSlider(slider, type);
}

void AppDelegate::setVolumeMusic(double volume) {
	controller->setVolumeMusic(volume);
}

AudioController* AppDelegate::getAudioController() {
	return controller;
}

void AppDelegate::switchLanguage() {
	manager->switchLanguage();
}