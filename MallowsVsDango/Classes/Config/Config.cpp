#include "Config.h"
#include "cocos2d.h"

USING_NS_CC;

Config::Config(std::string configfilename, std::string savename) : 
	config_filename(configfilename), save_filename(savename), saveFile(false){
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
void Config::setSave(Json::Value nroot){
	rootSav = nroot;
}
void Config::save(){
	Json::StyledWriter writer;
	std::string outputSave = writer.write(rootSav);
	std::string path = FileUtils::getInstance()->getWritablePath() + save_filename;

	bool succeed = FileUtils::getInstance()->writeStringToFile(outputSave, path);
	if(succeed){
		log("Saved File in %s",path.c_str());
	}
	else{
		log("error saving file %s", path.c_str());
	}
}

void Config::init(){
	auto fileUtils =  FileUtils::getInstance();

	std::string configFile = fileUtils->getStringFromFile(config_filename);
	std::string saveFile = fileUtils->getStringFromFile(FileUtils::getInstance()->getWritablePath() + save_filename);

	Json::Reader reader;
	Json::Reader readerSav;
	bool parsingConfigSuccessful(false);
	bool parsingSaveSuccessful(false);

	parsingConfigSuccessful = reader.parse(saveFile, rootSav, false);
	parsingSaveSuccessful = readerSav.parse(configFile, root, false);

	if (!parsingConfigSuccessful){
		// report to the user the failure and their locations in the document.
		std::string error = reader.getFormattedErrorMessages();
		return;
	}
	if (!parsingSaveSuccessful){
		// report to the user the failure and their locations in the document.
		std::string error = readerSav.getFormattedErrorMessages();
	}
	else{
		saveFile = true;
	}
	save();
}
