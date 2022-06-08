/*
 * DeviceParams.h
 *
 *  Created on: 8 Jul 2019
 *      Author: pp
 */

#ifndef DEVICEPARAMS_H_
#define DEVICEPARAMS_H_

#include <ostream>
#include <string>
#include <vector>
#include <map>

#include "ParamsList.h"
#include "RangeConverter.h"

class DeviceParams {
public:
	DeviceParams();
	virtual ~DeviceParams();

	void setName(std::string devName);

	ParamsList& getParams();
	ParamsList& getVariables();
	std::map<std::string,ParamsList>& getPortVariables();
	std::string getNameDevice();


	void setVariableForPortType(std::string portType,std::string varName,std::string varValue);
	ParamsList& getVariableListForPortType(std::string portType);
	std::string& getVariableForPortType(std::string portType,std::string varName);

	std::ostream& write(std::ostream& o);
private:
	std::string name;

	ParamsList params;
	ParamsList variables;

	std::map<std::string,ParamsList> portVariables;

};

#endif /* DEVICEPARAMS_H_ */
