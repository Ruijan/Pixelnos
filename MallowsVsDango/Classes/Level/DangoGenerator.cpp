#include "DangoGenerator.h"
#include "Level.h"
#include "../Config/json.h"
#include <iostream>
#include <fstream>

DangoGenerator::DangoGenerator(): timer(0),step(0),cWave(0){

}

DangoGenerator::~DangoGenerator(){}

DangoGenerator* DangoGenerator::createWithFilename(std::string filename){
	DangoGenerator* generator = new DangoGenerator();
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful(false);


	if(CC_TARGET_PLATFORM == CC_PLATFORM_LINUX){
		filename = "Resources/" + filename;
	}
	if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID){
		std::string document = "{\"nbwaves\":3,\"dangosChain\":[[0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0]],\"dangosTime\":[[2,2,2,2,2,2,2,2,2,2],[2,0.75,2,0.75,2,0.75,2,0.75,2,0.75],[3,0.75,0.75,3,0.75,0.75,4,0.75,0.75,0.75]]}";
		parsingSuccessful = reader.parse(document, root, false);
	}
	else{
		std::ifstream ifs;
		ifs.open(filename, std::ifstream::binary);
		parsingSuccessful = reader.parse(ifs, root, false);
	}
		
	if (!parsingSuccessful)
	{
		// report to the user the failure and their locations in the document.
		std::string error = reader.getFormattedErrorMessages();
		//CCLOG(error.c_str());
		//CCLOG("Failed to parse configuration\n");
		std::cerr << error<<std::endl;
		return nullptr;
	}
	else{
		for (unsigned int i(0); i < root["nbwaves"].asInt(); ++i){
			generator->addWave();
			for (unsigned int j(0); j < root["dangosChain"][i].size(); ++j){
				generator->addStep(root["dangosChain"][i][j].asInt(), root["dangosTime"][i][j].asDouble(),i);
			}
		}
	}

	return generator;
}

DangoGenerator* DangoGenerator::createWithDefaultValues(){
	DangoGenerator* generator = new DangoGenerator();
	/*generator->addStep(0, 2.0);
	generator->addStep(0, 2.0);
	for(int i(0); i < 10; ++i){
		generator->addStep(0,1.0);
	}
	for(int i(0); i < 10; ++i){
		generator->addStep(0,0.5);
	}
	for(int i(0); i < 10; ++i){
		generator->addStep(0,0.1);
	}*/
	return generator;
}

void DangoGenerator::update(double dt, Level* level){
	timer += dt;
	if (step < sequenceTimer[cWave].size()){
		if (timer > sequenceTimer[cWave][step]){
			if (step < sequenceTimer[cWave].size() - 1){
				Dango* dango(nullptr);
				switch (sequenceDango[cWave][step]){
					case 0:
						dango = Dango::create("dango1_attack_000.png", level->getPath());
						dango->setPosition(level->getPath()[0]->getPosition());
						break;
				}
				level->addDango(dango);
				++step;
				timer = 0;
			}
		}
	}
}

bool DangoGenerator::addStep(unsigned int dango, double time, int wave){
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
	std::vector<unsigned int> dangos;
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
