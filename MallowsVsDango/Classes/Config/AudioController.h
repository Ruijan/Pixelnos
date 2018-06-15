#ifndef AUDIOCONTROLER_HPP
#define AUDIOCONTROLER_HPP

#include <iostream>
#include "cocos2d.h"
#include "Lib/AudioSlider.h"
#include "Config/json.h"
#include "ui/CocosGUI.h"

class AudioController : public cocos2d::Node {
public:
	/**
	* @brief Help to differentiate between music and effect for the sliders.
	*/
	enum SOUNDTYPE {
		MUSIC = 0,
		EFFECT = 1
	};

	AudioController();

	/**
	* @brief Initiate the audio controller sliders from a vector of sliders and
	* their sound type (music of effect).
	* @param Json value obtained by reading a json file. It is usually the save
	* file that can be obtained through AppDelegate::getSave().
	*/
	void init(std::vector<std::pair<AudioSlider*, SOUNDTYPE>> sliders);

	/**
	* @brief Initiate the audio controller parameters from a previous saved
	* configuration file. It will load the maximum volume for music and effects,
	* if the game plays sound and effects and if the main music is in looped mode.
	* @param Json value obtained by reading a json file. It is usually the save
	* file that can be obtained through AppDelegate::getSave().
	*/
	void initFromConfig();

	/**
	* @brief Save the configuration of the sound so next time the player will enter
	* the game, it will load to the same sound volume and parameters.
	* @return If everything is okay, return true
	*/
	bool save();

	/**
	* @brief Play a music from a string file name. It puts automatically the sound
	* to max sound and plays it looped as saved in parameters. It will stop
	* the previous music played.
	* @param Where the file is located. Usually: music/something.mp3
	*/
	void playMusic(std::string filename);

	/**
	* @brief Play a music from a string file name. You can choose to play it with
	* a loop and at a specific volume. It will stop the previous music played.
	* @param Where the file is located. Usually: music/something.mp3
	* @param Play the sound as a loop or not
	* @param At which volume do you want to play it (between 0 and maxMusicVolume
	* that is 1 by default.
	*/
	void playMusic(std::string filename, bool looped, double volume);

	/**
	* @brief Play an effect from a string file name. It puts automatically the sound
	* to max sound and plays it looped as saved in parameters. It can play multiple
	* sounds effects at the same time.
	* @param Where the file is located. Usually: music/something.mp3
	*/
	void playEffect(std::string filename);

	/**
	* @brief Play a music from a string file name. You can choose to play it with
	* a loop and at a specific volume. It can play multiple sounds effects at the same time.
	* @param Where the file is located. Usually: music/something.mp3
	* @param Play the sound as a loop or not
	* @param At which volume do you want to play it (between 0 and maxMusicVolume
	* that is 1 by default.
	*/
	void playEffect(std::string filename, double volume);

	/**
	* @brief Set the volume of the current music to a value between 0 and 1.
	* It cannot exceed maxMusicValue. If above maxMusicValue, it will set it to
	* maxMusicValue.
	* @param Value between 0 and 1.
	*/
	void setVolumeMusic(float volume);

	/**
	* @brief Get the current volume of the music. It can be different from
	* maxMusicVolume
	* @return Value between 0 and 1.
	*/
	double getVolumeMusic();
	/**
	* @brief Set boolean musicEnabled to true or false. This value is saved in
	* json file.
	* @param True the music will play. False the music won't.
	*/
	void enableMusic(bool playmusic);

	/**
	* @brief Set boolean effectsEnabled to true or false. This value is saved in
	* json file. It could help reducing lag since it does not play at all sound
	* effects.
	* @param True the effects will play. False the effects won't.
	*/
	void enableEffects(bool playeffects);
	/**
	* @brief Set boolean loopEnabled to true or false. This value is saved in
	* json file. The main music will play only once in the game, and once in the
	* story menu and so on.
	* @param True the music will play forever. False it will play only once.
	*/
	void enableLoop(bool loop);

	/**
	* @brief Add a slider to the vector of slider. It will add it to the update
	* loop. The slider can be attributed to music or to effects. The slider
	* should be between 0 and 1.
	* @param Slider to add. Usually [min max] = [0.0f 1.0f].
	* @param Define the type of sound the slider will be linked to.
	* Ex: AudioController::SOUNDTYPE::MUSIC.
	*/
	void addSlider(AudioSlider* slider, SOUNDTYPE type);
	/**
	* @brief Add a checkbox to the vector of checkbox. It will add it to the update
	* loop. The checkbox can be attributed to music or to effects. When the checkbox
	* is selected (disabled), it will disable the sound for the sound type.
	* @param CheckBox to add.
	* @param Define the type of sound the checkbox will be linked to.
	* Ex: AudioController::SOUNDTYPE::EFFECTS.
	*/
	void addCheckBox(cocos2d::ui::CheckBox* box, SOUNDTYPE type);
	void addMusicCheckBox(cocos2d::ui::CheckBox* box);
	void addEffectsCheckBox(cocos2d::ui::CheckBox* box);
	/**
	* @brief Add a checkbox to the vector of checkbox. It will add it to the update
	* loop. When the checkbox is selected (disabled), it will disable the loop for music.
	* @param CheckBox to add.
	*/
	void addCheckBoxLoop(cocos2d::ui::CheckBox* box);

	/**
	* @brief Remove the given slider from the vector.
	* @param slider to remove.
	*/
	void removeSlider(AudioSlider* slider);
	/**
	* @brief Remove the given checkbox from the vector.
	* @param checkbox to remove.
	*/
	void removeCheckBox(cocos2d::ui::CheckBox* box);

	/**
	* @brief Update all the sliders value to the max volume from one slider.
	* @param timestep. Unused here but required for the update because of inheritance.
	*/
	virtual void update(float dt);

	/**
	* @brief Get maximum volume allowed for music.
	* @return A volume between 0.0 and 1.0
	*/
	double getMaxMusicVolume();

	/**
	* @brief Get maximum volume allowed for effects.
	* @return A volume between 0.0 and 1.0
	*/
	double getMaxEffectsVolume();

	double getMaxVolume(AudioController::SOUNDTYPE soundType);

	bool isLoopEnabled();


private:
	std::vector<AudioSlider*> musicSliders;
	std::vector<AudioSlider*> effectsSliders;
	std::vector<cocos2d::ui::CheckBox*> musicCheckBoxes;
	std::vector<cocos2d::ui::CheckBox*> effectsCheckBoxes;
	std::vector<cocos2d::ui::CheckBox*> loopCheckBoxes;

	double maxVolumeMusic;
	double maxVolumeEffects;

	int musicID;
	std::string currentMusic;
	std::vector<int> effectsID;

	bool loopEnabled;
	bool musicEnabled;
	bool effectsEnabled;
};

#endif
