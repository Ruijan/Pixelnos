#ifndef __FADEMUSIC_H__
#define __FADEMUSIC_H__

#include "cocos2d.h"
#include "audio/include/SimpleAudioEngine.h"

class FadeMusic : public cocos2d::ActionInterval{
public:
	static FadeMusic* create(float duration, float volume);
	//
	// Overrides
	//
	virtual FadeMusic* clone() const;
	virtual FadeMusic* reverse(void) const;
	virtual void startWithTarget(cocos2d::Node *target);
	/**
	 * @param time In seconds.
	 */
	virtual void update(float time);

CC_CONSTRUCTOR_ACCESS:
	FadeMusic() {}
	virtual ~FadeMusic() {}

	bool initWithDuration(float duration, float volume);

	protected:
	    float _toVolume;
	    float _fromVolume;
	    friend class FadeOutMusic;
	    friend class FadeInMusic;
	private:
	    CC_DISALLOW_COPY_AND_ASSIGN(FadeMusic);
};

class FadeInMusic : public FadeMusic
{
public:
    /**
     * Creates the action.
     * @param d Duration time, in seconds.
     * @return An autoreleased FadeIn object.
     */
    static FadeInMusic* create(float d);

    //
    // Overrides
    //
    virtual void startWithTarget(cocos2d::Node *target);
    virtual FadeInMusic* clone() const;
    virtual FadeMusic* reverse(void) const;

    /**
     * @js NA
     */
    void setReverseAction(FadeMusic* ac);

CC_CONSTRUCTOR_ACCESS:
	FadeInMusic():_reverseAction(nullptr) {}
    virtual ~FadeInMusic() {}

private:
    CC_DISALLOW_COPY_AND_ASSIGN(FadeInMusic);
    FadeMusic* _reverseAction;
};

/** @class FadeOut
 * @brief Fades Out an object that implements the RGBAProtocol protocol. It modifies the opacity from 255 to 0.
 The "reverse" of this action is FadeIn
*/
class FadeOutMusic : public FadeMusic
{
public:
    /**
     * Creates the action.
     * @param d Duration time, in seconds.
     */
    static FadeOutMusic* create(float d);

    //
    // Overrides
    //
    virtual void startWithTarget(cocos2d::Node *target);
    virtual FadeOutMusic* clone() const;
    virtual FadeMusic* reverse(void) const;

    /**
     * @js NA
     */
    void setReverseAction(FadeMusic* ac);

CC_CONSTRUCTOR_ACCESS:
	FadeOutMusic():_reverseAction(nullptr) {}
    virtual ~FadeOutMusic() {}
private:
    CC_DISALLOW_COPY_AND_ASSIGN(FadeOutMusic);
    FadeMusic* _reverseAction;
};


class ChangeMusic : public cocos2d::ActionInstant
{
public:
    /** Allocates and initializes the action.
     *
     * @return  An autoreleased Show object.
     */
    static ChangeMusic * create(std::string to_music);
    void initWithMusicName(std::string music);
    //
    // Overrides
    //
    /**
     * @param time In seconds.
     */
    virtual void update(float time);
    virtual ActionInstant* reverse() const;
    virtual ChangeMusic* clone() const;

CC_CONSTRUCTOR_ACCESS:
	ChangeMusic(){}
    virtual ~ChangeMusic(){}
protected:
    std::string _toMusic;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(ChangeMusic);
};

#endif
