#include "DangoGenerator.h"
#include "Level.h"
#include "../../Config/json.h"
#include "Dangos/Dangosimple.h"
#include "Dangos/Dangobese.h"
#include "Dangos/Dangorille.h"

USING_NS_CC;

DangoGenerator::DangoGenerator(): timer(0),step(0),cWave(0), total_steps(0), progress(0){

}

DangoGenerator::~DangoGenerator(){}

DangoGenerator* DangoGenerator::createWithFilename(std::string filename){
	DangoGenerator* generator;
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
		generator = createWithRoot(root);
	}

	return generator;
}

DangoGenerator* DangoGenerator::createWithRoot(Json::Value root) {
	DangoGenerator* generator = new DangoGenerator();
	for (int i(0); i < root["nbwaves"].asInt(); ++i) {
		generator->addWave();
		for (unsigned int j(0); j < root["dangosChain"][i].size(); ++j) {
			generator->addStep(root["dangosChain"][i][j].asString(), root["dangosTime"][i][j].asDouble(), 
				root["dangosPath"][i][j].asDouble(), i);
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
				std::string dango_type = sequenceDango[cWave][step].substr(0,sequenceDango[cWave][step].size()-1);
				int levelDango = Value(sequenceDango[cWave][step].substr(sequenceDango[cWave][step].size()-1,1)).asInt();
				if (dango_type == "dangosimple"){
					dango = Dangosimple::create(level->getPath(sequencePath[cWave][step]),levelDango);
				}
				else if(dango_type == "dangobese"){
					dango = Dangobese::create(level->getPath(sequencePath[cWave][step]),levelDango);
				}
				else if (dango_type == "dangorille") {
					dango = Dangorille::create(level->getPath(sequencePath[cWave][step]), levelDango, level);
				}
				dango->setPosition(level->getPath(sequencePath[cWave][step])[0]->getPosition());
				level->addDango(dango);
				++step;
				++progress;
				timer = 0;
			}
		}
	}
}

bool DangoGenerator::addStep(std::string dango, double time, unsigned int path, unsigned int wave){
	if (wave < sequenceTimer.size()){
		sequenceTimer[wave].push_back(time);
		sequenceDango[wave].push_back(dango);
		sequencePath[wave].push_back(path);
		++total_steps;
		return true;
	}
	else{
		return false;
	}
}

void DangoGenerator::addWave(){
	std::vector<std::string> dangos;
	std::vector<double> timers;
	std::vector<int> paths;
	sequenceDango.push_back(dangos);
	sequenceTimer.push_back(timers);
	sequencePath.push_back(paths);
}

void DangoGenerator::duplicateStep(unsigned int step, unsigned int wave) {
	if (step < sequenceDango[wave].size() - 1) {
		sequenceDango[wave].insert(sequenceDango[wave].begin() + step + 1, sequenceDango[wave][step]);
		sequenceTimer[wave].insert(sequenceTimer[wave].begin() + step + 1, sequenceTimer[wave][step]);
		sequencePath[wave].insert(sequencePath[wave].begin() + step + 1, sequencePath[wave][step]);
	}
	else {
		sequenceDango[wave].push_back(sequenceDango[wave][step]);
		sequenceTimer[wave].push_back(sequenceTimer[wave][step]);
		sequencePath[wave].push_back(sequencePath[wave][step]);
	}
	++total_steps;
}

void DangoGenerator::duplicateWave(unsigned int wave) {
	if (wave > sequenceDango.size() - 1) {
		sequenceDango.insert(sequenceDango.begin() + wave + 1, sequenceDango[wave]);
		sequenceTimer.insert(sequenceTimer.begin() + wave + 1, sequenceTimer[wave]);
		sequencePath.insert(sequencePath.begin() + wave + 1, sequencePath[wave]);
	}
	else {
		sequenceDango.push_back(sequenceDango[wave]);
		sequenceTimer.push_back(sequenceTimer[wave]);
		sequencePath.push_back(sequencePath[wave]);
	}
	total_steps += getNbSteps(wave);
}

void DangoGenerator::reset(){
	timer = 0;
	step = 0;
	cWave = 0;
	progress = 0;
}

void DangoGenerator::empty() {
	reset();
	sequenceDango.clear();
	sequenceTimer.clear();
	sequencePath.clear();
}

void DangoGenerator::startNextWave(){
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
int DangoGenerator::getNbSteps(unsigned int wave) {
	if ((int)wave < getNbWaves()) {
		return sequenceTimer[wave].size();
	}
	else {
		log("DangoGenerator: Wave does not exits (out of bound).");
		return 0;
	}
}

void DangoGenerator::removeStep(unsigned int step, unsigned int wave) {
	if ((int)wave < getNbWaves()) {
		if ((int)step < sequenceTimer[wave].size()) {
			sequenceTimer[wave].erase(sequenceTimer[wave].begin() + step);
			sequenceDango[wave].erase(sequenceDango[wave].begin() + step);
			sequencePath[wave].erase(sequencePath[wave].begin() + step);
			--total_steps;
		}
		else {
			log("DangoGenerator: Step out of wave size.");
		}
	}
	else {
		log("DangoGenerator: Wave does not exits (out of bound).");
	}
}

void DangoGenerator::removeWave(unsigned int wave) {
	if ((int)wave < getNbWaves()) {
		total_steps -= getNbSteps(wave);
		sequenceTimer.erase(sequenceTimer.begin() + wave);
		sequenceDango.erase(sequenceDango.begin() + wave);
		sequencePath.erase(sequencePath.begin() + wave);
	}
	else {
		log("DangoGenerator: Wave does not exits (out of bound).");
	}
}

double DangoGenerator::getTime(unsigned int step, unsigned int wave) {
	return  sequenceTimer[wave][step];
}

double DangoGenerator::getWaveTime(unsigned int wave) {
	double tot_time(0);
	if ((int)wave < getNbWaves()) {
		for (auto time : sequenceTimer[wave]) {
			tot_time += time;
		}
		return tot_time;
	}
	else {
		log("DangoGenerator: Wave does not exits (out of bound).");
		return 0;
	}
}

double DangoGenerator::getTotTime() {
	double tot_time(0);
	for (auto wave : sequenceTimer) {
		for (auto enemy : wave) {
			tot_time += enemy;
		}
	}
	return tot_time;
}

int DangoGenerator::getTotEnemies() {
	double tot_enemies(0);
	for (auto wave : sequenceDango) {
		tot_enemies += wave.size();
	}
	return tot_enemies;
}

int DangoGenerator::getPath(unsigned int step, unsigned int wave) {
	return  sequencePath[wave][step];
}
std::string DangoGenerator::getDango(unsigned int step, unsigned int wave) {
	return  sequenceDango[wave][step];
}
void DangoGenerator::setTime(double time, unsigned int step, unsigned int wave) {
	sequenceTimer[wave][step] = time;
}
void DangoGenerator::setDango(std::string name, unsigned int step, unsigned int wave) {
	sequenceDango[wave][step] = name;
}
void DangoGenerator::setPath(unsigned int path, unsigned int step, unsigned int wave) {
	sequencePath[wave][step] = path;
}

void DangoGenerator::saveInRoot(Json::Value& root) {
	root["nbwaves"] = getNbWaves();
	for (int i(0); i < getNbWaves(); ++i) {
		for (int j(0); j < getNbSteps(i); ++j) {
			root["dangosChain"][i][j] = sequenceDango[i][j];
			root["dangosTime"][i][j] = sequenceTimer[i][j];
			root["dangosPath"][i][j] = sequencePath[i][j];
		}
	}
}

float DangoGenerator::getProgress() {
	return ((float)progress) / ((float)total_steps);
}