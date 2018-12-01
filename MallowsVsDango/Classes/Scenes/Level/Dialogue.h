#ifndef DIALOGUE_H
#define DIALOGUE_H

// PUT THAT SHIT IN THE CONFIGURATION OR SAVE FILE !!!!
// BORDEL....
#define TEXT_SPEED 35

#include "cocos2d.h"
#include "../../Config/json.h"
#include <chrono>
#include <ostream>
#include "../../GUI/GUISettings.h"

typedef std::pair <std::string, std::string> Speech;

/** @class Dialogue
 *  @brief Handle dialogues during a level with caracters, speed of speech,
 *	emotion bubble, left or right apparition.
 */
class Dialogue: public cocos2d::Layer{

	private:
		/** @enum Dialogue::State
		 *  @brief Handling sequence of actions and animations: 
		 *		- Displaying current text
		 *		- Head is currently appearing
		 *		- Speech bubble is currently appearing
		 *		- Head is currently disappearing
		 *		- Speech bubble is currently disappearing
		 */
		enum State{
			DISPLAYING = 0,
			HEAD_APPEAR = 1,
			BUBBLE_APPEAR = 2,
			HEAD_DISAPPEAR = 3,
			BUBBLE_DISAPPEAR = 4
		};
		/** @enum Dialogue::Emotion
		*  @brief Emotion of the character, helps to display a correct speech bubble:
		*		- Normal mode
		*		- Angry mode: speech bubble with spikes + bouncing bubble
		*/
		enum Emotion {
			NORMAL = 0,
			ANGRY = 1
		};
		/** @enum Dialogue::Direction
		*  @brief Direction of the character speaking and appearing:
		*		- Left
		*		- Right
		*/
		enum Direction{
			LEFT = 0,
			RIGHT = 1
		};
		/** @enum Dialogue::TypeReading
		*  @brief Actual type of display:
		*		- Progressive: show caracter by caracter
		*		- All : show all at once
		*/
		enum TypeReading { 
			PROGRESSIVE, 
			ALL 
		};

		// data of dialogues (texts, speakers images, direcitons)
		std::vector<Speech> textes;
		std::vector<Emotion> emotions;
		std::vector<Direction> directions;

		// Display variables
		cocos2d::Node* speechBubble;
		cocos2d::Node* currentSpeechBubble;
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

		GUISettings* settings;

	protected:
		/**
		*  @brief Update emotion bubble when needed (look at the current speech index)
		*/
		void updateEmotionBubble();

	public:
		/**
		* @brief Constructor of the class, set the text, images name of the speakers,
		*	direction of the speaker and its emotions.
		* @param text vector of string with the text for each bubble. Be careful to not overflow the bubble.
		* @param speakers vector of string of images for the head of the speaker
		* @param direction vector of direction, LEFT or RIGHT. It is better to alternate.
		* @param emotion vector of emotion, NORMAL or ANGRY.
		*/
		Dialogue(std::vector<std::string> text, std::vector<std::string>speakers, 
			std::vector<Direction>& direction, std::vector<Emotion>& emotion, GUISettings* settings);
		/**
		* @brief Destructor of the class
		*/
		virtual ~Dialogue();
		/**
		* @brief Initialize a Dialogue from a configuration value and return it.
		* @return Dialogue fully initialize from a configuration value.
		*/
		static Dialogue* createFromConfig(Json::Value config, GUISettings* settings);
		/**
		* @brief Handle what is happening when the user touches the screen:
		*	- when the text is currently displaying, it show eveything at once
		*	- when the text finished to be displayed, start the next text
		*/
		void addEvents();

		/**
		* @return bool When all the caracters spoke and there is no more text to display, return true.
		*/
		bool hasFinished();
		
		/**
		* @brief Create the required images, launch the actions and start the dialogue.
		*/
		void launch();
		void addSpeechBubble(cocos2d::Size &visibleSize);
		void addCurrentHead(cocos2d::Size &visibleSize);
		void addSpeechLabel(cocos2d::Size &visibleSize);
		void addTapToContinueLabel(cocos2d::Size &visibleSize);
		void addSkipButton(cocos2d::Size &visibleSize);
		/**
		* @brief Handle all the state cases over time.
		*/
		void update();
		/**
		* @brief Put to death the dialogue and remove its event listener.
		*/
		void endDialogue();
		/**
		* @brief Depending on the current emotion, scale the speech bubble
		*/
		void scaleSpeechBubble();
};


#endif
