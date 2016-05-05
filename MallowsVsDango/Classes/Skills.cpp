#include "Skills.h"
#include "AppDelegate.h"
#include "SceneManager.h"


USING_NS_CC;

bool Skills::init(){

	if (!Scene::init()){ return false; }
	Size visibleSize = Director::getInstance()->getVisibleSize();
	config = ((AppDelegate*)Application::getInstance())->getConfig()["skills"]; //load data file
	root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file

	if (!root.isMember("holy_sugar")) {
		root["holy_sugar"] = 0;
	}

	Json::Value root2 = root;
	root2["skill"] = {};
	for (int k(0); k < config.size(); ++k) {
		for (int l(0); l < config[k].size(); ++l) {
			auto conf = config[k][l]["id"].asInt();
			root2["skill"][k][l]["id"] = config[k][l]["id"].asInt();
			root2["skill"][k][l]["bought"] = false;
		}
	}

	for (int i(0); i < root["skill"].size(); ++i) {
		for (int j(0); j < root["skill"][i].size(); ++j) {
			if (config[i][j]["id"].asInt() != root["skill"][i][j]["id"].asInt()) {
				for (int k(0); k < config.size(); ++k) {
					for (int l(0); l < config[k].size(); ++l) {
						if (config[k][l]["id"].asInt() == root["skill"][i][j]["id"].asInt()) {
							root2["skill"][k][l]["id"] = config[k][l]["id"].asInt();
							root2["skill"][k][l]["bought"] = root["skill"][i][j]["bought"].asBool();
						}
					}
				}
			}
			else {
				root2["skill"][i][j]["id"] = config[i][j]["id"].asInt();
				root2["skill"][i][j]["bought"] = root["skill"][i][j]["bought"].asBool();
			}
		}
	}
	root = root2;
	((AppDelegate*)Application::getInstance())->getConfigClass()->setSave(root);
	((AppDelegate*)Application::getInstance())->getConfigClass()->save();

	//generating a background for skills 
	Sprite* loading_background = Sprite::create("res/background/crissXcross.png");
	loading_background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	loading_background->setScale(visibleSize.width / loading_background->getContentSize().width);
	addChild(loading_background);

	//return button to levels on the top right corner
	cocos2d::ui::Button* back = ui::Button::create("res/buttons/back.png");
	addChild(back, 2);
	back->setScale(visibleSize.width / back->getContentSize().width / 15);
	back->setAnchorPoint(Vec2(1.f, 1.f));
	back->setPosition(Vec2(visibleSize.width, visibleSize.height));
	back->addTouchEventListener([back](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			SceneManager::getInstance()->setScene(SceneManager::LEVELS);
		}
	});

	//current sugar info
	Label* sugar_amount = Label::createWithTTF("X " + root["holy_sugar"].asString(), "fonts/LICABOLD.ttf", round(visibleSize.width / 35));
	sugar_amount->setColor(Color3B::WHITE);
	sugar_amount->setAnchorPoint(Vec2(0.5, 0.5));
	sugar_amount->setPosition(Vec2(visibleSize.width / 7, visibleSize.height - visibleSize.height / 17));
	addChild(sugar_amount, 3, "sugar_amount");

	Sprite* sugar_sprite1 = Sprite::create("res/buttons/sugar.png");
	sugar_sprite1->setAnchorPoint(Vec2(0.5, 0.5));
	sugar_sprite1->setScale(visibleSize.width / sugar_sprite1->getContentSize().width / 28);
	sugar_sprite1->setPosition(Vec2(visibleSize.width / 12, visibleSize.height - visibleSize.height / 18));
	addChild(sugar_sprite1, 3);

	//shop button
	cocos2d::ui::Button* shop = ui::Button::create("res/buttons/wood_nextlevel_panel.png");
	shop->setAnchorPoint(Vec2(0.5, 0.5));
	shop->setScaleX(visibleSize.width / shop->getContentSize().width * 0.18);
	shop->setScaleY(visibleSize.height / shop->getContentSize().height * 0.1);
	shop->setPosition(Vec2(visibleSize.width / 8, visibleSize.height * 0.85));
	addChild(shop, 3, "shop");
	//add shop_label
	Label* shop_label = Label::createWithTTF("SHOP", "fonts/LICABOLD.ttf", round(visibleSize.width / 35));
	shop_label->setColor(Color3B::RED);
	shop_label->enableOutline(Color4B::BLACK, 2);
	shop_label->setAlignment(TextHAlignment::CENTER);
	shop_label->setAnchorPoint(Vec2(0.5, 0.5));
	shop_label->setPosition(Vec2(shop->getPosition().x, shop->getPosition().y));
	addChild(shop_label, 3, "shop_label");

	//Skill_info, window left side
	Sprite* skill_info = Sprite::create("res/buttons/menupanel4.png");
	addChild(skill_info, 2);
	skill_info->setScaleX(visibleSize.width / skill_info->getContentSize().width / 4);
	skill_info->setScaleY(visibleSize.height / skill_info->getContentSize().height);
	skill_info->setPosition(Vec2(0.f, 0.f));
	skill_info->setAnchorPoint(Vec2(0.f, 0.f));

	//label for setting skill_name
	Label* skill_name = Label::createWithTTF(config[0][0]["name"].asString(), "fonts/LICABOLD.ttf", round(visibleSize.width / 40));
	skill_name->setColor(Color3B::ORANGE);
	skill_name->enableOutline(Color4B::BLACK, 2);
	skill_name->setDimensions(visibleSize.width / 5, visibleSize.height / 5);
	skill_name->setAlignment(TextHAlignment::CENTER);
	skill_name->setAnchorPoint(Vec2(0.5, 0.5));
	skill_name->setPosition(Vec2(visibleSize.width / 8, visibleSize.height * 0.65));
	addChild(skill_name, 3, "skill_name");
	//label for setting skill_description
	Label* skill_description = Label::createWithTTF(config[0][0]["description"].asString(), "fonts/LICABOLD.ttf", round(visibleSize.width / 45));
	skill_description->setColor(Color3B::BLACK);
	skill_description->setDimensions(visibleSize.width / 5, visibleSize.height / 4);
	skill_description->setAnchorPoint(Vec2(0.5, 0.5));
	skill_description->setPosition(Vec2(visibleSize.width / 8, visibleSize.height * 0.45));
	addChild(skill_description, 3, "skill_description");

	//add buy_button
	cocos2d::ui::Button* buy_button = ui::Button::create("res/buttons/wood_nextlevel_panel.png");
	buy_button->setAnchorPoint(Vec2(0.5, 0.5));
	buy_button->setScaleX(visibleSize.width / buy_button->getContentSize().width / 6);
	buy_button->setScaleY(visibleSize.height / buy_button->getContentSize().height / 6);
	buy_button->setPosition(Vec2(visibleSize.width / 8, visibleSize.height * 0.1));
	addChild(buy_button, 3, "buy_button");
	if (root["skill"][0][0]["bought"].asBool() || 
		config[0][0]["cost"].asInt() > root["holy_sugar"].asInt()){
		buy_button->setEnabled(false);
	}

	//add label buy
	Label* buy_label = Label::createWithTTF("Buy", "fonts/LICABOLD.ttf", round(visibleSize.width / 35));
	buy_label->setColor(Color3B::ORANGE);
	buy_label->enableOutline(Color4B::BLACK, 2);
	buy_label->setAlignment(TextHAlignment::CENTER);
	buy_label->setAnchorPoint(Vec2(0.5, 0.5));
	buy_label->setPosition(Vec2(visibleSize.width / 8, visibleSize.height * 0.12));
	addChild(buy_label, 3, "buy_label");

	//skill_cost + sprite under buy_label
	Sprite* sugar_sprite2 = Sprite::create("res/buttons/sugar.png");
	sugar_sprite2->setAnchorPoint(Vec2(0.5, 0.5));
	sugar_sprite2->setScale(visibleSize.width / sugar_sprite2->getContentSize().width / 35);
	sugar_sprite2->setPosition(Vec2(visibleSize.width / 8 - visibleSize.width / 50, visibleSize.height * 0.07));
	addChild(sugar_sprite2, 3, "sugar_sprite2");
	
	Label* skill_cost = Label::createWithTTF("X " + config[0][0]["cost"].asString(), "fonts/LICABOLD.ttf", round(visibleSize.width / 45));
	skill_cost->setColor(Color3B::BLACK);
	skill_cost->setPosition(Vec2(visibleSize.width / 8 + visibleSize.width / 50, sugar_sprite2->getPosition().y - visibleSize.height * 0.005));
	if (config[0][0]["cost"].asInt() > root["holy_sugar"].asInt()) {
		skill_cost->setColor(Color3B::RED);
	}
	addChild(skill_cost, 3, "skill_cost");

	//total number of skill tiers
	int tier_size = config.size();
	// Create the scrollview by horizontal
	ui::ScrollView* scrollView = ui::ScrollView::create();
	scrollView->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	scrollView->setContentSize(Size(visibleSize.width * 3 / 4, visibleSize.height));
	scrollView->setPosition(Vec2(visibleSize.width / 4, 0));
	scrollView->setInnerContainerSize(Size(tier_size * visibleSize.width / 6 + visibleSize.width / 20, visibleSize.height));
	addChild(scrollView, 1, "scrollView");

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
		for (unsigned int j(0); j < config[i].size(); ++j){
			set_skill(i, j);
			set_dependancy(i, j);
		};
	};

	//current selected skill
	Sprite* selected_skill = Sprite::create("res/buttons/yellow.png");
	selected_skill->setAnchorPoint(Vec2(0.5, 0.5));
	selected_skill->setScaleX(visibleSize.width / selected_skill->getContentSize().width * 0.11);
	selected_skill->setScaleY(visibleSize.height / selected_skill->getContentSize().height * 0.12);
	selected_skill->setPosition(getChildByName("scrollView")->getChildByName("0_0")->getPosition());
	scrollView->addChild(selected_skill, 1, "selected_skill");

	//buy_skill + confirmation message
	buy_button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Size visibleSize = Director::getInstance()->getVisibleSize();

			//buy confirmation 
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

			addChild(buy_info, 3, "buy_info");
			addChild(yes, 3, "yes");
			addChild(no, 3, "no");
			getChildByName("mask")->setVisible(!getChildByName("mask")->isVisible());

			no->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
				if (type == ui::Widget::TouchEventType::ENDED) {
					removeChildByName("buy_info");
					removeChildByName("yes");
					removeChildByName("no");
					getChildByName("mask")->setVisible(false);
				}
			});
			
			yes->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
				if (type == ui::Widget::TouchEventType::ENDED) {
					root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file

					int is_skill_bought = root["skill"][selected_tier_id][selected_skill_id]["bought"].asBool();

					//have enough holy_sugar ???
					if (root["holy_sugar"].asInt() >= config[selected_tier_id][selected_skill_id]["cost"].asInt()){
						//is skill already bought
						if (is_skill_bought == 1){
							return;
						}
						if (is_skill_bought == 0){
							root["skill"][selected_tier_id][selected_skill_id] = 1;

							((cocos2d::ui::Button*)getChildByName("buy_button"))->setEnabled(false);
							removeChildByName("buy_info");
							removeChildByName("yes");
							removeChildByName("no");
							getChildByName("mask")->setVisible(false);

							//update total amount holy_sugar
							root["holy_sugar"] = root["holy_sugar"].asInt() - config[selected_tier_id][selected_skill_id]["cost"].asInt();
							((Label*)getChildByName("sugar_amount"))->setString("X " + root["holy_sugar"].asString());
							((AppDelegate*)Application::getInstance())->getConfigClass()->setSave(root);
							((AppDelegate*)Application::getInstance())->getConfigClass()->save();

							//update skill panels, check box
							getChildByName("scrollView")->getChildByName(Value(selected_tier_id).asString() + "_" + Value(selected_skill_id).asString())->getChildByName("skill")->getChildByName("skill_bought")->setVisible(true);
							skill_update(selected_tier_id, selected_skill_id, (ui::Button*) getChildByName("scrollView")->getChildByName(Value(selected_tier_id).asString() + "_" + Value(selected_skill_id).asString())->getChildByName("skill"));
							
							for (int i(0); i < config[selected_tier_id + 1].size(); ++i){
							skill_update(selected_tier_id + 1, i, (ui::Button*) getChildByName("scrollView")->getChildByName(Value(selected_tier_id +1).asString() + "_" + Value(i).asString())->getChildByName("skill"));
							}
						}
					}
					else {
						return;
					}
				}
			});
		}
	});

	return true;
}

void Skills::set_skill(int tier_id, int skill_id){ //create skill button
	Size visibleSize = Director::getInstance()->getVisibleSize();

	//create a layout which contains sprites (skill, text)
	cocos2d::ui::Layout* layout = ui::Layout::create();
	getChildByName("scrollView")->addChild(layout, 2, Value(tier_id).asString() + "_" + Value(skill_id).asString());
	layout->setAnchorPoint(Vec2(0.5, 0.5));

	//add skill to layout
	ui::Button* skill = ui::Button::create();

	//check mark on bought skill
	Sprite* skill_bought = Sprite::create("res/buttons/validate.png");
	skill_bought->setAnchorPoint(Vec2(1, 1));
	skill_bought->setScaleX(visibleSize.width / skill_bought->getContentSize().width * 0.1);
	skill_bought->setScaleY(visibleSize.height / skill_bought->getContentSize().height * 0.1);
	skill->addChild(skill_bought, 3, "skill_bought");
	skill_bought->setVisible(false);
	if (root["skill"][tier_id][skill_id]["bought"].asBool()){
		skill_bought->setVisible(true);
	}
	skill_update(tier_id, skill_id, skill);

	layout->addChild(skill, 2, "skill");
	skill->addTouchEventListener([&, tier_id, skill_id](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Size visibleSize = Director::getInstance()->getVisibleSize();

			//update skill info and the selection rectangular
			if (root["holy_sugar"].asInt() < config[tier_id][skill_id]["cost"].asInt()){
				((Label*)getChildByName("skill_cost"))->setColor(Color3B::RED);
			}
			else{
				((Label*)getChildByName("skill_cost"))->setColor(Color3B::BLACK);
			}
			((Label*)getChildByName("skill_cost"))->setString("X " + config[tier_id][skill_id]["cost"].asString());
			((Label*)getChildByName("skill_name"))->setString(config[tier_id][skill_id]["name"].asString());
			((Label*)getChildByName("skill_description"))->setString(config[tier_id][skill_id]["description"].asString());
			getChildByName("scrollView")->getChildByName("selected_skill")->setPosition(getChildByName("scrollView")->getChildByName(Value(tier_id).asString() + "_" + Value(skill_id).asString())->getPosition());

			selected_tier_id = tier_id;
			selected_skill_id = skill_id;

			//set buy_button enabled only if skill not bought && all prerequisite checked
			((ui::Button*)getChildByName("buy_button"))->setEnabled(true);
			if (root["holy_sugar"].asInt() < config[tier_id][skill_id]["cost"].asInt()){
				((ui::Button*)getChildByName("buy_button"))->setEnabled(false);
			}
			else{
				if (root["level"].asInt() < config[tier_id][skill_id]["level_unlocked"].asInt()){
					((ui::Button*)getChildByName("buy_button"))->setEnabled(false);
				}
				else{
					if (root["skill"][tier_id][skill_id]["bought"].asBool()){
						((ui::Button*)getChildByName("buy_button"))->setEnabled(false);
					}
					else{
						bool enable1(true);
						for (unsigned i(0); i < config[tier_id][skill_id]["dependance"].size(); ++i){
							int j = config[tier_id][skill_id]["dependance"][i].asInt();
							if (!root["skill"][tier_id - 1][j]["bought"].asBool()){
								enable1 = false;
							}
							if (enable1 == false){
								((ui::Button*)getChildByName("buy_button"))->setEnabled(false);
							}
						}
					}
				}
			}
		}
	});
	
	//positionning skills tree and buy button
	//total number of skills in tier_id
	int skill_size = config[tier_id].size(); 
	if (tier_id == 0){
		layout->setPosition(Vec2(visibleSize.width / 10, visibleSize.height / 2));

	}
	else{
		if (config[tier_id].size() == 0){
			return;
		}
		if (config[tier_id].size() == 1){
			Vec2 pos_previous_skill = getChildByName("scrollView")->getChildByName(Value(tier_id - 1).asString() + "_0")->getPosition();
			layout->setPosition(Vec2(visibleSize.width / 6 + pos_previous_skill.x, visibleSize.height / 2));
		}
		if (config[tier_id].size() == 2){
			Vec2 pos_previous_skill = getChildByName("scrollView")->getChildByName(Value(tier_id - 1).asString() + "_0")->getPosition();
			layout->setPosition(Vec2(visibleSize.width / 6 + pos_previous_skill.x, visibleSize.height * (skill_id + 1) / (skill_size + 1)));
		}
		else {
			Vec2 pos_previous_skill = getChildByName("scrollView")->getChildByName(Value(tier_id - 1).asString() + "_0")->getPosition();
			layout->setPosition(Vec2(visibleSize.width / 6 + pos_previous_skill.x, visibleSize.height * (4 * skill_id + 2) / 12));
		}
	}
}

void Skills::set_dependancy(int tier_id, int skill_id){ // create link between skills
	Size visibleSize = Director::getInstance()->getVisibleSize();

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

void Skills::skill_update(int tier_id, int skill_id, ui::Button* skill){ //create skill button
	Size visibleSize = Director::getInstance()->getVisibleSize();

	/*if (tier_id == 0){ // skill n° 0_0
		skill->loadTextureNormal(config[tier_id][skill_id]["sprite_enabled"].asString());
	}*/
	if (root["level"].asInt() < config[tier_id][skill_id]["level_unlocked"].asInt()){ // compaign level < level_unlocked then disabled
		skill->loadTextureNormal(config[tier_id][skill_id]["sprite_disabled"].asString());
	}
	else {
		if (config[tier_id][skill_id]["dependance"].size() == 0){ // skill without dependance then enabled
			skill->loadTextureNormal(config[tier_id][skill_id]["sprite_enabled"].asString());
		}
		else{
			bool enable(true);
			for (unsigned i(0); i < config[tier_id][skill_id]["dependance"].size(); ++i){ // count dependance size
				int j = config[tier_id][skill_id]["dependance"][i].asInt();
				if (!root["skill"][tier_id - 1][j]["bought"].asBool()){ // if one prerequisite is not checked then false
					enable = false;
				}
			}
			if (enable){
				skill->loadTextureNormal(config[tier_id][skill_id]["sprite_enabled"].asString());
			}
			else{
				skill->loadTextureNormal(config[tier_id][skill_id]["sprite_disabled"].asString());
			}
		}
	}
	skill->setAnchorPoint(Vec2(0.5, 0.5));
	skill->setScaleX(visibleSize.width / skill->getContentSize().width * 0.1);
	skill->setScaleY(visibleSize.height / skill->getContentSize().height * 0.1);
	skill->setPosition(Vec2(0.f, 0.f));
	skill->getChildByName("skill_bought")->setPosition(Vec2(skill->getContentSize().width / 1.05, skill->getContentSize().height / 1.05));
}