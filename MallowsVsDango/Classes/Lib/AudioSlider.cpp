#include "AudioSlider.h"

USING_NS_CC;

AudioSlider::AudioSlider(Direction direction): _direction(direction),
_slider(nullptr)
{
}

AudioSlider::~AudioSlider(){}

AudioSlider* AudioSlider::create(Direction direction){
	AudioSlider* ret = new (std::nothrow) AudioSlider(direction);
	if (ret && !ret->init()) {
		delete ret;
		ret = nullptr;
	}
	ret->autorelease();
	return ret;
}

float AudioSlider::getValue() const{
    return _slider->getValue();
}

void AudioSlider::setValue(float minValue, float maxValue, float value){
	_slider->setMinimumValue(minValue);
	_slider->setMaximumValue(maxValue);
	_slider->setValue(value);
}

void AudioSlider::setValue(float value){
	_slider->setValue(value);
}

bool AudioSlider::init(){
	_slider = extension::ControlSlider::create("res/buttons/sliderTrack.png","res/buttons/sliderProgress.png" ,"res/buttons/sliderThumb.png");
	if (_direction == Vertical)
		_slider->setRotation(-90.0);
	addChild(_slider);
	return true;
}
