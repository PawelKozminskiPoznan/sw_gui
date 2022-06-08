/*
 * Project.cpp
 *
 *  Created on: 8 Jul 2019
 *      Author: pp
 */

#include <iostream>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include "RangeConverter.h"
#include "Project.h"
#include "DeviceTemplate.h"
#include "Utils.h"
#include <stdio.h>
#include <string.h>


using namespace std;

Project::Project(string fileName, bool show) {
    onlyProjectName = getOnlyProjName(fileName);
    this->fileName = fileName;
    chFileProject.setProjPath(fileName);
	currentDeviceParamsForParsing = nullptr;
	extension = ".cfg";
	commentSing = "!";
    f.clear();
    f.open(fileName, ios::in);
	this->show = show;
	if (!f.is_open())
	{
        logMsg("Nie można odtworzyć pliku projektu. Ścieżka: " + fileName);
		throw runtime_error("Unable to open file: " + fileName);
	}
	projectName = fileName;
	parse(f);
}

Project::~Project() {
	// TODO Auto-generated destructor stub
}


ostream& Project::write(ostream& o)
{
	o << "number of devices:" << this->numberOfDevices << endl; //TODO: używane opisy powinny być gdzieś zdefiniowane, żeby ciąg tutaj i przy parsowaniu zawsze byl taki sam
	o << "names: " ;
	copy(this->namesOfDevices.begin(),this->namesOfDevices.end(),ostream_iterator<string>(o,", "));
	o << endl;

	for (auto d: this->devices)
	{
		o << "#[" << d.first << "]\n";
		d.second.write(o);
	}
	return o;
}

DeviceParams* Project::getCurrentDeviceParams()
{
	return currentDeviceParamsForParsing;
}
void Project::setCurrentDeviceParams(std::string& deviceName)
{
	if (!deviceName.size())
        currentDeviceParamsForParsing=nullptr;
	else
	{
		currentDeviceParamsForParsing=&devices[deviceName];
		currentDeviceParamsForParsing->setName(deviceName);
	}
}

void Project::setCurrentPortType(const string& pT)
{
	this->currentPortType=pT;
}
string& Project::getCurrentPortType()
{
	return this->currentPortType;
}

void Project::parse(ifstream& f)
{
	string line;
	char ss[260];
    logMsg("----------Parsing start");
	while (getline(f,line))
	{
        unsigned int len=line.length();
		if (len > 0 && len<256)
		{
			//cout << DEBUG2_PREF << line << endl;
            if (1==sscanf (line.c_str(),"number of devices:%d",&numberOfDevices))
			{
                logMsg("Parsed number of devices: "+to_string(numberOfDevices));
                declareProjectFileLine("number of devices:", to_string(numberOfDevices), "project", "firstLine");
			}
			else if (1==sscanf(line.c_str(),"names:%[^\n]",ss))  //TODO: brak ograniczenia na długość
			{
                string allDev = "";
                bool firstElement = true;

				string s(ss);
				s=pputils::trim(s,TRIM_LEADING & TRIM_TRAILING);
                regex r("[^,]+");
				auto wordBegin = sregex_iterator(s.begin(),s.end(),r);
				auto wordEnd = sregex_iterator();
				for (sregex_iterator i = wordBegin; i != wordEnd; ++i)
				{
					smatch match = *i;
					string matchStr = match.str();
                    if(matchStr[0] == ' ')
                        matchStr = matchStr.substr(1);
                    if (regex_match(matchStr, regex("[A-Za-z0-9-_\\.]*")))
					{
                        logMsg("@@Parsed and add device: " + matchStr);
						this->namesOfDevices.push_back(matchStr);

                        if(!firstElement)
                        {
                            allDev += ", ";
                        }
                        firstElement = false;
                        allDev += matchStr;
					}
                }
                declareProjectFileLine("names:", allDev, "project", "secondLine");
			}
			else if (regex_match(line,regex("\\[\\[.+\\]\\]")))
			{
				string l2(line.begin()+2,line.end()-2); //omit '[[' and ']]'
				string nameCurrentDevice = currentDeviceParamsForParsing->getNameDevice();
                logMsg("Parsed and set current port: " + l2 +" on device: " + nameCurrentDevice);
				setCurrentPortType(l2);
			}
			else if (regex_match(line,regex("\\[.+\\]")))
			{
				string l2(line.begin()+1,line.end()-1); //omit '[' and ']'
                logMsg("");
                logMsg("$$$$$$$$$$$$$Set current device: " + l2);
				setCurrentDeviceParams(l2);
				setCurrentPortType("");
				chFileProject.incrementNumberOfDevicesTemplates();
				chFileProject.addNamesOfDevicesFromBrackets(l2);
                declareProjectFileLine(l2, "[" + l2 + "]", l2, "deviceNameBrackets");
			}
			else
			{
				vector<string> p=pputils::tokenize(line,"[^:]+");
				if (p.size()==2)
				{
                    if(p[1].at(0) == ' ')
                        p[1] = p[1].substr(1);
					string nameCurrentDevice = currentDeviceParamsForParsing->getNameDevice();
                    if (!getCurrentPortType().size())
					{
                        logMsg("Param: " + p[0] + " = " + p[1]);
						getCurrentDeviceParams()->getParams()[p[0]]=pputils::trim(p[1],TRIM_LEADING | TRIM_TRAILING);
                        if(p[0] == "template")
                            declareProjectFileLine(p[0] + ":", p[1], nameCurrentDevice, "template");
                        else if(p[0] == "max slots per port")
                            declareProjectFileLine(p[0] + ":", p[1], nameCurrentDevice, "maxSlots");
                        else
                            declareProjectFileLine(p[0] + ":", p[1], nameCurrentDevice, "ranges");
					}
					else
					{
                        logMsg("!!!Param: " + p[0] + " declaration [[port-variables]] before declared ranges. PortType= " + getCurrentPortType());
						throw std::logic_error ("!!!Param: " + p[0] + " declaration [[port-variables]] before declared ranges. PortType= " + getCurrentPortType());
					}
				}
                else if (p.size() == 3)//template: C:/...
				{
					string nameCurrentDevice = currentDeviceParamsForParsing->getNameDevice();
                    if(p[1].at(0) == ' ')
                        p[1] = p[1].substr(1);
					string templatePath = p[1] + ':' + p[2];
					if (!getCurrentPortType().size())
					{
                        declareProjectFileLine("template:", templatePath, nameCurrentDevice, "template");
                        logMsg("Param: " + p[0] + " = " + templatePath);
						getCurrentDeviceParams()->getParams()[p[0]] = pputils::trim(templatePath, TRIM_LEADING | TRIM_TRAILING);
					}
					else
					{
                        logMsg("!!!Param: " + p[0] + " declaration [[port-variables]] before declared ranges. PortType= " + getCurrentPortType());
						throw std::logic_error("!!!Param: " + p[0] + " declaration [[port-variables]] before declared ranges. PortType= " + getCurrentPortType());
					}
				}
				else
				{
					p=pputils::tokenize(line,"[^=]+");
					if (p.size()==2)
					{
                        if(p[1].at(0) == ' ')
                            p[1] = p[1].substr(1);
						DeviceParams* dp=getCurrentDeviceParams();//*
						if (!dp) {
							pair<string,string> projVariable;
							string var = p[0];
							if(var.back() == ' ')
								var.erase(var.end()-1,var.end());
							projVariable.first = var;
							projVariable.second = p[1];
							projectVariables.push_back(projVariable);
                            declareProjectFileLine(p[0]+'=', p[1], "projectVar", "variables");
						}
						else
						{
                            string nameCurrentDevice = currentDeviceParamsForParsing->getNameDevice();
							if (getCurrentPortType()=="")
							{
								dp->getVariables()[p[0]]=p[1];
                                logMsg("User variable: " + p[0] + " = " + p[1]);
                                declareProjectFileLine(p[0]+'=', p[1], nameCurrentDevice, "device");
							}
							else
							{
								auto m=dp->getVariableListForPortType(getCurrentPortType())[p[0]]=p[1];
                                logMsg("Overwrite variable or port_variable: " + p[0] + " = " + p[1]);
                                declareProjectFileLine(p[0]+'=', p[1], nameCurrentDevice, getCurrentPortType());
							}
						}
					}
				}
			}
		}
	}
    logMsg("----------Parsing end");
    logMsg("");
    logMsg("??????????Checking correctness of input files????????????????");
	chFileProject.chNamesAndTemplatesNumbers(namesOfDevices, getNumberOfDevices());
	chFileProject.chNamesAndTemplatesMatches(namesOfDevices);
	chFileProject.chDevicesPortsAmount(devices);
	for (auto dev: devices)
	{
		auto d=dev.second;
		auto templateName=d.getParams()["template"];
		vector<string> ports = chFileProject.chPortsFromTemplate(templateName);
		ParamsList params = d.getParams();
		chFileProject.chPortsMatching(ports, params);
	}
    logMsg("??????????Checking end????????????????");
    logMsg("");
}

std::stringstream& Project::createConfigForPortType(std::stringstream& output,std::string portTypeName, std::string portRangeString, std::vector<std::string>& configLinesFromTemplate,DeviceParams& dp)
{
	//cout << DEBUG_PREF << "for port type: " << portTypeName << " in range " << portRangeString << endl;
    //logMsg(DEBUG_PREF + "for port type: " + portTypeName + " in range " + portRangeString);
    logMsg("------------createConfigForPortType "+portTypeName+" start");
	if (!portRangeString.length())
	{
        logMsg("Empty port range for port type " + portTypeName + " of device ? ");
		//throw logic_error("Empty port range for port type " + portTypeName + " of device ? ");
	}
	else
	{
        size_t posSlash = fileName.find_last_of("/\\");
        string path;
        path.append(fileName.begin()+int(posSlash)+1,fileName.end()-4);//-4 jesli rozszerzenie .prj
        if(path[0] == '\\')
            path = "output" + path + "\\logs";
        else
            path = "output\\" + path + "\\logs";
        string extension = ".txt";
        path.append(extension);
        RangeConverter r(portRangeString,show,path);
		vector<string> tempNames = r.getNames();
		//sprawdzanie czy nie powtarzaja sie sloty/porty
        for(unsigned int j = 0; j< usedSlotsPort.size(); ++j)
		{
            for(unsigned int i = 0; i<tempNames.size(); ++i)
			{
				if(tempNames[i]==usedSlotsPort[j])
					throw invalid_argument("Trying of overwrite port/slot! = "+tempNames[i]);
			}
		}
        for(unsigned int i = 0; i<tempNames.size(); ++i)
		{
			usedSlotsPort.push_back(tempNames[i]);
		}

		for (auto currentPortName: r.getNames())
		{
            logMsg("Doing for port " + currentPortName);
			for (auto oneConfigLine: configLinesFromTemplate)
			{
                logMsg(oneConfigLine);

				//wstawiamy numer portu
				oneConfigLine=pputils::replace(oneConfigLine,"$P_PORT",currentPortName);

				//podmiana zmiennych znalezionych w porcie danego typu
				auto replacingPairs=getAllVariablesReplacings(oneConfigLine,currentPortName,dp.getVariableListForPortType(portTypeName));
				for (auto x: replacingPairs)
				{
					oneConfigLine=pputils::replace(oneConfigLine,x.first,x.second);
				}


				//zamiana zmiennych znalezionych w globalnych
				replacingPairs=getAllVariablesReplacings(oneConfigLine,currentPortName,dp.getVariables());
				for (auto x: replacingPairs)
				{
					oneConfigLine=pputils::replace(oneConfigLine,x.first,x.second);
				}
                logMsg("Output: " + oneConfigLine);

				if(replacingPairs.empty())
				{
					//vector<string> p=pputils::tokenize(oneConfigLine,"[$^] +");
					//if (p.size()==2)
					//{
					//	cout<<endl;
					//}
					for (auto x: projectVariables)
					{
 						oneConfigLine=pputils::replace(oneConfigLine,'$'+x.first,x.second);
					}
				}

				if (regex_search(oneConfigLine,regex(Project::VARIABLE_REGEX))) //jeśli po podmianach nadal znaleziono wzorzec zmiennej do podmiany do bład!
				{
                    logMsg("No substitution found in line '" + oneConfigLine + "'");
					throw logic_error("No substitution found in line '" + oneConfigLine + "'");
				}

				//wynik zapisujemy
				output << oneConfigLine << endl;
			}
		}
        logMsg("------------createConfigForPortType "+portTypeName+" end");
        logMsg("");
	}
	return output;
}

std::vector<std::pair<std::string,std::string>> Project::getAllVariablesReplacings(std::string line, std::string& portNumber, ParamsList& p)
{
	vector<pair<string,string> > replacingPairs;

	regex r(Project::VARIABLE_REGEX);

	regex_iterator<string::iterator> rit(line.begin(),line.end(),r);
	regex_iterator<string::iterator> rend;

	while (rit!=rend)
	{
		string from=rit->str().substr(1);

		string fromWithPortName="PORT_" + portNumber + "_" + from;
		string to;

		map<string,string>::iterator i=p.find(fromWithPortName);	//najpierw szukamy zmiennej z nazwa portu
		if (i==p.end())
		{
			i=p.find(from); //jesli nie znaleziono to szukamy "orginalnej" nazwy
		}


		if (i!=p.end()) //jesli cos znaleziono to dodajemy parę do zmiany do listy
		{
			to=(*i).second;
			replacingPairs.push_back(pair<string,string>("$" + from,to));
		}


        ++rit;
	}

	return replacingPairs;
}

map<string,stringstream> Project::createConfigFiles(map<string,int> portLimitations)
{
    logMsg("-------createConfigFiles for all devices start");
    map<string,stringstream> allConfigs;
    for (auto dev: devices)
    {
        logMsg("----------createConfigFiles "+dev.second.getNameDevice()+" start");
        auto d=dev.second;
        auto templateName=d.getParams()["template"];
        logMsg("Generating configuration for device " +dev.first + " with template: '" + templateName + "'");

        DeviceTemplate t(templateName);

        auto settings=t.getSettings();
        map<std::string,ConfigLinesList>::iterator findKey;
        findKey = settings.find("comment-sing");
        if (findKey != settings.end())
                commentSing = settings["comment-sing"].front();
        else
                commentSing = '!';
        size_t found = templateName.find("/");
        string temp;
        if(found != string::npos)
                temp = templateName.substr(found+1,templateName.size());
        vector<string> g=settings["version"];
        allConfigs[dev.first]<<commentSing<<" This is configuration for device: "<<dev.first<<endl;
        allConfigs[dev.first]<<commentSing<<" project file: "<<onlyProjectName<<".prj"<<endl;
        allConfigs[dev.first]<<commentSing<<" template: "<<temp<<"	";
        bool firstLine = false;
        for (auto s: g)
        {
            if(firstLine)
                    allConfigs[dev.first]<<commentSing<<" ";
            else
                    firstLine = true;
            allConfigs[dev.first]<<s<<endl;
        }
        findKey = settings.find("extension");
        if(findKey != settings.end())
        //extension = settings["extension"].front();
        {
            pair<string,string> devAndExt;
            devAndExt.first = dev.first;
            devAndExt.second = settings["extension"].front();
            extensionForDevice.push_back(devAndExt);
        }
        else
        {
            pair<string,string> devAndExt;
            devAndExt.first = dev.first;
            devAndExt.second = ".cfg";
            extensionForDevice.push_back(devAndExt);
        }
        g=settings["global"];

        for (auto s: g)//global sekcja
        {
            vector<pair<string,string>> rep; //wektor zmiennych i wartosci
            regex r("\\$[[:upper:]_[:digit:]]+");//szuka zmiennych $..._...
            regex_iterator<string::iterator> rit(s.begin(),s.end(),r);
            regex_iterator<string::iterator> rend;
            while (rit!=rend)
            {
                string toRep = d.getVariables()[rit->str().substr(1)];
                if(toRep == "")//zmienne USER_PROJ_...
                {
                    vector<pair<string,string>>::iterator iteratorProjVar;
                    string varProjName = rit->str().substr(1);
                    for(iteratorProjVar = projectVariables.begin(); iteratorProjVar != projectVariables.end(); ++iteratorProjVar)
                    {
                            if(iteratorProjVar->first == varProjName)
                                break;
                    }
                    if(iteratorProjVar != projectVariables.end())
                        toRep = iteratorProjVar->second;
                }
                if(toRep[0] == ' '){
                    toRep = toRep.substr(1,toRep.size());
                }
                rep.push_back(pair<string,string>(rit->str(),toRep));
                ++rit;
            }
            for (auto r: rep) //podmienianie pod nazwy zmiennych wartosci
            {
                s=pputils::replace(s,r.first,r.second);
            }
            allConfigs[dev.first]<<s<<endl; //nazwa konfigurowanego switcha
        }
        string defaultSlots = "1/1m1M";
        string devName = dev.second.getNameDevice();
        auto it = portLimitations.find(devName);
        if (it != portLimitations.end())
        {
            int limit = portLimitations[devName];
            defaultSlots += to_string(limit);
        }
        else
            defaultSlots += to_string(24);
        size_t posSlash = fileName.find_last_of("/\\");
        string path;
        path.append(fileName.begin()+int(posSlash)+1,fileName.end()-4);//-4 jesli rozszerzenie .prj
        if(path[0] == '\\')
            path = "output" + path + "\\logs";
        else
            path = "output\\" + path + "\\logs";
        string extension = ".txt";
        path.append(extension);
        RangeConverter l(defaultSlots,false,path);
        vector<string> sections = sectionFromTemplate(templateName);
        for(auto i : sections)
        {
            string portRangeString;
            if(i.compare(0,4,"port")==0)
            {
                //szukamy zakresu portów danego typu
                ParamsList pl=dev.second.getParams(); //ParamList to inaczej =map<std::string,std::string>
                for (auto pp: pl)// pp z szablonu projektu po linijce szblon proj.
                {
                    if (pp.first==("number of ports of type " + i))
                    {
                        portRangeString=pp.second;//przypisuje wartosci np 1-3
                    }
                }
                for (auto p: settings) //[...] z szablonu switcha
                {
                    if (p.first.find(i)!=string::npos)//if [port-...]
                    {
                        createConfigForPortType(allConfigs[dev.first],p.first,portRangeString,p.second,dev.second);
                        RangeConverter l(defaultSlots,false,path);
                    }
                }
            }
        }
        logMsg("----------createConfigFiles "+dev.second.getNameDevice()+" end");
        logMsg("");
        usedSlotsPort.clear();
    }
    logMsg("-------createConfigFiles for all devices end");
    logMsg("");
    return allConfigs;
}

void Project::saveOutputFile(map<string,stringstream>& configs)
{
	map<string,stringstream>::iterator it=configs.begin();
    bool devicesExist = false;
    for (it=configs.begin(); it!=configs.end();++it)
	{
        devicesExist = true;
		ofstream myFile;
		string folderProj;
		int posSlash = 0;
        replace(projectName.begin(), projectName.end(), '/', '\\');
        for(unsigned int i=projectName.size(); i>0; i--)
		{
            char a = projectName[i];
            if(a == '\\')
			{
                posSlash = int(i);
                break;
			}
		}
        unsigned int end = projectName.size();
        if(projectName[end-4] == '.')
            folderProj.append(projectName.begin()+posSlash,projectName.end()-4);//-4 jesli rozszerzenie .prj
        else
            folderProj.append(projectName.begin()+posSlash,projectName.end());
		string configFileName;
		if(folderProj[0] == '\\')
			configFileName = "output" + folderProj + "\\devices\\";
		else
			configFileName = "output\\" + folderProj + "\\devices\\";
		configFileName += (*it).first;
        vector<pair<string,string>>::iterator iterExtensionForDevice;
        for(iterExtensionForDevice = extensionForDevice.begin(); iterExtensionForDevice != extensionForDevice.end(); ++iterExtensionForDevice)
        {
            if(iterExtensionForDevice->first == (*it).first)
            {
                extension = iterExtensionForDevice->second;
                break;
            }
        }
		configFileName.append(extension);
		myFile.open(configFileName);
		myFile << (*it).second.str() << endl;
		myFile.close();
	}
}


void Project::logMsg(const string &msg)
{
    ofstream myFile;
    string path = onlyProjectName;
    path = "output\\" + path + "\\logs";
    string extension = ".txt";
    path.append(extension);
    myFile.open(path, ios::app);
    myFile <<msg<< endl;
    myFile.close();
}

vector<string> Project::sectionFromTemplate(string templateName)
{
	ifstream f (templateName);
	if (!f.is_open())
	{
		throw runtime_error("Unable to open file: " + templateName);
	}
	vector<string> templatesAndVar;
	string l;
	string sectionName;
	while(getline(f,l))
	{
		regex r("\\[[[:alpha:]\\-]+\\]");
		if (regex_match(l,r))
		{
			if(l != "[version]")
			{
				sectionName=l.substr(1,l.size()-2); //omit '[' and ']'
				templatesAndVar.push_back(sectionName);
			}
		}
	}
	f.close();
	return templatesAndVar;
}

vector<string>* Project::getLogs()
{
	return &logs;
}

unsigned int Project::getNumberOfDevices()
{
	return numberOfDevices;
}

void Project::declareProjectFileLine(const string& arg1, const string& arg2, const string& arg3, const string &arg4)
{
    pair <string,string> pairPom;
    pairPom.first = arg1;
    pairPom.second = arg2;
    projectFile[arg3].insert(pair <string, pair<string,string>> (arg4,pairPom));
}

map<string,multimap<string,pair<string,string>>> Project::getParsedProject()
{
    return projectFile;
}


string Project::getOnlyProjName(string proj)
{
    size_t posSlash = proj.find_last_of("/\\");
    size_t posDot = proj.find_last_of(".");
    string name;
    if(posDot != string::npos)
    {
        name.append(proj.begin()+int(posSlash)+1,proj.begin()+int(posDot));
        return name;
    }
    else if(posSlash != string::npos)
    {
        name.append(proj.begin()+int(posSlash)+1,proj.end());
        return name;
    }
    else
        return proj;
}
