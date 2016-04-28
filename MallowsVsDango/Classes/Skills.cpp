#include "Skills.h"
#include "AppDelegate.h"
#include "SceneManager.h"

USING_NS_CC;

bool Skills::init(){

	if (!Scene::init()){ return false; }
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Json::Value config = ((AppDelegate*)Application::getInstance())->getConfig()["skills"]; //load data file
	Json::Value root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file

	//generating a background for skills 
	Sprite* loading_background = Sprite::create("res/background/crissXcross.png");
	loading_background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	loading_background->setScale(visibleSize.width / loading_background->getContentSize().width);
	addChild(loading_background);

	//return button to levels on the top right corner
	cocos2d::ui::Button* back = ui::Button::create("res/buttons/back.png");
	addChild(back, 2);
	back->setPosition(Vec2(visibleSize.width - back->getContentSize().width / 2,
		visibleSize.height - back->getContentSize().height / 2));
	back->addTouchEventListener([back](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			SceneManager::getInstance()->setScene(SceneManager::LEVELS);
		}
	});

	//information panel top right edge : sucrettes
	Sprite* sugar_info = Sprite::create("res/buttons/panel_top.png");
	addChild(sugar_info, 2);
	sugar_info->setScaleX(visibleSize.width / sugar_info->getContentSize().width / 5);
	sugar_info->setScaleY(visibleSize.height / sugar_info->getContentSize().height / 15);
	sugar_info->setPosition(Vec2(0.f, visibleSize.height));
	sugar_info->setAnchorPoint(Vec2(0.f, 1.f));

	Label* sugar_amount = Label::createWithTTF("X " + root["holy_sugar"].asString(), "fonts/LICABOLD.ttf", round(visibleSize.width / 40));
	sugar_amount->setColor(Color3B::WHITE);
	sugar_amount->setAnchorPoint(Vec2(0.5, 0.5));
	sugar_amount->setPosition(Vec2(visibleSize.width / 8, visibleSize.height - visibleSize.height / 30));
	addChild(sugar_amount, 2);

	Sprite* sugar_sprite = Sprite::create("res/buttons/sugar.png");
	sugar_sprite->setAnchorPoint(Vec2(0.5, 0.5));
	sugar_sprite->setScale(visibleSize.width / sugar_sprite->getContentSize().width / 28);
//	sugar_sprite->setScaleY(visibleSize.height / sugar_sprite->getContentSize().height * 0.22);
	sugar_sprite->setPosition(Vec2(visibleSize.width / 15, visibleSize.height - visibleSize.height / 30));
	addChild(sugar_sprite, 2);

	//total number of skill tiers
	int tier_size = config.size();
	// Create the scrollview by horizontal
	ui::ScrollView* scrollView = ui::ScrollView::create();
	scrollView->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	scrollView->setContentSize(visibleSize);
	scrollView->setPosition(Vec2(0,0));
	addChild(scrollView, 1, "scrollView");
	scrollView->setInnerContainerSize( Size(tier_size * visibleSize.width / 3 , visibleSize.height));
	
	//add mask to "disable" buttons
	cocos2d::ui::Button* mask = ui::Button::create("res/buttons/mask.png");
	mask->setAnchorPoint(Vec2(0.5, 0.5));
	mask->setScaleX(visibleSize.width / mask->getContentSize().width);
	mask->setScaleY(visibleSize.height / mask->getContentSize().height);
	mask->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(mask, 2, "mask");
	mask->setVisible(false);
	
	mask->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			removeChildByName("buy_info");
			removeChildByName("yes");
			removeChildByName("no");
			getChildByName("mask")->setVisible(false);
		}
	});


	
	//generating skills' button
	for (unsigned int i(0); i < config.size(); ++i) {

		for (unsigned int j(0); j < config[i].size(); j++){

			setSkill(i, j);
			setDependancy(i,j);
		};
	};
	return true;
}

void Skills::setSkill(int tier_id, int skill_id){ //create skill button
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Json::Value config = ((AppDelegate*)Application::getInstance())->getConfig()["skills"];
	
	//create a layout which contains sprites (skill, text)
	cocos2d::ui::Layout* layout = ui::Layout::create();
	getChildByName("scrollView")->addChild(layout, 2, Value(tier_id).asString() + "_" + Value(skill_id).asString());
	layout->setAnchorPoint(Vec2(0.5, 0.5));
	
	//add skill to layout
	Sprite* skill = Sprite::create(config[tier_id][skill_id]["sprite"].asString());
	skill->setAnchorPoint(Vec2(0.5, 0.5));
	skill->setScaleX(visibleSize.width / skill->getContentSize().width * 0.22);
	skill->setScaleY(visibleSize.height / skill->getContentSize().height * 0.22);
	skill->setPosition(Vec2(0.f, 0.f));
	layout->addChild(skill);
	//skill_name
	Label* skill_name = Label::createWithTTF(config[tier_id][skill_id]["name"].asString(), "fonts/LICABOLD.ttf", round(visibleSize.width / 55));
	skill_name->setColor(Color3B::MAGENTA);
	skill_name->setDimensions(visibleSize.width / 6, visibleSize.height / 10);
	skill_name->setAlignment(TextHAlignment::CENTER);
	skill_name->setAnchorPoint(Vec2(0.5, 0.5));
	skill_name->setPosition(Vec2(0.f, visibleSize.height * 0.04));
	layout->addChild(skill_name);

	//skill_description
	Label* skill_description = Label::createWithTTF(config[tier_id][skill_id]["description"].asString(), "fonts/LICABOLD.ttf", round(visibleSize.width / 65));
	skill_description->setColor(Color3B::BLACK);
	skill_description->setDimensions(visibleSize.width / 5, visibleSize.height / 10);
	/*skill_description->setAlignment(TextHAlignment::CENTER);*/
	skill_description->setAnchorPoint(Vec2(0.5, 0.5));
	skill_description->setPosition(Vec2(0.f, - visibleSize.height / 19));
	layout->addChild(skill_description);

	//skill_cost + "Cost" label
	Label* skill_cost = Label::createWithTTF(config[tier_id][skill_id]["cost"].asString(), "fonts/LICABOLD.ttf", round(visibleSize.width / 65));
	skill_cost->setColor(Color3B::BLACK);
	skill_cost->setDimensions(visibleSize.width / 6, visibleSize.height / 20);
	skill_cost->setAlignment(TextHAlignment::RIGHT);
	skill_cost->setPosition(Vec2(0.f, 0.f));
	layout->addChild(skill_cost);

	Label* cost = Label::createWithTTF("Cost : ", "fonts/LICABOLD.ttf", round(visibleSize.width / 65));
	cost->setColor(Color3B::BLACK);
	cost->setDimensions(visibleSize.width / 8, visibleSize.height / 20);
	cost->setAlignment(TextHAlignment::RIGHT);
	cost->setPosition(Vec2(0.f, 0.f));
	layout->addChild(cost);

	//add buy button
	cocos2d::ui::Button* buy = ui::Button::create("res/buttons/buttonBuy.png");
	buy->setAnchorPoint(Vec2(0.5, 0.5));
	buy->setScaleX(visibleSize.width / skill->getContentSize().width * 0.16);
	buy->setScaleY(visibleSize.height / skill->getContentSize().height * 0.16);
	layout->addChild(buy, 2, "buy");
	buy->setPosition(Vec2(0.f, - visibleSize.height * 0.12));
	
	//buy button action + yes/no features
	buy->addTouchEventListener([&, tier_id, skill_id, buy](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Size visibleSize = Director::getInstance()->getVisibleSize();

			//buy confirmation window
			Sprite* buy_info = Sprite::create("res/buttons/checkbox_enemy_filled.png");
			buy_info->setAnchorPoint(Vec2(0.5, 0.5));
			buy_info->setScaleX(visibleSize.width / buy_info->getContentSize().width * 0.20);
			buy_info->setScaleY(visibleSize.height / buy_info->getContentSize().height * 0.16);
			buy_info->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

			cocos2d::ui::Button* yes = ui::Button::create("res/buttons/plus.png");
			yes->setAnchorPoint(Vec2(0.5, 0.5));
			yes->setScale(visibleSize.width / yes->getContentSize().width * 0.10);
			yes->setPosition(Vec2(visibleSize.width / 2 - visibleSize.width / 15, visibleSize.height / 2));

			cocos2d::ui::Button* no = ui::Button::create("res/buttons/minus.png");
			no->setAnchorPoint(Vec2(0.5, 0.5));
			no->setScale(visibleSize.width / no->getContentSize().width * 0.10);
			no->setPosition(Vec2(visibleSize.width / 2 + visibleSize.width / 15, visibleSize.height / 2));

			addChild(buy_info, 2, "buy_info");
			addChild(yes, 2, "yes");
			addChild(no, 2, "no");
			getChildByName("mask")->setVisible(!getChildByName("mask")->isVisible());

			no->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
				if (type == ui::Widget::TouchEventType::ENDED) {
					removeChildByName("buy_info");
					removeChildByName("yes");
					removeChildByName("no");
					getChildByName("mask")->setVisible(false);
				}
			});

			yes->addTouchEventListener([&, tier_id, skill_id, buy](Ref* sender, ui::Widget::TouchEventType type) {
				if (type == ui::Widget::TouchEventType::ENDED) {
					Json::Value root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file
					int skill_bought = root["skill"][tier_id][skill_id].asInt();

					if (skill_bought == 1){
						return;
					}
					if (skill_bought == 0){
						root["skill"][tier_id][skill_id] = 1;
						((AppDelegate*)Application::getInstance())->getConfigClass().setSave(root);
						((AppDelegate*)Application::getInstance())->getConfigClass().save();
						removeChildByName("buy_info");
						removeChildByName("yes");
						removeChildByName("no");
						getChildByName("mask")->setVisible(false);
						getChildByName("scrollView")->getChildByName(Value(tier_id).asString() + "_" + Value(skill_id).asString())->getChildByName("buy")->setVisible(false);
					}
				}
			});


		}
	});
		
	/*
	//buy action with holy sugar
	/*
	buy->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
		
			if ()
		}
	
	((AppDelegate*)Application::getInstance())->getConfigClass().setSave(root);
	((AppDelegate*)Application::getInstance())->getConfigClass().save();
	*/

	//positionning skills tree and buy button
	//total number of skills in tier_id
	int skill_size = config[tier_id].size(); 
	if (tier_id == 0){
		layout->setPosition(Vec2(visibleSize.width / 6, visibleSize.height / 2));

	}
	else{
		if (config[tier_id].size() == 0){
			return;
		}
		if (config[tier_id].size() == 1){
			layout->setPosition(Vec2(visibleSize.width / 6, visibleSize.height / 2));
		}
		if (config[tier_id].size() == 2){
			Vec2 pos_previous_skill = getChildByName("scrollView")->getChildByName(Value(tier_id - 1).asString() + "_0")->getPosition();
			layout->setPosition(Vec2(visibleSize.width / 3 + pos_previous_skill.x, visibleSize.height * (skill_id + 1) / (skill_size + 1)));
		}
		else {
			Vec2 pos_previous_skill = getChildByName("scrollView")->getChildByName(Value(tier_id - 1).asString() + "_0")->getPosition();
			layout->setPosition(Vec2(visibleSize.width / 3 + pos_previous_skill.x, visibleSize.height * (4 * skill_id + 2) / 12));
		}
	}


}

void Skills::setDependancy(int tier_id, int skill_id){ // create link between skills
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Json::Value config = ((AppDelegate*)Application::getInstance())->getConfig()["skills"];

	unsigned int a = config[tier_id][skill_id]["dependance"].size();
	int e = config[tier_id][skill_id]["dependance"][0].asInt();
	if (a == 0) {
		return;
	}
	else {
		for (unsigned int i(0); i < a; i++) {
			int b = config[tier_id][skill_id]["dependance"][i].asInt();
			DrawNode* arrow = DrawNode::create();
			Vec2 pos[4];

			//get scrollView then skill's layout "tier_id_skill_id"
			pos[0] = getChildByName("scrollView")->getChildByName(Value(tier_id).asString() + "_" + Value(skill_id).asString())->getPosition();
			pos[1] = getChildByName("scrollView")->getChildByName(Value(tier_id - 1).asString() + "_" + Value(b).asString())->getPosition();
			pos[2] = getChildByName("scrollView")->getChildByName(Value(tier_id - 1).asString() + "_" + Value(b).asString())->getPosition();
			pos[3] = getChildByName("scrollView")->getChildByName(Value(tier_id).asString() + "_" + Value(skill_id).asString())->getPosition();

			arrow->drawPolygon(pos, 4, Color4F::BLUE, 4, Color4F::BLUE);
			arrow->setAnchorPoint(Vec2(0.5, 0.5));
			getChildByName("scrollView")->addChild(arrow, 1);
		};

	};
		

}