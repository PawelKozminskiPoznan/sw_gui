/*
 * DeviceParams.cpp
 *
 *  Created on: 8 Jul 2019
 *      Author: pp
 */


#include <string>
#include "DeviceParams.h"
#include "ParamsList.h"
using namespace std;

void DeviceParams::setName(string n)
{
	this->name=n;
}
ostream& DeviceParams::write(ostream& o)
{
	o << "[" << this->name <<"]\n"; //TODO: [ i ] powinny byc gdzies zapisane, zeby uzywac tych samych znakow przy parsowaniu i wypisywaniu

	for (auto p: this->getParams())
	{
		o << p.first << " : " << p.second << endl;
	}
	for (auto v: this->getVariables())
	{
		o << v.first << " = " << v.second << endl;
	}
	for (auto p: this->getPortVariables())
	{
		o << "[[" << p.first << "]]\n";
		for (auto pp: p.second)
		{
			o << pp.first << " = " << pp.second << endl;
		}
	}

	return o;
}
void DeviceParams::setVariableForPortType(string portType,string varName,string varValue)
{
	this->portVariables[portType][varName]=varValue;
}

ParamsList& DeviceParams::getVariableListForPortType(string portType)
{
	return this->portVariables[portType];
}

string& DeviceParams::getVariableForPortType(string portType,string varName)
{
	return this->portVariables[portType].at(varName);
}

ParamsList& DeviceParams::getParams()
{
	return this->params;
}
ParamsList& DeviceParams::getVariables()
{
	return this->variables;
}


std::map<std::string,ParamsList>& DeviceParams::getPortVariables()
{
	return this->portVariables;
}


DeviceParams::DeviceParams() {
	// TODO Auto-generated constructor stub

}

DeviceParams::~DeviceParams() {
	// TODO Auto-generated destructor stub
}


string DeviceParams::getNameDevice()
{
    if(&name == nullptr)
        return name;
    else

        throw "";
}
