#include "Shop.h"
#include "../../AppDelegate.h"
#include "../../SceneManager.h"


USING_NS_CC;

bool Shop::init() {

	if (!Scene::init()) { return false; }
	Size visibleSize = Director::getInstance()->getVisibleSize();
	root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file

	//generating a background for skills 
	Sprite* loading_background = Sprite::create("res/background/space.png");
	loading_background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	loading_background->setScaleX(visibleSize.width / loading_background->getContentSize().width);
	loading_background->setScaleY(visibleSize.height / loading_background->getContentSize().height);
	addChild(loading_background);

	//return button to levels on the top right corner
	cocos2d::ui::Button* back = ui::Button::create("res/buttons/restart_button.png");
	back->setScale(visibleSize.width / back->getContentSize().width / 15);
	back->setAnchorPoint(Vec2(1.f, 1.f));
	back->setPosition(Vec2(visibleSize.width - visibleSize.width / 40, visibleSize.height - visibleSize.height / 40));
	back->addTouchEventListener([back](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			SceneManager::getInstance()->setScene(SceneFactory::LEVELS);
		}
	});
	addChild(back, 2);

	Sprite* sugar_sprite1 = Sprite::create("res/buttons/holy_sugar.png");
	sugar_sprite1->setScale(visibleSize.width / sugar_sprite1->getContentSize().width / 12);
	sugar_sprite1->setPosition(Vec2(visibleSize.width / 20, visibleSize.height -
		sugar_sprite1->getContentSize().height * sugar_sprite1->getScaleY() / 2 - visibleSize.height / 40));
	addChild(sugar_sprite1, 3);

	//current sugar info
	Label* sugarAmount = Label::createWithTTF("x " + root["holy_sugar"].asString(), "fonts/LICABOLD.ttf", round(visibleSize.width / 25));
	sugarAmount->setColor(Color3B::YELLOW);
	sugarAmount->enableOutline(Color4B::BLACK, 2);
	sugarAmount->setPosition(sugar_sprite1->getPosition() +
		Vec2(sugar_sprite1->getContentSize().width * sugar_sprite1->getScaleX() / 2 + visibleSize.width / 20, 0));
	addChild(sugarAmount, 3, "sugarAmount");

	ui::ListView* list_view = ui::ListView::create();

	list_view->setItemsMargin(visibleSize.width / 40);
	list_view->setGravity(ui::ListView::Gravity::CENTER_HORIZONTAL);
	list_view->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	list_view->setContentSize(Size(visibleSize.width, visibleSize.height * 0.7));
	//list_view->setBackGroundColorType(ui::LAYOUT_COLOR_SOLID);
	list_view->setBackGroundColor(Color3B::GREEN);
	list_view->setPosition(Vec2(visibleSize.width / 2 - list_view->getContentSize().width / 2,
		visibleSize.height * 0.4 - list_view->getContentSize().height / 2));

	for (int i = 0; i < 10; i++)
	{
		ui::Layout* item = ui::Layout::create();
		Sprite* model = Sprite::create("res/buttons/shop_item.png");

		model->setScale(list_view->getContentSize().height * 0.9 / model->getContentSize().height);
		model->setPosition(Vec2(model->getContentSize().width * model->getScaleX() / 2,
			model->getContentSize().height * model->getScaleY() / 2));
		item->setContentSize(Size(model->getContentSize().width * model->getScale(), model->getContentSize().height * model->getScale()));

		ui::Text* title = ui::Text::create("Box of Holy Sugar", "fonts/LICABOLD.ttf", round(visibleSize.width / 35));
		title->setColor(Color3B::BLACK);
		title->setAnchorPoint(Vec2(0.5f, 1.f));
		title->setPosition(Vec2(model->getPosition().x, model->getContentSize().height * model->getScaleY() * 0.9));
		title->ignoreContentAdaptWithSize(false);
		title->setContentSize(Size(item->getContentSize().width * 0.9,
			item->getContentSize().height * 0.3));
		title->setTextHorizontalAlignment(TextHAlignment::CENTER);

		Sprite* item_image = Sprite::create("res/buttons/bunch_holy_sugar.png");
		item_image->setScale(item->getContentSize().width * 0.9 /
			item_image->getContentSize().width);
		item_image->setPosition(model->getPosition().x, model->getPosition().y - model->getContentSize().height * model->getScaleY() * 0.1);

		ui::Button* buy = ui::Button::create("res/buttons/shop.png");
		buy->setScale(model->getContentSize().width * model->getScaleX() * 0.6 / buy->getContentSize().width);
		buy->setPosition(Vec2(model->getPosition().x, model->getPosition().y -
			model->getContentSize().height * model->getScaleY() / 2));
		buy->setTitleText("$ 0.99");
		buy->setTitleFontName("fonts/LICABOLD.ttf");
		buy->setTitleColor(Color3B::RED);
		buy->setTitleFontSize(60);

		item->addChild(model);
		item->addChild(buy);
		item->addChild(title);
		item->addChild(item_image);

		list_view->pushBackCustomItem(item);
	}
	addChild(list_view);

	return true;
}

void Shop::switchLanguage() {

}