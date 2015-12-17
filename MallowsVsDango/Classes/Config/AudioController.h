#ifndef AUDIOCONTROLER_HPP
#define AUDIOCONTROLER_HPP

#include <iostream>
#include "Lib/AudioSlider.h"
#include "cocos2d.h"

class AudioController: public cocos2d::Node{
public:
	enum SOUNDTYPE{
		MUSIC = 0,
		EFFECT = 1
	};
	AudioController();
	void init(std::vector<std::pair<AudioSlider*, SOUNDTYPE>> sliders);
	void setVolumeMusic(float volume);
	void addSlider(AudioSlider* slider, SOUNDTYPE type);
	void removeSlider(AudioSlider* slider);
	virtual void update(float dt);

private:
	std::vector<AudioSlider*> sliders_music;
	std::vector<AudioSlider*> sliders_effects;
};

#endif
