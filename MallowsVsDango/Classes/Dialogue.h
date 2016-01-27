#ifndef DIALOGUE_H
#define DIALOGUE_H
#define TEXT_SPEED 35

#include "cocos2d.h"
#include "Config/json.h"
#include <chrono>
#include <ostream>

typedef std::pair <std::string, std::string> Speech;

class Dialogue: public cocos2d::Layer{

	private:
		enum State{
			DISPLAYING = 0,
			HEAD_APPEAR = 1,
			BUBBLE_APPEAR = 2,
			HEAD_DISAPPEAR = 3,
			BUBBLE_DISAPPEAR = 4
		};
		enum Direction{
			LEFT = 0,
			RIGHT = 1
		};
		enum TypeReading { PROGRESSIVE, ALL };

		// data of dialogues (texts, speakers images, direcitons)
		std::vector<Speech> textes;
		std::vector<Direction> directions;

		// Display variables
		cocos2d::Sprite* speechBubble;
		cocos2d::Label* speech;
		cocos2d::Label* tapToContinue;
		cocos2d::Sprite* currentHead;

		// Dialogue parameters
		unsigned int currentSpeech;
		bool running;
		bool finished;
		int posCurrentCaract;
		TypeReading type;
		State state;
		std::chrono::time_point<std::chrono::system_clock> start, end;
		cocos2d::EventListenerTouchOneByOne* listener;
		cocos2d::Action* cAction;

	public:
		Dialogue(std::vector<std::string> text, std::vector<std::string>speakers, std::vector<Direction> direction);
		virtual ~Dialogue();
		static Dialogue* createFromConfig(Json::Value config);
		void addEvents();
		void launch();
		bool isRunning();
		bool hasFinished();
		void update();
		void addSpeech(std::string text, cocos2d::Sprite* = NULL);
		void setDialogueType(TypeReading nType);
		void endDialogue();
		void scaleSpeechBubble();
};


#endif
