#include "AudioController.h"
#include "ui/CocosGUI.h"
#include "audio/include/AudioEngine.h"
#include "../AppDelegate.h"

USING_NS_CC;
using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocos2d::experimental;

AudioController::AudioController():max_music_volume(1.0),max_effects_volume(1.0),
loop_enabled(true), music_enabled(true), effects_enabled(true){
	Director::getInstance()->getScheduler()->schedule(schedule_selector(
		AudioController::update),this,0,false);
	// thanks to that we don't try to change the volume of an invalid music.
	music_ID = AudioEngine::INVALID_AUDIO_ID;
}

void AudioController::init(std::vector<std::pair<AudioSlider*, SOUNDTYPE>> nsliders){
	for(auto& c_slider : nsliders){
		addSlider(c_slider.first, c_slider.second);
	}
}

void AudioController::initFromConfig(Json::Value config){
	// Set the parameters from json file
	max_music_volume	= config["sound"]["maxVolumeMusic"].asDouble();
	max_effects_volume	= config["sound"]["maxVolumeEffects"].asDouble();
	loop_enabled		= config["sound"]["loopMusic"].asBool();
	music_enabled		= config["sound"]["playMusic"].asBool();
	effects_enabled		= config["sound"]["playEffects"].asBool();
	// Set the volume of the current music if there is a music playing
	if (music_ID != AudioEngine::INVALID_AUDIO_ID) {
		AudioEngine::setVolume(music_ID, max_music_volume);
	}
	// Set the volume of the current effects if there are effects playing
	for (unsigned int i(0); i < effects_ID.size(); ++i) {
		if (effects_ID[i] != AudioEngine::INVALID_AUDIO_ID) {
			AudioEngine::setVolume(effects_ID[i], max_effects_volume);
		}
	}
}

bool AudioController::save() {
	// get the current save (a copy of it) which is a Json value (not a string).
	Json::Value root = ((AppDelegate*)Application::getInstance())->getSave();
	// and modify the parameters
	root["sound"]["maxVolumeMusic"]		= max_music_volume;
	root["sound"]["maxVolumeEffects"]	= max_effects_volume;
	root["sound"]["loopMusic"]			= loop_enabled;
	root["sound"]["playMusic"]			= music_enabled;
	root["sound"]["playEffects"]		= effects_enabled;
	// set the current save to the new one.
	((AppDelegate*)Application::getInstance())->getConfigClass()->setSave(root);
	// apply the changes to the file
	((AppDelegate*)Application::getInstance())->getConfigClass()->save();
	return true;
}

void AudioController::addSlider(AudioSlider* slider, SOUNDTYPE type){
	if(type == MUSIC){
		slider->setValue(max_music_volume);
		sliders_music.push_back(slider);
	}
	else{
		slider->setValue(max_effects_volume);
		sliders_effects.push_back(slider);
	}
}

void AudioController::addButton(cocos2d::ui::CheckBox* checkbox, SOUNDTYPE type) {
	if (type == MUSIC) {
		checkbox->setSelected(!music_enabled);
		music_buttons.push_back(checkbox);
	}
	else {
		checkbox->setSelected(!effects_enabled);
		effects_buttons.push_back(checkbox);
	}
}

void AudioController::addButtonLoop(cocos2d::ui::CheckBox* box) {
	box->setSelected(loop_enabled);
	loop_buttons.push_back(box);
}

void AudioController::removeSlider(AudioSlider* slider){
	// once we set it to nullptr we can find it and delete it
	sliders_music.erase(std::remove(sliders_music.begin(), sliders_music.end(), slider),
		sliders_music.end());
	sliders_effects.erase(std::remove(sliders_effects.begin(), sliders_effects.end(), slider),
		sliders_effects.end());
}

void AudioController::removeButton(cocos2d::ui::CheckBox* box) {
	// once we set it to nullptr we can find it and delete it
	music_buttons.erase(std::remove(music_buttons.begin(), music_buttons.end(), box),
		music_buttons.end());
	effects_buttons.erase(std::remove(effects_buttons.begin(), effects_buttons.end(), box),
		effects_buttons.end());
	loop_buttons.erase(std::remove(loop_buttons.begin(), loop_buttons.end(), box),
		loop_buttons.end());
}

void AudioController::setVolumeMusic(float volume){
	if(music_ID != AudioEngine::INVALID_AUDIO_ID) {
		// set the volume to volume if volume is inferior to max_music_volume ; else set it to
		// max_music_volume
		AudioEngine::setVolume(music_ID, volume < max_music_volume ? volume : max_music_volume);
	}
}
double AudioController::getVolumeMusic() {
	return AudioEngine::getVolume(music_ID);
}

void AudioController::enableMusic(bool playmusic) {
	music_enabled = playmusic;
	// Change all the checkbox selection to the new value
	for (auto checkbox : music_buttons) {
		checkbox->setSelected(!music_enabled);
	}
	// Stop the music if the checkbox says to not play music else
	// play the current music that should be played.
	if (!music_enabled) {
		if (music_ID != AudioEngine::INVALID_AUDIO_ID) {
			AudioEngine::stop(music_ID);
		}
	}
	else {
		playMusic(current_music);
	}
	// Save the parameters into the save file.
	save();
}
void AudioController::enableEffects(bool playeffects) {
	effects_enabled = playeffects;
	// Change all the checkbox selection to the new value
	for (auto& checkbox : effects_buttons) {
		checkbox->setSelected(!effects_enabled);
	}
	// Stop the effects if the checkbox says to not play effects. Delete all the
	// current sound effects.
	if (!effects_enabled) {
		for (unsigned int i(0); i < effects_ID.size(); ++i) {
			if (effects_ID[i] != AudioEngine::INVALID_AUDIO_ID) {
				AudioEngine::stop(effects_ID[i]);
			}
		}
		effects_ID.empty();
	}
	// Save the parameters into the save file.
	save();
}

void AudioController::enableLoop(bool loop) {
	loop_enabled = loop;
	if (music_ID != AudioEngine::INVALID_AUDIO_ID) {
		AudioEngine::setLoop(music_ID, loop_enabled);
	}
	// Change all the checkbox selection to the new value
	for (auto& box : loop_buttons) {
		box->setSelected(loop_enabled);
	}
	save();
}

void AudioController::update(float dt){
	// update the music volume if the slider position is not at the same level as
	// the maximum value. It also update the position of all the other sliders.
	for(auto& slider : sliders_music){
		if(slider->getValue() != max_music_volume){
			if (music_ID != AudioEngine::INVALID_AUDIO_ID) {
				AudioEngine::setVolume(music_ID, slider->getValue());
			}
			max_music_volume = slider->getValue();
			save();
			for(auto& c_slider : sliders_music){
				c_slider->setValue(slider->getValue());
			}
			break;
		}
	}
	for(auto& slider : sliders_effects){
		if(slider->getValue() != max_effects_volume){
			for (unsigned int i(0); i < effects_ID.size(); ++i) {
				if (effects_ID[i] != AudioEngine::INVALID_AUDIO_ID) {
					AudioEngine::setVolume(effects_ID[i], slider->getValue());
				}
			}
			max_effects_volume = slider->getValue();
			save();
			for(auto& c_slider : sliders_effects){
				c_slider->setValue(slider->getValue());
			}
			break;
		}
	}
	// If an effect is done playing, we remove its ID from the vector. It is more efficient like this.
	effects_ID.erase(std::remove(effects_ID.begin(), effects_ID.end(), AudioEngine::INVALID_AUDIO_ID), effects_ID.end());
}

double AudioController::getMaxMusicVolume(){
	return max_music_volume;
}

double AudioController::getMaxEffectsVolume(){
	return max_effects_volume;
}

void AudioController::playMusic(std::string filename, bool looped, double volume) {
	// Stop the previous music
	if (music_ID != AudioEngine::INVALID_AUDIO_ID) {
		AudioEngine::stop(music_ID);
	}
	// play the new one with the new volume if the player enabled the music
	current_music = filename;
	if (music_enabled) {
		music_ID = AudioEngine::play2d(filename, looped, volume < max_music_volume ? volume : max_music_volume);
	}
}

void AudioController::playMusic(std::string filename) {
	playMusic(filename, loop_enabled, max_music_volume);
}

void AudioController::playEffect(std::string filename, double volume) {
	if (effects_enabled) {
		// play the effect with the new volume if the player enabled the sound effects
		int effectID = AudioEngine::play2d(filename, false, volume < max_effects_volume ? volume : max_effects_volume);
		// when the sound effect finishes to play, we set its ID to invalid. It
		// will be removed at the next update call
		AudioEngine::setFinishCallback(effectID, [&](int id, const std::string& filePath) {
			for (auto& effectid : effects_ID) {
				if (effectid == id) {
					effectid = AudioEngine::INVALID_AUDIO_ID;
					return;
				}
			}
		});
	}
}

void AudioController::playEffect(std::string filename) {
	playEffect(filename, max_music_volume);
}
