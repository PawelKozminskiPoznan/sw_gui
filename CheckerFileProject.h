/*
 * CheckerFileProject.h
 *
 *  Created on: Jun 9, 2020
 *      Author: pawelkoz
 */

#ifndef CHECKERFILEPROJECT_H_
#define CHECKERFILEPROJECT_H_

#include <vector>
#include <string>
#include <map>

#include "DeviceParams.h"


class CheckerFileProject {
public:
	CheckerFileProject();
	virtual ~CheckerFileProject();
	void incrementNumberOfDevicesTemplates();
	void chNamesAndTemplatesNumbers(std::vector<std::string> namesOfDevices, unsigned int);
	void chNamesAndTemplatesMatches(std::vector<std::string> namesOfDevices);
	void addNamesOfDevicesFromBrackets(std::string);
	void chDevicesPortsAmount(std::map<std::string,DeviceParams>);
	std::vector<std::string> chPortsFromTemplate(std::string templateName);
	void chPortsMatching(std::vector<std::string> templates,ParamsList parList);
	void setLog(std::vector<std::string>* log);
	std::vector<std::string>* getLogs();
	bool chExistenceTemplate(std::string nameTemplate);
    bool exsistFolder(std::string);
    void setProjPath(const std::string &path);
private:
	int numberOfDevicesTemplates;
	std::vector<std::string> namesOfDevicesFromBrackets;
    std::vector<std::string>* logsCheck;
    void logMsg(const std::string &msg);
    std::string projPath;
};

#endif /* CHECKERFILEPROJECT_H_ */
