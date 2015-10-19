#include "Config.h"
#include "cocos2d.h"
#include <fstream>
#include <istream>

USING_NS_CC;

Config::Config(std::string configfilename, std::string savename) : reader(), readerSav(), saveFile(false){

	bool parsingConfigSuccessful(false);
	bool parsingSaveSuccessful(false);

	if(CC_TARGET_PLATFORM == CC_PLATFORM_LINUX){
		configfilename = "Resources/" + configfilename;
		savename = "Resources/" + savename;
	}
	if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID){
		std::string document = "{\"towers\":{\"archer\":{\"name\":\"archer\",\"description\":\"Archer:Thismallowisalongdistanceattacker.\",\"attack_speed\":1.5,\"damages\":1,\"range\":120,\"cost\":30,\"image\":\"res/turret/tower.png\",\"image_menu\":\"res/turret/tower_menu.png\",\"image_menu_disable\":\"res/turret/tower_menu_disable.png\",\"animation\":\"res/turret/animations/archer.png\",\"animation_size\":15,\"animation_steady_size\":3},\"grill\":{\"name\":\"grill\",\"description\":\"FlameThrower:Selectatargetandburnitovertime.\",\"attack_speed\":2.5,\"damages\":1,\"range\":80,\"cost\":50,\"image\":\"res/turret/grill.png\",\"image_menu\":\"res/turret/grill_menu.png\",\"image_menu_disable\":\"res/turret/grill_menu_disable.png\",\"animation\":\"res/turret/animations/grill.png\",\"animation_size\":1,\"animation_steady_size\":4}},\"dangos\":{\"dango\":{\"speed\":100,\"hitpoints\":2}},\"levels\":{\"level1\":{\"generator\":\"res/level1_generator.json\",\"sugar\":40}}}";
		parsingConfigSuccessful = reader.parse(document, root, false);
	}
	else{
		std::ifstream ifs;
		ifs.open(configfilename, std::ifstream::binary);
		parsingConfigSuccessful = reader.parse(ifs, root, false);
		
		std::ifstream ifs2;
		ifs2.open(savename, std::ifstream::binary);
		parsingSaveSuccessful = readerSav.parse(ifs2, rootSav, false);
	}
	
	if (!parsingConfigSuccessful)
	{
		// report to the user the failure and their locations in the document.
		std::string error = reader.getFormattedErrorMessages();
		return;
	}
	if (!parsingSaveSuccessful)
	{
		// report to the user the failure and their locations in the document.
		std::string error = readerSav.getFormattedErrorMessages();
	}
	else{
		saveFile = true;
	}

}

Json::Value const Config::getConfigValues() const{
	return root;
}

Json::Value const Config::getSaveValues() const{
	return rootSav;
}

bool Config::isSaveFile() const{
	return saveFile;
}
