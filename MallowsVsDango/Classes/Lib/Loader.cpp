#include "Loader.h"
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;

Loader::Loader() : state(IDLE), c_index(0), sprite_loaded(true) {}

Loader::~Loader() {}

void Loader::loading(float dt) {

	switch (state) {
	case IDLE:
		state = TILESET;
		//log("state : %i, index : %i",(int)state, c_index);
		break;
	case TILESET:
		//log("state : %i, index : %i",(int)state, c_index);
		if (c_index < tilesets.size()) {
			if (sprite_loaded) {
				int pos = tilesets[c_index].find(".plist");
				Director::getInstance()->getTextureCache()->addImageAsync((tilesets[c_index].substr(0, pos)).append(".png"), CC_CALLBACK_1(Loader::loadSprite, this));
			}
			sprite_loaded = false;
		}
		else {
			state = ANIMATION;
			c_index = 0;
		}
		break;
	case ANIMATION:
		if (c_index < animations.size()) {
			//log("state : ANIMATION, index : %i, %s", c_index, animations[c_index].c_str());
			if (sprite_loaded) {
				int pos = animations[c_index].find(".plist");
				Director::getInstance()->getTextureCache()->addImageAsync((animations[c_index].substr(0, pos)).append(".png"), CC_CALLBACK_1(Loader::loadSprite, this));
			}
			sprite_loaded = false;
		}
		else {
			state = MUSIC;
			c_index = 0;
		}
		break;
	case MUSIC:
		if (c_index < musics.size()) {
			//log("state : MUSIC, index : %i, %s", c_index, musics[c_index].c_str());
			//CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(musics[c_index].c_str());
			++c_index;
		}
		else {
			state = EFFECT;
			c_index = 0;
		}
		break;
	case EFFECT:
		if (c_index < effects.size()) {
			//log("state : EFFECT, index : %i, %s", c_index, effects[c_index].c_str());
			//CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(effects[c_index].c_str());
			++c_index;
		}
		else {
			state = OBJECT;
			c_index = 0;
		}
		break;
	case OBJECT:
		//log("state : %i, index : %i",(int)state, c_index);
		initAttributes();
		break;
	}
}

void Loader::addAnimation(std::string anim) {
	animations.push_back(anim);
}

void Loader::addMusic(std::string music) {
	musics.push_back(music);
}
void Loader::addEffect(std::string effect) {
	effects.push_back(effect);
}
void Loader::addTileset(std::string tileset) {
	tilesets.push_back(tileset);
}

bool Loader::isLoaded() const {
	return state == LOADED;
}

double Loader::getProgress() {
	int total = tilesets.size() + musics.size() + effects.size() + animations.size();
	switch (state) {
	case IDLE:
		return 0.0;
	case TILESET:
		return (c_index) / (double)total;
	case ANIMATION:
		return (c_index + tilesets.size()) / (double)total;
	case MUSIC:
		return (c_index + tilesets.size() + animations.size()) / (double)total;
	case EFFECT:
		return (c_index + tilesets.size() + animations.size() + effects.size()) / (double)total > 1.0 ?
			1 : (c_index + tilesets.size() + animations.size() + effects.size()) / (double)total;
	case OBJECT:
		return 1.0;
	case LOADED:
		return 1.0;
	default:
		return 0.0;
	}
}

void Loader::initAttributes() {
	state = LOADED;
}

void Loader::loadSprite(Texture2D* texture) {
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	if (state == TILESET) {
		cache->addSpriteFramesWithFile(tilesets[c_index], texture);
	}
	else if (state == ANIMATION) {
		cache->addSpriteFramesWithFile(animations[c_index], texture);
	}
	++c_index;
	sprite_loaded = true;
}

void Loader::reset() {
	tilesets.clear();
	animations.clear();
	musics.clear();
	effects.clear();

	c_index = 0;
	state = IDLE;
	sprite_loaded = true;
}

void Loader::unload() {
	TextureCache* cache = Director::getInstance()->getTextureCache();
	SpriteFrameCache* Spritecache = SpriteFrameCache::getInstance();
	for (unsigned int i(0); i < tilesets.size(); ++i) {
		int pos = tilesets[i].find(".plist");
		cache->removeTextureForKey((tilesets[i].substr(0, pos)).append(".png"));
		Spritecache->removeSpriteFramesFromFile(tilesets[i]);
	}
	for (unsigned int i(0); i < animations.size(); ++i) {
		int pos = animations[i].find(".plist");
		cache->removeTextureForKey((animations[i].substr(0, pos)).append(".png"));
		Spritecache->removeSpriteFramesFromFile(animations[i]);
	}
}
