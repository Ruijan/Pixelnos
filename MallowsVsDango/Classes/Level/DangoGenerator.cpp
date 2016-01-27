#include "DangoGenerator.h"
#include "Level.h"
#include "../Config/json.h"

USING_NS_CC;

DangoGenerator::DangoGenerator(): timer(0),step(0),cWave(0){

}

DangoGenerator::~DangoGenerator(){}

DangoGenerator* DangoGenerator::createWithFilename(std::string filename){
	DangoGenerator* generator = new DangoGenerator();
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful(false);
	auto fileUtils = FileUtils::getInstance();
	std::string document = fileUtils->getStringFromFile(filename);
	parsingSuccessful = reader.parse(document, root, false);
		
	if (!parsingSuccessful)
	{
		// report to the user the failure and their locations in the document.
		std::string error = reader.getFormattedErrorMessages();
		log("error : %s",error.c_str());
		return nullptr;
	}
	else{
		for (int i(0); i < root["nbwaves"].asInt(); ++i){
			generator->addWave();
			for (unsigned int j(0); j < root["dangosChain"][i].size(); ++j){
				generator->addStep(root["dangosChain"][i][j].asString(), root["dangosTime"][i][j].asDouble(),i);
			}
		}
	}

	return generator;
}


void DangoGenerator::update(double dt, Level* level){
	timer += dt;
	if (step < sequenceTimer[cWave].size()){
		if (timer > sequenceTimer[cWave][step]){
			if (step < sequenceTimer[cWave].size()){
				Dango* dango(nullptr);
				//log("Dango : %s",sequenceDango[cWave][step].c_str());
				std::string dango_type = sequenceDango[cWave][step].substr(0,sequenceDango[cWave][step].size()-1);
				//log("Dango : %s",sequenceDango[cWave][step].c_str());
				int levelDango = Value(sequenceDango[cWave][step].substr(sequenceDango[cWave][step].size()-1,1)).asInt();
				//log("Type of Dango : %s",dango_type.c_str());
				//log("Level of Dango : %s",sequenceDango[cWave][step].substr(sequenceDango[cWave][step].size()-1,1).c_str());
				//log("Level of Dango : %i",levelDango);
				if (dango_type == "dangosimple"){
					dango = Dangosimple::create(level->getPath(),levelDango);
				}
				else if(dango_type == "dangobese"){
					dango = Dangobese::create(level->getPath(),levelDango);
				}
				dango->setPosition(level->getPath()[0]->getPosition());
				level->addDango(dango);
				++step;
				timer = 0;
			}
		}
	}
}

bool DangoGenerator::addStep(std::string dango, double time,unsigned int wave){
	if (wave < sequenceTimer.size()){
		sequenceTimer[wave].push_back(time);
		sequenceDango[wave].push_back(dango);
		return true;
	}
	else{
		return false;
	}
}

void DangoGenerator::addWave(){
	std::vector<std::string> dangos;
	std::vector<double> timers;
	sequenceDango.push_back(dangos);
	sequenceTimer.push_back(timers);
}

void DangoGenerator::reset(){
	timer = 0;
	step = 0;
	cWave = 0;
}

void DangoGenerator::nextWave(){
	if (cWave + 1 < getNbWaves()){
		++cWave;
		step = 0;
	}
}
bool DangoGenerator::isWaveDone(){
	return (step >= sequenceTimer[cWave].size() - 1);
}
bool DangoGenerator::isDone(){
	return (isWaveDone() && cWave >= getNbWaves() - 1);
}

int DangoGenerator::getNbWaves(){
	return sequenceTimer.size();
}
