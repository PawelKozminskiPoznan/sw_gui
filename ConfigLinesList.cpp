/*
 * ParamsList.cpp
 *
 *  Created on: 8 Jul 2019
 *      Author: pp
 */

#include "ConfigLinesList.h"

#include <string>

using namespace std;

ConfigLinesList::ConfigLinesList() {
	// TODO Auto-generated constructor stub

}

ConfigLinesList::~ConfigLinesList() {
	// TODO Auto-generated destructor stub
}

void ConfigLinesList::setName(string& n)
{
	name=n;
}

string& ConfigLinesList::getName()
{
	return name;
}

