#include "AudioController.h"
#include "ui/CocosGUI.h"
#include "audio/include/AudioEngine.h"
#include "../AppDelegate.h"

USING_NS_CC;
using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocos2d::experimental;

AudioController::AudioController() :maxVolumeMusic(1.0), maxVolumeEffects(1.0),
loopEnabled(true), musicEnabled(true), effectsEnabled(true) {
	Director::getInstance()->getScheduler()->schedule(schedule_selector(
		AudioController::update), this, 0, false);
	// thanks to that we don't try to change the volume of an invalid music.
	musicID = AudioEngine::INVALID_AUDIO_ID;
}

void AudioController::init(std::vector<std::pair<AudioSlider*, SOUNDTYPE>> nsliders) {
	for (auto& c_slider : nsliders) {
		addSlider(c_slider.first, c_slider.second);
	}
}

void AudioController::initFromConfig() {
	auto config = ((AppDelegate*)Application::getInstance())->getConfigClass();
	if (!config->isSaveFile()) {
		Json::Value save_file = config->getSaveValues();
		save_file["sound"]["maxVolumeMusic"] = 0.5f;
		save_file["sound"]["maxVolumeEffects"] = 0.5f;
		save_file["sound"]["loopMusic"] = true;
		save_file["sound"]["playMusic"] = true;
		save_file["sound"]["playEffects"] = true;
		config->setSave(save_file);
		config->save();
	}
	// Set the parameters from json file
	maxVolumeMusic = config->getSaveValues()["sound"]["maxVolumeMusic"].asDouble();
	maxVolumeEffects = config->getSaveValues()["sound"]["maxVolumeEffects"].asDouble();
	loopEnabled = config->getSaveValues()["sound"]["loopMusic"].asBool();
	musicEnabled = config->getSaveValues()["sound"]["playMusic"].asBool();
	effectsEnabled = config->getSaveValues()["sound"]["playEffects"].asBool();
	// Set the volume of the current music if there is a music playing
	if (musicID != AudioEngine::INVALID_AUDIO_ID) {
		AudioEngine::setVolume(musicID, pow(maxVolumeEffects, 2));
	}
	// Set the volume of the current effects if there are effects playing
	for (unsigned int i(0); i < effectsID.size(); ++i) {
		if (effectsID[i] != AudioEngine::INVALID_AUDIO_ID) {
			AudioEngine::setVolume(effectsID[i], maxVolumeEffects);
		}
	}
}

bool AudioController::save() {
	// get the current save (a copy of it) which is a Json value (not a string).
	Json::Value root = ((AppDelegate*)Application::getInstance())->getSave();
	// and modify the parameters
	root["sound"]["maxVolumeMusic"] = maxVolumeMusic;
	root["sound"]["maxVolumeEffects"] = maxVolumeEffects;
	root["sound"]["loopMusic"] = loopEnabled;
	root["sound"]["playMusic"] = musicEnabled;
	root["sound"]["playEffects"] = effectsEnabled;
	// set the current save to the new one.
	((AppDelegate*)Application::getInstance())->getConfigClass()->setSave(root);
	// apply the changes to the file
	((AppDelegate*)Application::getInstance())->getConfigClass()->save();
	((AppDelegate*)Application::getInstance())->getConfigClass()->setSettingsNeedSave(true);
	return true;
}

void AudioController::addSlider(AudioSlider* slider, SOUNDTYPE type) {
	if (type == MUSIC) {
		slider->setValue(maxVolumeMusic);
		musicSliders.push_back(slider);
	}
	else {
		slider->setValue(maxVolumeEffects);
		effectsSliders.push_back(slider);
	}
}

void AudioController::addMusicCheckBox(cocos2d::ui::CheckBox* checkbox) {
	checkbox->setSelected(!musicEnabled);
	musicCheckBoxes.push_back(checkbox);
	checkbox->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		switch (type) {
		case cocos2d::ui::Widget::TouchEventType::ENDED:
			enableMusic(((cocos2d::ui::CheckBox*)sender)->isSelected());
			break;
		}
	});
}

void AudioController::addEffectsCheckBox(cocos2d::ui::CheckBox* checkbox) {
	checkbox->setSelected(!effectsEnabled);
	effectsCheckBoxes.push_back(checkbox);
	checkbox->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		switch (type) {
		case cocos2d::ui::Widget::TouchEventType::ENDED:
			enableEffects(((cocos2d::ui::CheckBox*)sender)->isSelected());
			break;
		}
	});
}

void AudioController::addCheckBox(cocos2d::ui::CheckBox* checkbox, SOUNDTYPE type) {
	if (type == MUSIC) {
		addMusicCheckBox(checkbox);
	}
	else if (type == EFFECT) {
		addEffectsCheckBox(checkbox);
	}
}

void AudioController::addCheckBoxLoop(cocos2d::ui::CheckBox* box) {
	box->setSelected(!loopEnabled);
	loopCheckBoxes.push_back(box);
	box->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		switch (type) {
		case cocos2d::ui::Widget::TouchEventType::ENDED:
			enableLoop(((cocos2d::ui::CheckBox*)sender)->isSelected());
			break;
		}
	});
}

void AudioController::removeSlider(AudioSlider* slider) {
	// once we set it to nullptr we can find it and delete it
	musicSliders.erase(std::remove(musicSliders.begin(), musicSliders.end(), slider),
		musicSliders.end());
	effectsSliders.erase(std::remove(effectsSliders.begin(), effectsSliders.end(), slider),
		effectsSliders.end());
}

void AudioController::removeCheckBox(cocos2d::ui::CheckBox* box) {
	// once we set it to nullptr we can find it and delete it
	musicCheckBoxes.erase(std::remove(musicCheckBoxes.begin(), musicCheckBoxes.end(), box),
		musicCheckBoxes.end());
	effectsCheckBoxes.erase(std::remove(effectsCheckBoxes.begin(), effectsCheckBoxes.end(), box),
		effectsCheckBoxes.end());
	loopCheckBoxes.erase(std::remove(loopCheckBoxes.begin(), loopCheckBoxes.end(), box),
		loopCheckBoxes.end());
}

void AudioController::setVolumeMusic(float volume) {
	if (musicID != AudioEngine::INVALID_AUDIO_ID) {
		// set the volume to volume if volume is inferior to max_music_volume ; else set it to
		// max_music_volume
		AudioEngine::setVolume(musicID, volume < maxVolumeMusic ? volume : maxVolumeMusic);
	}
}
double AudioController::getVolumeMusic() {
	return AudioEngine::getVolume(musicID);
}

void AudioController::enableMusic(bool playmusic) {
	musicEnabled = playmusic;
	// Change all the checkbox selection to the new value
	for (auto* checkbox : musicCheckBoxes) {
		checkbox->setSelected(musicEnabled);
	}
	// Stop the music if the checkbox says to not play music else
	// play the current music that should be played.
	if (!musicEnabled) {
		if (musicID != AudioEngine::INVALID_AUDIO_ID) {
			AudioEngine::stop(musicID);
		}
	}
	else {
		playMusic(currentMusic);
	}
	// Save the parameters into the save file.
	save();
}
void AudioController::enableEffects(bool playeffects) {
	effectsEnabled = playeffects;
	// Change all the checkbox selection to the new value
	for (auto* checkbox : effectsCheckBoxes) {
		checkbox->setSelected(effectsEnabled);
	}
	// Stop the effects if the checkbox says to not play effects. Delete all the
	// current sound effects.
	if (!effectsEnabled) {
		for (unsigned int i(0); i < effectsID.size(); ++i) {
			if (effectsID[i] != AudioEngine::INVALID_AUDIO_ID) {
				AudioEngine::stop(effectsID[i]);
			}
		}
		effectsID.empty();
	}
	// Save the parameters into the save file.
	save();
}

void AudioController::enableLoop(bool loop) {
	loopEnabled = loop;
	if (musicID != AudioEngine::INVALID_AUDIO_ID) {
		AudioEngine::setLoop(musicID, loopEnabled);
	}
	// Change all the checkbox selection to the new value
	for (auto& box : loopCheckBoxes) {
		box->setSelected(loopEnabled);
	}
	save();
}

void AudioController::update(float dt) {
	// update the music volume if the slider position is not at the same level as
	// the maximum value. It also update the position of all the other sliders.
	for (auto* slider : musicSliders) {
		if (slider == nullptr) {
			log("null slider");
		}
		if (slider->getValue() != maxVolumeMusic) {
			if (musicID != AudioEngine::INVALID_AUDIO_ID) {
				float n_volume = exp(slider->getValue()) / exp(1);
				AudioEngine::setVolume(musicID, pow(slider->getValue(), 2));
			}
			maxVolumeMusic = slider->getValue();
			save();
			for (auto& c_slider : musicSliders) {
				c_slider->setValue(slider->getValue());
			}
			break;
		}
	}
	for (auto& slider : effectsSliders) {
		if (slider->getValue() != maxVolumeEffects) {
			for (unsigned int i(0); i < effectsID.size(); ++i) {
				if (effectsID[i] != AudioEngine::INVALID_AUDIO_ID) {
					AudioEngine::setVolume(effectsID[i], slider->getValue());
				}
			}
			maxVolumeEffects = slider->getValue();
			save();
			for (auto& c_slider : effectsSliders) {
				c_slider->setValue(slider->getValue());
			}
			break;
		}
	}
	// If an effect is done playing, we remove its ID from the vector. It is more efficient like this.
	effectsID.erase(std::remove(effectsID.begin(), effectsID.end(), AudioEngine::INVALID_AUDIO_ID), effectsID.end());
}

double AudioController::getMaxMusicVolume() {
	return maxVolumeMusic;
}

double AudioController::getMaxEffectsVolume() {
	return maxVolumeEffects;
}

double AudioController::getMaxVolume(AudioController::SOUNDTYPE soundType) {
	if (soundType == MUSIC) {
		return getMaxMusicVolume();
	}
	else if (soundType == EFFECT) {
		return getMaxEffectsVolume();
	}
}

void AudioController::playMusic(std::string filename, bool looped, double volume) {
	// Stop the previous music
	if (musicID != AudioEngine::INVALID_AUDIO_ID) {
		AudioEngine::stop(musicID);
	}
	// play the new one with the new volume if the player enabled the music
	currentMusic = filename;
	if (musicEnabled) {
		musicID = AudioEngine::play2d(filename, looped, volume < maxVolumeMusic ? volume : maxVolumeMusic);
	}
}

void AudioController::playMusic(std::string filename) {
	playMusic(filename, loopEnabled, maxVolumeMusic);
}

void AudioController::playEffect(std::string filename, double volume) {
	if (effectsEnabled) {
		// play the effect with the new volume if the player enabled the sound effects
		int effectID = AudioEngine::play2d(filename, false, volume < maxVolumeEffects ? volume : maxVolumeEffects);
		// when the sound effect finishes to play, we set its ID to invalid. It
		// will be removed at the next update call
		AudioEngine::setFinishCallback(effectID, [&](int id, const std::string& filePath) {
			for (auto& effectid : effectsID) {
				if (effectid == id) {
					effectid = AudioEngine::INVALID_AUDIO_ID;
					return;
				}
			}
		});
	}
}

void AudioController::playEffect(std::string filename) {
	playEffect(filename, maxVolumeMusic);
}

bool AudioController::isLoopEnabled() {
	return loopEnabled;
}