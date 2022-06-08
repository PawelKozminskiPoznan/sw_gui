/*
 * ParamsList.h
 *
 *  Created on: 8 Jul 2019
 *      Author: pp
 */

#ifndef CONFIGLINESLIST_H_
#define CONFIGLINESLIST_H_

#include <vector>
#include <string>
#include <map>

class ConfigLinesList:  public std::vector<std::string> {
public:
	void setName(std::string& n);
	std::string& getName();
	ConfigLinesList();
	virtual ~ConfigLinesList();
private:
	std::string name;
};

#endif /* CONFIGLINESLIST_H_ */
