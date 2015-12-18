#include "FadeMusic.h"
#include "../Config/AudioController.h"
#include "../AppDelegate.h"
//
// FadeInMusic
//
USING_NS_CC;

FadeInMusic* FadeInMusic::create(float d)
{
    FadeInMusic* action = new (std::nothrow) FadeInMusic();

    action->initWithDuration(d,((AppDelegate*)Application::getInstance())->getAudioController()->getMaxMusicVolume());
    action->autorelease();

    return action;
}

FadeInMusic* FadeInMusic::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) FadeInMusic();
    a->initWithDuration(_duration,((AppDelegate*)Application::getInstance())->getAudioController()->getMaxMusicVolume());
    a->autorelease();
    return a;
}

void FadeInMusic::setReverseAction(FadeMusic *ac)
{
    _reverseAction = ac;
}


FadeMusic* FadeInMusic::reverse() const
{
    auto action = FadeOutMusic::create(_duration);
    action->setReverseAction(const_cast<FadeInMusic*>(this));
    return action;

}

void FadeInMusic::startWithTarget(cocos2d::Node* target)
{
    ActionInterval::startWithTarget(target);

    if (nullptr != _reverseAction) {
        this->_toVolume = this->_reverseAction->_fromVolume;
    }else{
        _toVolume = ((AppDelegate*)Application::getInstance())->getAudioController()->getMaxMusicVolume();
    }

    if (target) {
        _fromVolume = 0.0f;
    }
}



//
// FadeOutMusic
//

FadeOutMusic* FadeOutMusic::create(float d)
{
    FadeOutMusic* action = new (std::nothrow) FadeOutMusic();

    action->initWithDuration(d,0.0f);
    action->autorelease();

    return action;
}

FadeOutMusic* FadeOutMusic::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) FadeOutMusic();
    a->initWithDuration(_duration,0.0f);
    a->autorelease();
    return a;
}

void FadeOutMusic::startWithTarget(cocos2d::Node* target)
{
    ActionInterval::startWithTarget(target);

    if (nullptr != _reverseAction) {
    	_toVolume = _reverseAction->_fromVolume;
    }else{
    	_toVolume = 0.0f;
    }

    if (target) {
    	_fromVolume = ((AppDelegate*)Application::getInstance())->getAudioController()->getMaxMusicVolume();
    }
}

void FadeOutMusic::setReverseAction(FadeMusic *ac)
{
    _reverseAction = ac;
}

FadeMusic* FadeOutMusic::reverse() const
{
    auto action = FadeInMusic::create(_duration);
    action->setReverseAction(const_cast<FadeOutMusic*>(this));
    return action;
}

//
// FadeMusic
//
FadeMusic* FadeMusic::create(float duration, float volume)
{
    FadeMusic *fademusic = new (std::nothrow) FadeMusic();
    fademusic->initWithDuration(duration, volume);
    fademusic->autorelease();

    return fademusic;
}

bool FadeMusic::initWithDuration(float duration, float volume)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _toVolume = volume;
        return true;
    }

    return false;
}

FadeMusic* FadeMusic::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) FadeMusic();
    a->initWithDuration(_duration, _toVolume);
    a->autorelease();
    return a;
}

FadeMusic* FadeMusic::reverse() const
{
    CCASSERT(false, "reverse() not supported in FadeMusic");
    return nullptr;
}

void FadeMusic::startWithTarget(cocos2d::Node* target)
{
    ActionInterval::startWithTarget(target);

    if (target)
    {
        _fromVolume = CocosDenshion::SimpleAudioEngine::getInstance()->getBackgroundMusicVolume();
    }
    /*_fromOpacity = target->getOpacity();*/
}

void FadeMusic::update(float time)
{
    if (_target)
    {
    	((AppDelegate*)Application::getInstance())->setVolumeMusic((float)(_fromVolume + (_toVolume - _fromVolume) * time));
    }
    /*_target->setOpacity((GLubyte)(_fromOpacity + (_toOpacity - _fromOpacity) * time));*/
}

//
// Change Music
//
ChangeMusic* ChangeMusic::create(std::string to_music)
{
	ChangeMusic* ret = new (std::nothrow) ChangeMusic();

    if (ret) {
        ret->autorelease();
        ret->initWithMusicName(to_music);
    }

    return ret;
}

void ChangeMusic::update(float time) {
    CC_UNUSED_PARAM(time);
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(_toMusic.c_str(), true);
}

cocos2d::ActionInstant* ChangeMusic::reverse() const
{
    return ChangeMusic::create(_toMusic);
}

ChangeMusic * ChangeMusic::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) ChangeMusic();
    a->autorelease();
    return a;
}

void ChangeMusic::initWithMusicName(std::string music){
	_toMusic = music;
}
