/*
 * ChekerFileProject.cpp
 *
 *  Created on: Jun 9, 2020
 *      Author: pawelkoz
 */

#include "CheckerFileProject.h"
#include "RangeConverter.h"
#include "ParamsList.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <regex>
#include <map>
#include <dirent.h>
#include <sys/stat.h>


using namespace std;

CheckerFileProject::CheckerFileProject() {
	// TODO Auto-generated constructor stub
	numberOfDevicesTemplates = 0;
}

CheckerFileProject::~CheckerFileProject() {
	// TODO Auto-generated destructor stub
}

void CheckerFileProject::chNamesAndTemplatesNumbers(vector<string> namesOfDevices, unsigned int numberOfDevices)
{
	//compare 'number of devices:' to number of devices from 'names:'
	string stringNumberOfDevices = to_string(numberOfDevices);
	string stringNamesOfDevicesSize = to_string(namesOfDevices.size());
    if(numberOfDevices != namesOfDevices.size())
	{
        logMsg("W sekcji: 'number of devices:'="+stringNumberOfDevices+" liczba urządzeń jest różna niż w sekcji: 'names:'="+stringNamesOfDevicesSize);
        throw logic_error("'number of devices:'="+stringNumberOfDevices+" is different than the number of devices from 'names:'="+stringNamesOfDevicesSize);
	}
	else
        logMsg("W sekcji: 'number of devices:'="+stringNumberOfDevices+" liczba urzadzeń jest równa z sekcją: 'names:'="+stringNamesOfDevicesSize);
	//compare amount devices declarated in "names:" and devices from [device_name]
    if(numberOfDevicesTemplates == int(namesOfDevices.size()))
	{
        logMsg("Liczba urządzeń zadeklarowanych w sekcji: 'names:'="+to_string(namesOfDevices.size())+" oraz w nawiasach: '[nazwa_urządzenia]'="+to_string(numberOfDevicesTemplates)+" jest poprawna");
	}
	else
	{
        logMsg("Liczba urządzeń zadeklarowanych w sekcji: 'names:'="+to_string(namesOfDevices.size())+" oraz w nawiasach: '[nazwa_urządzenia]'="+to_string(numberOfDevicesTemplates)+" jest niepoprawna");
        throw logic_error("Number of devices declared in 'names:'="+to_string(namesOfDevices.size())+" and from '[...]'="+to_string(numberOfDevicesTemplates)+" is INCORRET");
	}
}

void CheckerFileProject::incrementNumberOfDevicesTemplates()
{
    ++numberOfDevicesTemplates;
}

void CheckerFileProject::chNamesAndTemplatesMatches(vector<string> namesOfDevices)
{
	sort(namesOfDevices.begin(), namesOfDevices.end());
	sort(namesOfDevicesFromBrackets.begin(), namesOfDevicesFromBrackets.end());
	if(namesOfDevices == namesOfDevicesFromBrackets)
        logMsg("Nazwy urządzeń w sekcji 'names:'="+to_string(namesOfDevices.size())+" i w sekcjach '[nazwa_urządzenia]'="+to_string(namesOfDevicesFromBrackets.size())+" są identyczne");
	else
	{
        logMsg("Nazwy urządzeń w sekcji 'names:'="+to_string(namesOfDevices.size())+" i w sekcjach '[nazwa_urządzenia]'="+to_string(namesOfDevicesFromBrackets.size())+" różnią się");
        throw logic_error("Nazwy urządzeń w sekcji 'names:'="+to_string(namesOfDevices.size())+" i w sekcjach '[nazwa_urządzenia]'="+to_string(namesOfDevicesFromBrackets.size())+" różnią się");
	}
}

void CheckerFileProject::addNamesOfDevicesFromBrackets(std::string nameOfDev)
{
	namesOfDevicesFromBrackets.push_back(nameOfDev);
}

void CheckerFileProject::chDevicesPortsAmount(map<string,DeviceParams> devPar)
{
	//sprawdzanie zakresow portow - czy nie sa przekroczone zakresy
	map<string,DeviceParams>::iterator itDevPar=devPar.begin();
    for (itDevPar=devPar.begin(); itDevPar!=devPar.end();++itDevPar)//for po urzadzeniach
	{
        auto devParams= itDevPar->second;
        logMsg("For device: " + itDevPar->first);
		auto params = devParams.getParams();
        vector<int> usedPorts;
//		int maxPorts;
//		map<string,string>::iterator itPar;
//		for (itPar=params.begin(); itPar!=params.end();itPar++)
//		{
//			if(itPar->first == "number of ports")
//			{
//				maxPorts = stoi(itPar->second);
//				break;
//			}
//		}
		/*for (itPar=params.begin(); itPar!=params.end();itPar++)
		{
			if(itPar->first != "number of ports" && itPar->first != "template" )
			{
				string paramValue = itPar->second;
				RangeConverter r(paramValue,getLogs(),false);
				vector<int>::iterator itNumersPort;
				for(itNumersPort=r.getNumbers().begin(); itNumersPort!=r.getNumbers().end(); itNumersPort++)
				{
					if(!(*itNumersPort <= maxPorts && *itNumersPort > 0)) // sprawdzanie czy nie poza zakresem portow
					{
						string text = to_string(*itNumersPort);
                        logMsg("??Declared port number " + text + " was exceed scope");
						throw logic_error("Declared port number " + text + " was exceed scope");
					}
					usedPorts.push_back(*itNumersPort);
				}
			}
		}*/
		//spr czy porty sa duplikowane
		map<int,int> countMap;
		map<int,int>::iterator it;
		vector <int>::iterator i = usedPorts.begin();
        for(; i != usedPorts.end(); ++i)
		{
			it = countMap.find(*i);
			if(it != countMap.end())
                ++it->second;
			else
				countMap[*i]=1;
		}
		it=countMap.begin();
        for(; it != countMap.end(); ++it)
		{
			if(it->second > 1)
			{
				string text = to_string(it->first);
                logMsg("??Redundant declaration of port " + text);
                throw logic_error("Redundant declaration of port " + text);
			}
		}

	}
}


void CheckerFileProject::chPortsMatching(vector<string> templates,ParamsList parList)
{
	int amountMatchedPorts = 0;
    int amountPortsFromProj = 0;
	for(auto x:templates)
	{
        for(map<string,string>::iterator it=parList.begin(); it!=parList.end(); ++it)
		{
			string line = it->first;
			regex r(".*"+x+".*");
			if (regex_match(line,r))
			{
                ++amountMatchedPorts;
			}
		}
	}
    for(map<string,string>::iterator it=parList.begin(); it!=parList.end(); ++it)
    {
        string line = it->first;
        size_t z = line.find("number");
        if(z != string::npos)
            ++amountPortsFromProj;
    }
    //int sizeParList = int(parList.size()-2);//minus 2 bo 'template:' i 'max slots per port:'
    if(amountMatchedPorts != amountPortsFromProj)
	{
        logMsg("Not all ports from projects template are in device template or wrong names");
        throw logic_error("Not all ports from projects template are in device template or wrong names");
	}
}

void CheckerFileProject::setLog(vector<string>* log)
{
	logsCheck = log;
}

vector<string>* CheckerFileProject::getLogs()
{
	return logsCheck;
}

vector<string> CheckerFileProject::chPortsFromTemplate(string templateName)
{
	ifstream f (templateName);
	if (!f.is_open())
	{
        logMsg("Nie można odtworzyć pliku szablonu: " + templateName);
        throw runtime_error("Nie można odtworzyć pliku szablonu: " + templateName);
	}
	vector<string> templates;
	string l;
	while (getline(f,l))
	{
        int len=int(l.length());
		if (len > 0 && len<256)
		{
			regex r("\\[[[:alpha:]\\-]+\\]");
			if (regex_match(l,r))
			{
				string l2(l.begin()+1,l.end()-1); //omit '[' and ']'
				if(l2.compare(0,4,"port")==0)//if section starts from 'port...
				templates.push_back(l2);
			}
		}
	}
	f.close();
	return templates;
}


bool CheckerFileProject::chExistenceTemplate(string nameTemplate)
{
	ifstream f (nameTemplate);
	if (!f.is_open())
	{
		return false;
	}
	else
		return true;
	f.close();
}


bool CheckerFileProject::exsistFolder(string path)
{
    DIR *dir;
    char* p= new char[path.size()+1];
    size_t firstCitate = path.find_first_of("\"");
    size_t lastCitate = path.find_last_of("\"");
    if(firstCitate != string::npos && lastCitate != string::npos)
    {
        path.erase(path.begin()+int(firstCitate));
        path.erase(path.begin()+int(--lastCitate));
    }
    strcpy(p,path.c_str());
    if ((dir = opendir (p)) != nullptr)
        return true;
    else
        return false;
//    struct stat buffer;
//     return (stat (path.c_str(), &buffer) == 0);
}

void CheckerFileProject::logMsg(const string &msg)
{
    ofstream myFile;
    string path;
    size_t posSlash = projPath.find_last_of("/\\");
    path.append(projPath.begin()+int(posSlash)+1,projPath.end()-4);//-4 jesli rozszerzenie .prj
    if(path[0] == '\\')
        path = "output" + path + "\\logs";
    else
        path = "output\\" + path + "\\logs";
    string extension = ".txt";
    path.append(extension);
    myFile.open(path, ios::app);
    myFile <<msg<< endl;
    myFile.close();
}

void CheckerFileProject::setProjPath(const string &path)
{
    projPath = path;
}
