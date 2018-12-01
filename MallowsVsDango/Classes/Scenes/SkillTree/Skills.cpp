#include "Skills.h"
#include "../../AppDelegate.h"
#include "../../SceneManager.h"

USING_NS_CC;

Skills * Skills::create(Config * config, GUISettings* settings)
{
	Skills* skillScene = new Skills();

	if (skillScene->init(config, settings))
	{
		skillScene->autorelease();
		return skillScene;
	}
	else
	{
		delete skillScene;
		return NULL;
	}
}

bool Skills::init(Config* config, GUISettings* settings) {
	configClass = config;
	if (!AdvancedScene::init(settings)) { return false; }

	tutorial_running = false;
	currentTalent = Json::Value::null;
	c_button = nullptr;
	tutorial = new SkillTutorial(configClass->getSkillTutorialSettings(), this, settings);

	Json::Value talents = configClass->getConfigValues(Config::ConfigType::TALENT)["talents"]; //load data file
	Json::Value savedSkills = ((AppDelegate*)Application::getInstance())->getSave(); //load save file

	initSkillTreeProperties(savedSkills, talents);

	addBackground();
	addBlackMask();
	createBackToMainMenuButton();
	createLeftPanelForSkillDescrition(savedSkills);
	createValidationLayout();
	createBuyingButtonLayout();
	createRequirementDescription(configClass);
	initSkillTree();

	return true;
}

void Skills::createRequirementDescription(Config* configClass)
{
	requirementDescription = RequirementDescription::create(this, settings, configClass);
	updateRequirementDescriptionPosition();
	addChild(requirementDescription, 3, "requirement_description");
}

void Skills::updateRequirementDescriptionPosition()
{
	cocos2d::Label* skillDescription = leftPanel->getSkillDescription();
	requirementDescription->setPosition(Vec2(settings->getVisibleSize().width / 8, skillDescription->getPosition().y -
		skillDescription->getContentSize().height -
		settings->getVisibleSize().height / 25));
}

void Skills::initSkillTreeProperties(Json::Value &savedSkills, Json::Value &talents)
{
	if (!savedSkills.isMember("holy_sugar")) {
		savedSkills["holy_sugar"] = 0;
	}
	if (!savedSkills.isMember("holy_sugar_spent")) {
		savedSkills["holy_sugar_spent"] = 0;
	}
	// FILLING SAVE FILE WITH VALUES
	Json::Value savedSkillsCopy = savedSkills;
	initializeDefaultSkillTree(savedSkillsCopy, talents);
	updateSkillTreeWithSaveFile(savedSkills, talents, savedSkillsCopy);
	savedSkills = savedSkillsCopy;
	configClass->setSave(savedSkills);
	configClass->save();
}

void Skills::createLeftPanelForSkillDescrition(Json::Value &savedSkills) {
	leftPanel = LeftPanel::create(savedSkills, settings);
	addChild(leftPanel);
}

void Skills::createBuyingButtonLayout()
{
	buyingLayout = BuyLayout::create(settings, CC_CALLBACK_2(Skills::buyButtonCallBack, this));
	addChild(buyingLayout, 1, "buyingLayout");
}

void Skills::createValidationLayout()
{
	validationLayout = ValidationLayout::create(settings, CC_CALLBACK_2(Skills::validationCallBack, this));
	addChild(validationLayout, 2, "validationLayout");
}

void Skills::validationCallBack(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		Json::Value save_root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file
		save_root["talents"][getSavedSkillPosFromID(currentTalent["id"].asInt())]["bought"] = true;
		save_root["holy_sugar"] = save_root["holy_sugar"].asInt() - currentTalent["cost"].asInt();
		save_root["holy_sugar_spent"] = save_root["holy_sugar_spent"].asInt() + currentTalent["cost"].asInt();
		leftPanel->setSugarAmount(save_root["holy_sugar"].asInt());
		configClass->setSave(save_root);
		configClass->save();

		//update skill panels, check box
		removeChildByName("talent_page");
		initSkillTree();
		selectSkill(currentTalent["id"].asInt());
		validationLayout->hide();
	}
}

void Skills::buyButtonCallBack(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		validationLayout->show(currentTalent);
	}
}

void Skills::addBlackMask()
{
	addChild(ui::Layout::create(), 2, "black_mask");
	ui::Button* mask = ui::Button::create("res/buttons/mask.png");
	mask->setScaleX(settings->getVisibleSize().width / mask->getContentSize().width);
	mask->setScaleY(settings->getVisibleSize().height / mask->getContentSize().height);
	mask->setPosition(Vec2(settings->getVisibleSize().width / 2, settings->getVisibleSize().height / 2));
	getChildByName("black_mask")->addChild(mask, 4);
	getChildByName("black_mask")->setVisible(false);
}

void Skills::createBackToMainMenuButton()
{
	cocos2d::ui::Button* back = ui::Button::create("res/buttons/restart_button.png");
	Sprite* back_wood = Sprite::create("res/buttons/close2_shadow.png");

	back->addChild(back_wood, -1);
	addChild(back, 2);
	back->setScale(settings->getVisibleSize().width / back->getContentSize().width / 15);
	back->setAnchorPoint(Vec2(1.f, 1.f));
	back_wood->setScale(0.98f);
	back_wood->setPosition(Vec2(back->getContentSize().width / 2,
		back->getContentSize().height / 2));
	back->setPosition(Vec2(settings->getVisibleSize().width, settings->getVisibleSize().height));
	back->addTouchEventListener([back](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			SceneManager::getInstance()->setScene(SceneFactory::LEVELS);
		}
	});
}

void Skills::addBackground()
{
	Sprite* loading_background = Sprite::create("res/background/space.png");
	loading_background->setPosition(Vec2(settings->getVisibleSize().width * 5 / 8, settings->getVisibleSize().height / 2));
	loading_background->setScale(settings->getVisibleSize().height / loading_background->getContentSize().height);
	addChild(loading_background);
}

void Skills::updateSkillTreeWithSaveFile(Json::Value &save_root, Json::Value &talents, Json::Value &root2)
{
	for (unsigned int i(0); i < save_root["talents"].size(); ++i) {
		if (save_root["talents"][i].isMember("id")) {
			if (talents[i]["id"].asInt() != save_root["talents"][i]["id"].asInt()) {
				for (unsigned int k(0); k < talents.size(); ++k) {
					if (talents[k]["id"].asInt() == save_root["talents"][i]["id"].asInt()) {
						root2["talents"][k]["id"] = talents[k]["id"].asInt();
						root2["talents"][k]["bought"] = save_root["talents"][i]["bought"].asBool();
					}
				}
			}
			else {
				root2["talents"][i]["id"] = talents[i]["id"].asInt();
				root2["talents"][i]["bought"] = save_root["talents"][i]["bought"].asBool();
			}
		}
	}
}

void Skills::initializeDefaultSkillTree(Json::Value &root2, Json::Value &talents)
{
	root2["talents"] = {};
	for (unsigned int k(0); k < talents.size(); ++k) {
		auto conf = talents[k]["id"].asInt();
		root2["talents"][k]["id"] = talents[k]["id"].asInt();
		root2["talents"][k]["bought"] = false;
	}
}

Json::Value Skills::getSkillFromID(int id) {
	Json::Value talents = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::TALENT)["talents"]; //load data file
	for (auto& talent : talents) {
		if (talent["id"] == id) {
			return talent;
		}
	}
	return Json::Value::null;
}

Json::Value Skills::getSavedSkillFromID(int id) {
	Json::Value talents = ((AppDelegate*)Application::getInstance())->getConfigClass()->getSaveValues()["talents"]; //load data file
	for (auto& talent : talents) {
		if (talent["id"] == id) {
			return talent;
		}
	}
	return Json::Value::null;
}

int Skills::getSavedSkillPosFromID(int id) {
	Json::Value talents = configClass->getSaveValues()["talents"]; //load data file
	for (unsigned int i(0); i < talents.size(); ++i) {
		if (talents[i]["id"] == id) {
			return i;
		}
	}
	return -1;
}

void Skills::initSkillTree() {
	Json::Value talents = configClass->getConfigValues(Config::ConfigType::TALENT)["talents"]; //load data file
	Json::Value save_root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file
	Size visibleSize = settings->getVisibleSize();

	// create Page view for talents
	ui::PageView* talent_pages = ui::PageView::create();
	talent_pages->setContentSize(Size(visibleSize.width * 3 / 4, visibleSize.height));
	talent_pages->setPosition(Vec2(visibleSize.width * 5 / 8 - talent_pages->getContentSize().width / 2,
		visibleSize.height / 2 - talent_pages->getContentSize().height / 2));

	int row = 0;
	int column = 0;
	int max_row = 3;
	int max_column = 4;
	ui::Layout* c_page = nullptr;

	for (auto& talent : talents) {
		// if current page is not created, let's create it
		if (c_page == nullptr) {
			c_page = ui::Layout::create();
			talent_pages->addPage(c_page);
		}

		// create the button for the specific talent
		float buttonScale = talent_pages->getContentSize().width / (1.75 * max_row);
		cocos2d::ui::Button* skillButton = createSkillButton(save_root, talent, buttonScale);
		float position_x = column * talent_pages->getContentSize().width * 1 / max_column + talent_pages->getContentSize().width * 1 / (2 * max_column);
		float position_y = -row * talent_pages->getContentSize().height * 1 / max_row + talent_pages->getContentSize().height * (2 * max_row - 1) / (2 * max_row);
		skillButton->setPosition(Vec2(position_x, position_y));
		talentButtons[talent["id"].asInt()] = skillButton;
		c_page->addChild(skillButton);
		++row;
		if (row >= max_row) {
			++column;
			row = 0;
			if (column >= max_column) {
				column = 0;
				c_page = nullptr;
			}
		}
	}
	addChild(talent_pages, 1, "talent_page");
}

cocos2d::ui::Button* Skills::createSkillButton(Json::Value &save_root, Json::Value & talent, float buttonScale)
{
	std::string sprite_name = getSkillSpriteName(save_root, talent);
	ui::Button* skillButton = ui::Button::create(sprite_name);
	skillButton->setEnabled(shouldEnableSkillButton(save_root, talent));
	skillButton->setScale(buttonScale / skillButton->getContentSize().width);

	skillButton->addTouchEventListener([&, talent, skillButton](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED && skillButton->isEnabled()) {
			selectSkill(talent["id"].asInt());
		}
	});
	if (getSavedSkillFromID(talent["id"].asInt())["bought"].asBool()) {
		setTalentBtnBought(skillButton);
	}
	if (currentTalent != Json::Value::null && currentTalent["id"].asInt() == talent["id"].asInt()) {
		c_button = skillButton;
	}
	return skillButton;
}
bool Skills::shouldEnableSkillButton(Json::Value & save_root, Json::Value & talent)
{
	return save_root["c_level"].asInt() > talent["condition_level"].asInt() && save_root["c_world"].asInt() > talent["condition_world"].asInt();
}

std::string Skills::getSkillSpriteName(Json::Value &save_root, Json::Value & talent) {
	std::string sprite_name = "";
	if (save_root["c_level"].asInt() > talent["condition_level"].asInt() && save_root["c_world"].asInt() > talent["condition_world"].asInt() &&
		save_root["holy_sugar_spent"].asInt() > talent["condition_sugar"].asInt() && (talent["condition_id"].asInt() < 0 ||
		(talent["condition_id"].asInt() > 0 && getSavedSkillFromID(talent["condition_id"].asInt())["bought"].asBool()))) {
		sprite_name = talent["sprite_enabled"].asString();
	}
	else {
		sprite_name = talent["sprite_disabled"].asString();
	}
	return sprite_name;
}

void Skills::setTalentBtnBought(ui::Button* btn) {
	Sprite* validate = Sprite::create("res/buttons/validate.png");
	validate->setScale(btn->getContentSize().width * 0.4 / validate->getContentSize().width);
	validate->setPosition(Vec2(btn->getContentSize().width * 5 / 6, validate->getContentSize().height * validate->getScaleY() / 3));
	btn->addChild(validate);
}

void Skills::showValidationPanel()
{
	validationLayout->show(currentTalent);
}

void Skills::selectSkill(int id) {
	Json::Value talent = getSkillFromID(id);
	ui::Button* talent_btn = talentButtons[id];

	Size visibleSize = settings->getVisibleSize();
	std::string language = settings->getLanguage();
	Json::Value talents = configClass->getConfigValues(Config::ConfigType::TALENT)["talents"]; //load data file
	Json::Value save_root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file

	((Label*)leftPanel->getChildByName("skill_name"))->setString(talent["name_" + language].asString());

	leftPanel->updateSkillDescription(talent);
	updateBuyingCapacity(talent, save_root);
	requirementDescription->update(talent, save_root);
	updateRequirementDescriptionPosition();
	updateBuyingLayoutVisibility(talent);
	removeSelectedEffectFromSkillButton();
	c_button = talent_btn;
	currentTalent = talent;
	addSelectedEffectToSkillButton(talent_btn);
}

cocos2d::ui::Button * Skills::getTalentButton(int index)
{
	return talentButtons[index];
}

cocos2d::ui::Button * Skills::getBuyButton()
{
	return ((cocos2d::ui::Button*)buyingLayout->getChildByName("buy_button"));
}

void Skills::removeSelectedEffectFromSkillButton()
{
	if (c_button != nullptr) {
		c_button->removeChildByName("yellow");
	}
}

void Skills::addSelectedEffectToSkillButton(cocos2d::ui::Button * talent_btn)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Sprite* selected = Sprite::create("res/buttons/Skills/selected.png");
	selected->setScale((talent_btn->getScale() * talent_btn->getContentSize().width + visibleSize.width / 20) / selected->getContentSize().width);
	selected->setPosition(Vec2(talent_btn->getContentSize().width / 2, talent_btn->getContentSize().height / 2));
	talent_btn->addChild(selected, -1, "yellow");
}

void Skills::updateBuyingLayoutVisibility(Json::Value &talent)
{
	if (getSavedSkillFromID(talent["id"].asInt())["bought"].asBool()) {
		buyingLayout->showLockedText();
	}
	else {
		buyingLayout->showBuyButton();
	}
}

void Skills::updateBuyingCapacity(Json::Value &talent, Json::Value &save_root)
{
	if (!hasEnoughHolySugar(talent, save_root)) {
		buyingLayout->disableBuy();
	}
	else {
		buyingLayout->enableBuy();
	}
	buyingLayout->updateCost(talent["cost"].asInt());
}

bool Skills::hasEnoughHolySugar(Json::Value &talent, Json::Value &save_root)
{
	return talent["cost"].asInt() < save_root["holy_sugar"].asInt();
}

void Skills::update(float dt) {
	tutorial->update(dt);
}

void Skills::hideValidationPanel()
{
	validationLayout->hide();
}

void Skills::onEnterTransitionDidFinish() {
	removeChildByName("talent_page");
	initSkillTree();
	leftPanel->setSugarAmount(configClass->getSaveValues()["holy_sugar"].asInt());
	scheduleUpdate();
}