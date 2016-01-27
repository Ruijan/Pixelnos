#ifndef LOADER_HPP
#define LOADER_HPP

#include "cocos2d.h"

class Loader{
public:
	enum State{
		IDLE,
		TILESET,
		ANIMATION,
		MUSIC,
		EFFECT,
		OBJECT,
		LOADED
	};
	Loader();
	virtual ~Loader();
	virtual void loading(float dt);
	virtual void initAttributes();
	void loadSprite(cocos2d::Texture2D* texture);

	void addAnimation(std::string anim);
	void addMusic(std::string music);
	void addEffect(std::string effect);
	void addTileset(std::string tileset);

	double getProgress();
	bool isLoaded() const;
	void reset();
	void unload();



private:
	State state;
	std::vector<std::string> tilesets;
	std::vector<std::string> animations;
	std::vector<std::string> musics;
	std::vector<std::string> effects;
	unsigned int c_index;
	bool sprite_loaded;
};

#endif
