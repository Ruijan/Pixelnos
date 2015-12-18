#include "AudioController.h"
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;

AudioController::AudioController():maxMusicVolume(1.0),maxEffectsVolume(1.0){
	Director::getInstance()->getScheduler()->schedule(schedule_selector(AudioController::update),this,0,false);
}

void AudioController::init(std::vector<std::pair<AudioSlider*, SOUNDTYPE>> nsliders){
	for(auto& c_slider : nsliders){
		addSlider(c_slider.first, c_slider.second);
	}
}

void initFromConfig(Json::Value config){
//things to code here.
}

void AudioController::addSlider(AudioSlider* slider, SOUNDTYPE type){
	if(type == MUSIC){
		sliders_music.push_back(slider);
	}
	else{
		sliders_effects.push_back(slider);
	}
}

void AudioController::removeSlider(AudioSlider* slider){
	for(auto c_slider : sliders_music){
		if(slider == c_slider){
			c_slider = nullptr;
		}
	}
	sliders_music.erase(std::remove(sliders_music.begin(), sliders_music.end(), nullptr), sliders_music.end());
	for(auto c_slider : sliders_effects){
		if(slider == c_slider){
			c_slider = nullptr;
		}
	}
	sliders_effects.erase(std::remove(sliders_effects.begin(), sliders_effects.end(), nullptr), sliders_effects.end());
}
void AudioController::setVolumeMusic(float volume){
	if(volume <= maxMusicVolume){
		CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(volume);
	}
}

void AudioController::update(float dt){
	for(auto& slider : sliders_music){
		if(slider->getValue() != maxMusicVolume){
			CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(slider->getValue());
			maxMusicVolume = slider->getValue();
			for(auto& c_slider : sliders_music){
				c_slider->setValue(slider->getValue());
			}
			break;
		}
	}
	for(auto& slider : sliders_effects){
		if(slider->getValue() != maxEffectsVolume){
			CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(slider->getValue());
			maxEffectsVolume = slider->getValue();
			for(auto& c_slider : sliders_effects){
				c_slider->setValue(slider->getValue());
			}
			break;
		}
	}
}

double AudioController::getMaxMusicVolume(){
	return maxMusicVolume;
}

double AudioController::getMaxEffectsVolume(){
	return maxEffectsVolume;
}
