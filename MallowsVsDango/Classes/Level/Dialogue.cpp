#include "Dialogue.h"
#include "../Scenes/MyGame.h"
#include "../AppDelegate.h"

USING_NS_CC;
using namespace std::chrono;
using namespace std;

Dialogue::Dialogue(std::vector<string> text, std::vector<std::string>speakers, 
	std::vector<Direction> direction, std::vector<Emotion> emotion) :
running(false), finished(false), posCurrentCaract(0), currentSpeech(0), type(PROGRESSIVE), 
speech(nullptr), state(HEAD_APPEAR), directions(direction), emotions(emotion){

	// associate a text to a speaker
	if (text.size() == speakers.size()){
		for (unsigned int i(0); i < text.size(); ++i){
			textes.push_back(Speech(text[i], speakers[i]));
		}
	}
	else{
		log("Sizes of the vectors don't match for the dialogues.");
	}
}
Dialogue::~Dialogue(){
	log("delete dialogue");
	removeAllChildren();
	if (listener != nullptr) {
		cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
	}
}

Dialogue* Dialogue::createFromConfig(Json::Value config){
	std::vector<std::string> text;
	std::vector<std::string> heads;
	std::vector<Direction> direction;
	std::vector<Emotion> emotions;
	std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();

	for(unsigned int i(0); i < config.size(); ++i){
		for(unsigned int j(0); j < config[i]["text_" + language].size(); ++j){
			heads.push_back(config[i]["speaker"].asString());
			text.push_back(config[i]["text_" + language][j].asString());
			Emotion emotion;
			if(config[i]["emotion"][j].asString() == "NORMAL"){
				emotion = Emotion::NORMAL;
			}
			else if (config[i]["emotion"][j].asString() == "ANGRY") {
				emotion = Emotion::ANGRY;
			}
			emotions.push_back(emotion);
			Direction dir = config[i]["side"].asString() == "LEFT" ? Dialogue::Direction::LEFT : Dialogue::Direction::RIGHT;
			direction.push_back(dir);
		}
	}
	return new Dialogue(text, heads, direction, emotions);
}

void Dialogue::updateEmotionBubble() {
	switch (emotions[currentSpeech]) {
	case NORMAL:
		speechBubble->getChildByName("NORMAL")->setVisible(true);
		speechBubble->getChildByName("ANGRY")->setVisible(false);
		break;
	case ANGRY:
		speechBubble->getChildByName("NORMAL")->setVisible(false);
		speechBubble->getChildByName("ANGRY")->setVisible(true);
		break;
	}
}

void Dialogue::launch(){
	start = std::chrono::system_clock::now();

	Size visibleSize = Director::getInstance()->getVisibleSize();
	std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();

	// The speech bubble is just a node. We add two sprites to this node. It becomes easier to switch
	// from one image to another.
	speechBubble = Node::create();
	speechBubble->addChild(Sprite::create("res/buttons/speech.png"), 1, "NORMAL");
	speechBubble->addChild(Sprite::create("res/buttons/speech_fury.png"), 1, "ANGRY");

	speechBubble->getChildByName("NORMAL")->setScale(visibleSize.width / 3 / speechBubble->getChildByName("NORMAL")->getContentSize().width);
	speechBubble->getChildByName("ANGRY")->setScale(visibleSize.width / 3 / speechBubble->getChildByName("NORMAL")->getContentSize().width);

	double offset = speechBubble->getChildByName("NORMAL")->getContentSize().width * speechBubble->getChildByName("NORMAL")->getScale();

	updateEmotionBubble();

	currentHead = Sprite::create(textes[currentSpeech].second);
	currentHead->setScale(visibleSize.width / 5 / currentHead->getContentSize().width);
	currentHead->setPosition(Vec2(-currentHead->getContentSize().width,
		visibleSize.height / 3));
	auto moveto = MoveTo::create(0.35f,
		Vec2(currentHead->getTextureRect().size.width*currentHead->getScale() / 2,
		visibleSize.height / 3));
	cAction = currentHead->runAction(moveto);
	cAction->retain();
	for (int i(0); i < speechBubble->getChildrenCount(); ++i) {
		speechBubble->getChildren().at(i)->setAnchorPoint(Vec2(0, 0.5));
	}
	
	speechBubble->setPosition(Vec2(round(currentHead->getTextureRect().size.width*currentHead->getScale()),
		visibleSize.height / 2 + currentHead->getTextureRect().size.height*currentHead->getScale() / 2));
	speech = Label::createWithTTF("", "fonts/Love Is Complicated Again.ttf", 30.f * visibleSize.width / 1280);
	speech->setColor(Color3B::BLACK);
	speech->setWidth(offset * 3 / 4);
	speech->setPosition(Point(round(speechBubble->getPosition().x - offset ), speechBubble->getPosition().y + 
		speechBubble->getChildByName("NORMAL")->getContentSize().height * speechBubble->getChildByName("NORMAL")->getScale() / 10));
	speech->setAlignment(TextHAlignment::CENTER);

	auto skip = ui::Button::create("res/buttons/red_button.png");
	skip->setScale(visibleSize.width / 5 / skip->getContentSize().width);
	skip->setTitleText(((AppDelegate*)Application::getInstance())->getConfig()
		["buttons"]["skip_dialogues"][language].asString());
	skip->setTitleFontName("fonts/LICABOLD.ttf");
	skip->setTitleFontSize(45.f * visibleSize.width / 1280);
	Label* skip_label = skip->getTitleRenderer();
	skip_label->enableOutline(Color4B::BLACK, 2);
	skip->setTitleColor(Color3B::WHITE);
	skip->setTitleAlignment(cocos2d::TextHAlignment::CENTER);
	skip->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			endDialogue();
		}
	});
	skip->setPosition(Vec2(visibleSize.width * 3 / 4 - skip->getContentSize().width*skip->getScale(),
		skip->getContentSize().height*skip->getScale()/4));

	tapToContinue = Label::createWithSystemFont("Tap to continue", "Arial", 25.f * visibleSize.width / 1280);
	tapToContinue->setPosition(Point(speech->getPosition().x, speechBubble->getPosition().y - 
		speechBubble->getChildByName("NORMAL")->getContentSize().height*speechBubble->getScale() / 4));

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
	addChild(skip);
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
		// In case the text is currently displaying, we show everything.
		// If it waits to tap, we start to close the bubble.
		if (state == DISPLAYING){
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
	// we set an arbitrary priority
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 29);
}

bool Dialogue::hasFinished(){
	return finished;
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
				updateEmotionBubble();
				if ((directions[currentSpeech] == RIGHT && currentSpeech == 0)){
					for (int i(0); i < speechBubble->getChildrenCount(); ++i) {
						((Sprite*)speechBubble->getChildren().at(i))->setFlippedX(
							!((Sprite*)speechBubble->getChildren().at(i))->isFlippedX());
					}
					
				}
				else if (currentSpeech > 0 && directions[currentSpeech - 1] != directions[currentSpeech]){
					for (int i(0); i < speechBubble->getChildrenCount(); ++i) {
						((Sprite*)speechBubble->getChildren().at(i))->setFlippedX(
							!((Sprite*)speechBubble->getChildren().at(i))->isFlippedX());
					}
				}
				if (directions[currentSpeech] == LEFT){
					speechBubble->setPosition(Point(round(currentHead->getTextureRect().size.width*currentHead->getScale()),
						visibleSize.height / 2 + currentHead->getTextureRect().size.height*currentHead->getScale() / 2));
				}
				else{
					speechBubble->setPosition(Point(round(visibleSize.width * 3 / 4 -
						currentHead->getTextureRect().size.width * currentHead->getScale()) - 
						speechBubble->getChildByName("NORMAL")->getContentSize().width * speechBubble->getChildByName("NORMAL")->getScale(),
						visibleSize.height / 2 + currentHead->getTextureRect().size.height * currentHead->getScale() / 2));
				}
				scaleSpeechBubble();
			}
			break;
		case BUBBLE_APPEAR:
			if (cAction->isDone()){
				speech->setPosition(Point(round(speechBubble->getPosition().x + 
					speechBubble->getChildByName("NORMAL")->getContentSize().width * speechBubble->getChildByName("NORMAL")->getScale() / 2), 
					speechBubble->getPosition().y +	speechBubble->getChildByName("NORMAL")->getContentSize().height * 
					speechBubble->getChildByName("NORMAL")->getScale() / 10));
				tapToContinue->setPosition(Point(speech->getPosition().x, speechBubble->getPosition().y - 
					speechBubble->getChildByName("NORMAL")->getContentSize().height * speechBubble->getChildByName("NORMAL")->getScale() / 4));
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
					currentHead->setScale(visibleSize.width / 5 / currentHead->getContentSize().width);
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
					updateEmotionBubble();
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
					updateEmotionBubble();
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
	Action* scaleBy = nullptr;
	if (emotions[currentSpeech] == NORMAL) {
		scaleBy = ScaleBy::create(0.200f, 100.0f);
	}
	else if (emotions[currentSpeech] == ANGRY) {
		scaleBy = EaseBounceOut::create(ScaleBy::create(0.5f, 100.0f));
	}
	cAction = speechBubble->runAction(scaleBy);
	cAction->retain();
}
