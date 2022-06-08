/*
 * Project.h
 *
 *  Created on: 8 Jul 2019
 *      Author: pp
 */

#ifndef PROJECT_H_
#define PROJECT_H_

#include <fstream>
#include <string>

#include "ConfigLinesList.h"
#include "DeviceParams.h"
#include "CheckerFileProject.h"

#include <map>
#include <vector>
#include <sstream>
#include <ostream>

using namespace std;

class Project {
public:
	const string VARIABLE_REGEX="\\$[[:upper:]_[:digit:]]+";

	Project(string fileName,bool);
	virtual ~Project();

	ostream& write(std::ostream& o);


    map<std::string,std::stringstream> createConfigFiles(map<string,int>);
	stringstream& createConfigForPortType(stringstream& ss,string portTypeName, string portRangeString, vector<string>& configLinesFromTemplate,DeviceParams& dp);
    void saveOutputFile(map<string,stringstream>& configs);
	vector<string>* getLogs();
	unsigned int getNumberOfDevices();
    map<string,multimap<string,pair<string,string>>> getParsedProject();
private:
    string fileName;
    ifstream f;
	std::string projectName;
	unsigned int numberOfDevices;
	std::vector<std::string> namesOfDevices;
	bool show;
	string extension;
	string commentSing;
	std::vector<std::pair<std::string,std::string>> projectVariables;

	DeviceParams* currentDeviceParamsForParsing;
	std::string   currentPortType;
	std::map<std::string,DeviceParams> devices;

	vector<string> usedSlotsPort;
	//vector<int> usedSlots;

    vector<pair<string,string>> extensionForDevice;

	DeviceParams* getCurrentDeviceParams();
	void setCurrentDeviceParams(std::string& deviceName);
    void setCurrentPortType(const string&);
	std::string& getCurrentPortType();
	vector<string> sectionFromTemplate(string templateName);
    void parse(std::ifstream& f);

	std::vector<std::string> logs;

	CheckerFileProject chFileProject;

	std::vector<std::pair<std::string,std::string>> getAllVariablesReplacings(std::string line, std::string& portNumber, ParamsList& p);
    void declareProjectFileLine(const string& arg1, const string& arg2, const string& arg3, const string& arg4);
    map<string,multimap<string,pair<string,string>>> projectFile;
    void logMsg(const string &msg);
    string getOnlyProjName(string proj);
    string onlyProjectName;
};

#endif /* PROJECT_H_ */
