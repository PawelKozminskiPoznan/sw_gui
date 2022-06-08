/*
 * DeviceTemplate.cpp
 *
 *  Created on: 8 Jul 2019
 *      Author: pp
 */

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <regex>
#include "DeviceTemplate.h"

#include "Utils.h"

using namespace std;

DeviceTemplate::DeviceTemplate(string fileName) {
	// TODO Auto-generated constructor stub
	ifstream f (fileName);
	if (!f.is_open())
	{
		throw runtime_error("Unable to open file: " + fileName);
	}
	parse(f);
}

map<string,ConfigLinesList>& DeviceTemplate::getSettings()
{
	return this->settings;
}

void DeviceTemplate::setCurrentSection(string sectionName)
{
	if (sectionName=="")
		duringParseCurrentSection=0;
	else
	{
		duringParseCurrentSection = &settings[sectionName];
		duringParseCurrentSection->setName(sectionName);
		//cout << DEBUG_PREF << ": " << sectionName<<endl;
	}
}

ConfigLinesList* DeviceTemplate::getCurrentSection()
{
	return duringParseCurrentSection;
}

bool DeviceTemplate::parseLine(string l)
{
	//usuwamy biale znaki z konca i poczatku
	l=pputils::trim(l,TRIM_LEADING & TRIM_TRAILING);

//	if (pputils::isThisAComment(l)) return true;

	//search for a section  '[[name-of-the-section]]'
	regex r("\\[[[:alpha:]\\-]+\\]");
	if (regex_match(l,r))
	{
		string l2(l.begin()+1,l.end()-1); //omit '[' and ']'
		setCurrentSection(l2);
	}
	else //jeśli to nie jest sekcja to parametry pod sekcją
	{
		ConfigLinesList *p=getCurrentSection();
		if (!p) return false;
		p->push_back(l);
	}
	return true;
}

bool DeviceTemplate::parse(ifstream &f)
{
	vector<string> fileLines;
	string line;

	while (getline(f,line))
	{
		int len=line.length();
		if (len<256)//len > 0 &&
		{
			fileLines.push_back(line);
			//cout << DEBUG_PREF << line << endl;
			parseLine(line);
		}
	}
	return true;
}

DeviceTemplate::~DeviceTemplate() {
	// TODO Auto-generated destructor stub
}

