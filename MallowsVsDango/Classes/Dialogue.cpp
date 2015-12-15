#include "Dialogue.h"
#include "Game.h"

USING_NS_CC;
using namespace std::chrono;
using namespace std;

Dialogue::Dialogue(std::vector<string> text, std::vector<std::string>speakers, std::vector<Direction> direction) : running(false), finished(false),
posCurrentCaract(0), currentSpeech(0), type(PROGRESSIVE), speech(nullptr), state(HEAD_APPEAR){

	Size visibleSize = Director::getInstance()->getVisibleSize();
	directions = direction;
	if (text.size() == speakers.size()){
		for (unsigned int i(0); i < text.size(); ++i){
			textes.push_back(Speech(text[i], speakers[i]));
		}
	}
	else{
		log("Sizes of the vectors don't match.");
	}
}
Dialogue::~Dialogue(){
}

Dialogue* Dialogue::createFromConfig(Json::Value config){
	std::vector<std::string> text;
	std::vector<std::string> heads;
	std::vector<Direction> direction;


	for(int i(0); i < config.size(); ++i){
		for(int j(0); j < config[i]["text"].size(); ++j){
			heads.push_back(config[i]["speaker"].asString());
			text.push_back(config[i]["text"][j].asString());
			Direction dir = config[i]["side"].asString() == "LEFT" ? Dialogue::Direction::LEFT : Dialogue::Direction::RIGHT;
			direction.push_back(dir);
		}
	}
	return new Dialogue(text, heads, direction);

}

void Dialogue::launch(){
	start = std::chrono::system_clock::now();

	Size visibleSize = Director::getInstance()->getVisibleSize();
	speechBubble = Sprite::create("res/buttons/speech.png");

	currentHead = Sprite::create(textes[currentSpeech].second);
	currentHead->setPosition(Vec2(-currentHead->getContentSize().width,
		visibleSize.height / 3));
	auto moveto = MoveTo::create(0.35f,
		Vec2(currentHead->getTextureRect().size.width*currentHead->getScale() / 2,
		visibleSize.height / 3));
	cAction = currentHead->runAction(moveto);
	cAction->retain();
	speechBubble->setAnchorPoint(Vec2(0, 0.5));
	speechBubble->setPosition(Point(round(currentHead->getTextureRect().size.width*currentHead->getScale()),
		visibleSize.height / 2 + currentHead->getTextureRect().size.height*currentHead->getScale() / 2));
	speech = Label::createWithTTF("", "fonts/Love Is Complicated Again.ttf", 30.f);
	speech->setColor(Color3B::BLACK);
	speech->setWidth(speechBubble->getContentSize().width*speechBubble->getScale() * 3 / 4);
	speech->setPosition(Point(round(speechBubble->getPosition().x + speechBubble->getContentSize().width*speechBubble->getScale() / 2), speechBubble->getPosition().y + 25));
	speech->setAlignment(TextHAlignment::CENTER);
	tapToContinue = Label::createWithSystemFont("Tap to continue", "Arial", 25.f);
	tapToContinue->setPosition(Point(speech->getPosition().x, speechBubble->getPosition().y - speechBubble->getContentSize().height*speechBubble->getScale() / 4));

	tapToContinue->setColor(Color3B::BLACK);
	tapToContinue->setVisible(false);
	FadeIn* fadeIn = FadeIn::create(0.5f);
	FadeOut* fadeout = FadeOut::create(0.5f);

	tapToContinue->runAction(RepeatForever::create(Sequence::create(fadeIn, fadeout, NULL)));
	speechBubble->setScale(0.01f);
	addChild(currentHead);
	addChild(speechBubble);
	addChild(speech);
	addChild(tapToContinue);
	addEvents();
	running = true;
}

void Dialogue::addEvents(){
	listener = cocos2d::EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = [&](cocos2d::Touch* touch, cocos2d::Event* event) -> bool{
		return true;
	};
	listener->onTouchMoved = [&](cocos2d::Touch* touch, cocos2d::Event* event) -> void{
	};
	listener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event) -> void{
		if (cAction == nullptr || cAction->isDone()){
			if ((unsigned int)posCurrentCaract < textes[currentSpeech].first.length() - 1){

				posCurrentCaract = textes[currentSpeech].first.length();
				std::string text = textes[currentSpeech].first;
				speech->setString(text);
				tapToContinue->setVisible(true);
			}
			else{
				tapToContinue->setVisible(false);
				auto scaleBy = ScaleBy::create(0.125f, 0.01f);
				cAction = speechBubble->runAction(scaleBy);
				cAction->retain();

				state = BUBBLE_DISAPPEAR;
				speech->setString("");
				posCurrentCaract = 0;
				++currentSpeech;
			}
		}

	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 29);
}

bool Dialogue::isRunning(){
	return running;
}
bool Dialogue::hasFinished(){
	return finished;
}
void Dialogue::setDialogueType(TypeReading nType){
	type = nType;
}
void Dialogue::update(){
	if (!finished && running){
		Size visibleSize = Director::getInstance()->getVisibleSize();
		switch (state){
		case DISPLAYING:
			if (type == PROGRESSIVE){
				end = std::chrono::system_clock::now();
				std::chrono::milliseconds oneMillisecond(TEXT_SPEED);
				if (end - start > oneMillisecond){
					if ((unsigned int)posCurrentCaract < textes[currentSpeech].first.length()){
						std::string text = textes[currentSpeech].first;
						text.resize(posCurrentCaract);
						speech->setString(text);
						start = std::chrono::system_clock::now();
						++posCurrentCaract;
					}
					else{
						if (speech->getString() != textes[currentSpeech].first){
							speech->setString(textes[currentSpeech].first);
						}
						tapToContinue->setVisible(true);
					}
				}
			}
			else{
				std::string text = textes[currentSpeech].first;
				speech->setString(text);
				//put code here if it is not progressive
			}
			break;
		case HEAD_APPEAR:
			if (cAction->isDone()){
				state = BUBBLE_APPEAR;
				cAction->release();
				if ((directions[currentSpeech] == RIGHT && currentSpeech == 0)){
					speechBubble->setFlippedX(!speechBubble->isFlippedX());
				}
				else if (currentSpeech > 0 && directions[currentSpeech - 1] != directions[currentSpeech]){
					speechBubble->setFlippedX(!speechBubble->isFlippedX());
				}
				if (directions[currentSpeech] == LEFT){
					speechBubble->setPosition(Point(round(currentHead->getTextureRect().size.width*currentHead->getScale()),
						visibleSize.height / 2 + currentHead->getTextureRect().size.height*currentHead->getScale() / 2));
				}
				else{
					speechBubble->setPosition(Point(round(visibleSize.width * 3 / 4 -
						currentHead->getTextureRect().size.width*currentHead->getScale()) - speechBubble->getContentSize().width,
						visibleSize.height / 2 + currentHead->getTextureRect().size.height*currentHead->getScale() / 2));
				}
				scaleSpeechBubble();
			}
			break;
		case BUBBLE_APPEAR:
			if (cAction->isDone()){
				speech->setPosition(Point(round(speechBubble->getPosition().x + speechBubble->getContentSize().width*speechBubble->getScale() / 2), speechBubble->getPosition().y + 25));
				tapToContinue->setPosition(Point(speech->getPosition().x, speechBubble->getPosition().y - speechBubble->getContentSize().height*speechBubble->getScale() / 4));
				state = DISPLAYING;
			}
			break;
		case HEAD_DISAPPEAR:
			if (cAction->isDone() && currentSpeech < textes.size()){
				cAction->release();
				if (textes[currentSpeech - 1].second != textes[currentSpeech].second ||
					directions[currentSpeech - 1] != directions[currentSpeech]){
					state = HEAD_APPEAR;

					removeChild(currentHead);
					currentHead = nullptr;
					currentHead = Sprite::create(textes[currentSpeech].second);
					Action* moveto;
					if (directions[currentSpeech] == LEFT){
						currentHead->setPosition(Vec2(-currentHead->getContentSize().width,
							visibleSize.height / 3));
						moveto = MoveTo::create(0.35f,
							Vec2(currentHead->getTextureRect().size.width*currentHead->getScale() / 2,
							visibleSize.height / 3));
					}
					else{
						currentHead->setPosition(Vec2(visibleSize.width,
							visibleSize.height / 3));
						moveto = MoveTo::create(0.35f,
							Vec2(visibleSize.width * 3 / 4 - currentHead->getTextureRect().size.width*currentHead->getScale() / 2,
							visibleSize.height / 3));
					}



					cAction = currentHead->runAction(moveto);
					cAction->retain();
					addChild(currentHead);
				}
				else{
					state = BUBBLE_APPEAR;
					scaleSpeechBubble();
				}
			}
			else if (cAction->isDone() && currentSpeech >= textes.size()){
				endDialogue();
			}
			break;
		case BUBBLE_DISAPPEAR:
			if (cAction->isDone()){
				cAction->release();
				if (currentSpeech == textes.size() ||
					textes[currentSpeech - 1].second != textes[currentSpeech].second ||
					directions[currentSpeech - 1] != directions[currentSpeech]){
					state = HEAD_DISAPPEAR;
					Action* moveto;
					if (directions[currentSpeech - 1] == LEFT){
						moveto = MoveTo::create(0.35f,
							Vec2(-currentHead->getContentSize().width - currentHead->getPosition().x,
							currentHead->getPosition().y));
					}
					else{
						moveto = MoveTo::create(0.35f,
							Vec2(currentHead->getContentSize().width + currentHead->getPosition().x,
							currentHead->getPosition().y));
					}
					cAction = currentHead->runAction(moveto);
					cAction->retain();
				}
				else{
					state = BUBBLE_APPEAR;
					scaleSpeechBubble();
				}
			}
			break;
		};
	}
}

void Dialogue::endDialogue(){
	finished = true;
	posCurrentCaract = 0;
	currentSpeech = 0;
	cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
}

void Dialogue::scaleSpeechBubble(){
	auto scaleBy = ScaleBy::create(0.200f, 100.0f);
	cAction = speechBubble->runAction(scaleBy);
	cAction->retain();
}