/*
 * DeviceTemplate.h
 *
 *  Created on: 8 Jul 2019
 *      Author: pp
 */

#ifndef DEVICETEMPLATE_H_
#define DEVICETEMPLATE_H_

#include <vector>
#include <string>
#include <fstream>
#include <map>

#include "ConfigLinesList.h"

class DeviceTemplate {
public:
	DeviceTemplate(std::string fileName);
	virtual ~DeviceTemplate();
	ConfigLinesList scanSection(std::string name);
	std::map<std::string,ConfigLinesList>& getSettings();
private:
	unsigned int version;
	//std::map<std::string,ConfigLinesList> globalSettings;
	//std::map<std::string,ConfigLinesList> portSettings;
	std::map<std::string,ConfigLinesList> settings;
	ConfigLinesList *duringParseCurrentSection;


	void setCurrentSection(std::string sectionName);
	ConfigLinesList* getCurrentSection();
	bool parse(std::ifstream& f);
	bool parseLine(std::string line);

};

#endif /* DEVICETEMPLATE_H_ */
