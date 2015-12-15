#ifndef __FADEMUSIC_H__
#define __FADEMUSIC_H__

#include "cocos2d.h"
#include "audio/include/SimpleAudioEngine.h"

class CC_DLL FadeMusic : public cocos2d::ActionInterval{
public:
	static FadeMusic* create(float duration, float volume);
	//
	// Overrides
	//
	virtual FadeMusic* clone() const override;
	virtual FadeMusic* reverse(void) const override;
	virtual void startWithTarget(cocos2d::Node *target) override;
	/**
	 * @param time In seconds.
	 */
	virtual void update(float time) override;

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

class CC_DLL FadeInMusic : public FadeMusic
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
    virtual void startWithTarget(cocos2d::Node *target) override;
    virtual FadeInMusic* clone() const override;
    virtual FadeMusic* reverse(void) const override;

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
class CC_DLL FadeOutMusic : public FadeMusic
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
    virtual void startWithTarget(cocos2d::Node *target) override;
    virtual FadeOutMusic* clone() const  override;
    virtual FadeMusic* reverse(void) const override;

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


class CC_DLL ChangeMusic : public cocos2d::ActionInstant
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
    virtual void update(float time) override;
    virtual ActionInstant* reverse() const override;
    virtual ChangeMusic* clone() const override;

CC_CONSTRUCTOR_ACCESS:
	ChangeMusic(){}
    virtual ~ChangeMusic(){}
protected:
    std::string _toMusic;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(ChangeMusic);
};

#endif
