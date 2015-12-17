#ifndef AUDIOSLIDER_HPP
#define AUDIOSLIDER_HPP

#include "cocos2d.h"
#include "extensions/GUI/CCControlExtension/CCControlSlider.h"
#include "audio/include/SimpleAudioEngine.h"

class AudioSlider : public cocos2d::Node
{
public:
    enum Direction {
        Vertical,
        Horizontal
    };

    static AudioSlider *create(Direction direction);

    float getValue() const;

    void setValue(float minValue, float maxValue, float value);
    void setValue(float value);

private:
    AudioSlider(Direction direction);
    virtual ~AudioSlider();

    bool init();

    Direction _direction;
    cocos2d::extension::ControlSlider* _slider;
};

#endif
