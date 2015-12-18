#ifndef AUDIOCONTROLER_HPP
#define AUDIOCONTROLER_HPP

#include <iostream>
#include "cocos2d.h"
#include "Lib/AudioSlider.h"
#include "Config/json.h"

class AudioController: public cocos2d::Node{
public:
	enum SOUNDTYPE{
		MUSIC = 0,
		EFFECT = 1
	};
	AudioController();
	void init(std::vector<std::pair<AudioSlider*, SOUNDTYPE>> sliders);
	void initFromConfig(Json::Value config);
	void setVolumeMusic(float volume);
	void addSlider(AudioSlider* slider, SOUNDTYPE type);
	void removeSlider(AudioSlider* slider);
	virtual void update(float dt);
	double getMaxMusicVolume();
	double getMaxEffectsVolume();

private:
	std::vector<AudioSlider*> sliders_music;
	std::vector<AudioSlider*> sliders_effects;
	double maxMusicVolume;
	double maxEffectsVolume;
};

#endif
