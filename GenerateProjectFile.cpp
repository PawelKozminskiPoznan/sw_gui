/*
 * GenerateProjectFile.cpp
 *
 *  Created on: Jun 15, 2020
 *      Author: pawelkoz
 */

#include "GenerateProjectFile.h"
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cstring>
#include <string>
#include <regex>

#include <windows.h>
#include <sys/stat.h> 
#include <sys/types.h>

using namespace std;


GenerateProjectFile::GenerateProjectFile() {
	selectionVariable = ' ';
	nameOfProjFile = " ";
	tempFile = " ";
    newDev = true;
    newDevRange = true;
    newProjectConfig = true;
    newDevProjVar = true;
    newDevVar = true;
    newDevDevVar = true;
    newPortVar = true;
    newDevPortVar = true;
    editedProj = false;
//	bool unproperPathExsistProject = false;
//	do
//	{
//		if(!unproperPathExsistProject){
//		cout<<"Create new project file (y)/ Use existing project file(n):";
//		cin>>selectionVariable;
//		}
//		if(selectionVariable == "y" || selectionVariable == "Y")
//		{
//			cout<<"Enter project file name: ";
//			cin>>nameOfProjFile;
//			createProjectFile(checkExtensionFile(nameOfProjFile,".prj"));
//			proj = new Project(checkExtensionFile(pathProj,".prj"),false);
//		}
//		else if (selectionVariable == "n" || selectionVariable == "N")
//		{
//			cout<<"Enter the path to the project file :";
//			cin>>nameOfProjFile;
//			outputsFoldersForExsistProj(checkExtensionFile(nameOfProjFile,".prj"));
//			ifstream f (checkExtensionFile(nameOfProjFile,".prj"));
//			if (!f.is_open())
//			{
//				unproperPathExsistProject = true;
//			}
//			else
//			{
//				proj = new Project(checkExtensionFile(nameOfProjFile,".prj"),true);
//				unproperPathExsistProject = false;
//			}
//		}
//		else
//		{
//			cout<<"Use only a 'y/Y' or 'n/N' sign"<<endl;
//			selectionVariable = ' ';
//			cin.clear();
//			fflush(stdin);
//		}
//	}while(selectionVariable != "y" && selectionVariable != "n" && selectionVariable != "Y" && selectionVariable != "N" || unproperPathExsistProject);
//	configs = proj->createConfigFiles();
//	proj->saveOutputFile(configs);
}

GenerateProjectFile::GenerateProjectFile(const string& argumentT, const string& argumentO)
{
	nameOfProjFile = argumentO;
	tempFile = argumentT;
	isTemplateFile();
	createProjectFile(checkExtensionFile(nameOfProjFile,".prj"));
	proj = new Project(checkExtensionFile(pathProj,".prj"),false);
    //configs = proj->createConfigFiles();
	proj->saveOutputFile(configs);
}

GenerateProjectFile::GenerateProjectFile(short inputType, const string& argument)
{
	tempFile = " ";
	switch(inputType)
	{
		case 1://argumentR
		{
			nameOfProjFile = argument;
			outputsFoldersForExsistProj(checkExtensionFile(nameOfProjFile,".prj"));
			proj = new Project(checkExtensionFile(nameOfProjFile,".prj"),true);
			break;
		}
		case 2://argumentO
		{
			nameOfProjFile = argument;
			createProjectFile(checkExtensionFile(nameOfProjFile,".prj"));
			proj = new Project(checkExtensionFile(pathProj,".prj"),false);
			break;
		}
		case 4://argumentT
		{
			tempFile = argument;
			isTemplateFile();
			cout<<"Enter project file name: ";
			cin>>nameOfProjFile;
			createProjectFile(checkExtensionFile(nameOfProjFile,".prj"));
			proj = new Project(checkExtensionFile(pathProj,".prj"),false);
		}
	}
//	configs = proj->createConfigFiles();
	proj->saveOutputFile(configs);
}

GenerateProjectFile::~GenerateProjectFile() {
	// TODO Auto-generated destructor stub
}

string GenerateProjectFile::getFileName()
{
	return nameOfProjFile;
}

Project* GenerateProjectFile::getProject()
{
	return proj;
}

string GenerateProjectFile::checkExtensionFile(string name, string extention)
{
	if(name.size() > 4){
		if(name.substr(name.length()-4,name.length()) != extention)
			name.append(extention);
	}
	else
		name.append(extention);
	return name;
}

void GenerateProjectFile::createProjectFile(const string &nameFile)
{
	variableSwitchName = "USER_SW_SWITCH_NAME";
	setPrefixVariables();
	outputsFolders(nameFile);
	cout<<"Start creating a project file: " + nameFile<<endl;
	string text = "number of devices: ";
	string userData;
	bool isGoodNrDev = false;
	int amoutnDevices;
	do
	{
		cout<<"Enter the number of devices: ";
		cin>>userData;
		try
		{
			amoutnDevices = stoi(userData);
			text += userData;
			declareProjectFileLine("number of devices:", userData, "project", "firstLine");
			isGoodNrDev = false;
		}
        catch (...)
		{
			isGoodNrDev = true;
		}
	}while(isGoodNrDev);
	bool repeat = false;
	text = "";
    for(int i=0; i<amoutnDevices; ++i){
		cout<<"Enter the name of device nr:"<<i+1<<"/"<<amoutnDevices<<"(singly):";
		cin>>userData;
		devicesNames.push(userData);
		if(repeat)
			text += ", ";
		text += userData;
		repeat = true;
	}
	declareProjectFileLine("names:", text, "project", "secondLine");
	while(!devicesNames.empty())
	{
		usedSwitchVariables.clear();
		string nameDevice = devicesNames.front();
		declareProjectFileLine(nameDevice, "[" + nameDevice + "]", nameDevice, "deviceNameBrackets");
		cout<<"Start to create configuration for ["<<devicesNames.front()<<"] device."<<endl;
		string templatePath = templateInput();// wybieranie pliku szablonu
		declareProjectFileLine("template:", templatePath, nameDevice, "template");
		commentVariable = commentFromTemplate(templatePath);
		commentPort = commentPortFromTemplate(templatePath);
		/*text = "number of ports: ";
		int maxNumPort;
		bool isNumber = false;
		do{
			cout<<"Enter the number of ALL ports. Device: "<<devicesNames.front()<<":";
			cin>>userData;
			try{
				maxNumPort = stoi(userData);
				isNumber = true;
			}
			catch (const invalid_argument& e)
			{
				isNumber = false;
			}
		}while(!isNumber);
		text += userData;
		declareProjectFileLine("number of ports:", userData, nameDevice, "numberOfPorts");*/
		//vector<vector<string>::iterator> toDelete;
		//vector<string> portTypes;
		usedPortTypes.clear();
		createRanges(templatePath);// tworzenie zakresow dla typow portow
		var = variablesFromTemplate(templatePath);// var to mapa stringow(zmienne proj,device oraz zmienne portowe) na vector string(zmienne)

		//string overWritePort = " ";
		//vector<string> portTypesToOverwrite;
		/*while(overWritePort != "n" && portTypes.size()>0)
		{
			cout<<"Port types:  Device: ["<<devicesNames.front()<<"]"<<endl;
			map<int,string> menu;
            for(int i=0; i<portTypes.size(); ++i)
			{
				cout<<" "<<i<<". "<<portTypes[i]<<endl;
				menu[i] = portTypes[i];
			}
			cout<<"Which port type do you want to use to OVERWRITE variables? (choose number of port type / if no or no more enter 'n')"<<endl;
			int nr;
			string nrMenu;
			cin>>nrMenu;
			if(nrMenu != "n"){
				try
				{
					nr = stoi(nrMenu);
					overWritePort = menu[nr];
				}
				catch (const invalid_argument& e)
				{
					overWritePort = " ";
				}
			}
			else
				overWritePort = "n";
			bool isPortType = false;
			vector<string>::iterator toDelete;// ta nazwa juz uzyta
			for(toDelete = portTypes.begin(); toDelete != portTypes.end(); ++toDelete)
			{
				if(*toDelete == overWritePort)
				{
					isPortType = true;
					break;
				}
			}
			if(overWritePort != "n" && isPortType)//spr czy to typ
			{
				portTypesToOverwrite.push_back(overWritePort);
				portTypes.erase(toDelete);
			}
		}
*/

		//map<string,vector<string>> usedVar;
		map<string,vector<string>>::iterator iteratorVariables;
		iteratorVariables = var.find("project");
		if(iteratorVariables != var.end())
			initVariable(iteratorVariables);
		iteratorVariables = var.find("device");
		if(iteratorVariables != var.end())
			initVariable(iteratorVariables);
		for(iteratorVariables = var.begin(); iteratorVariables != var.end(); ++iteratorVariables)
			if(iteratorVariables != var.find("project") && iteratorVariables != var.find("device"))
				initVariable(iteratorVariables);
		//if(iteratorVariables != var["project"] && iteratorVariables != var["device"])
		devicesNames.pop();
	}
	writeToProjectFile();
	cout<<"End of project file creation."<<endl;
}

void GenerateProjectFile::writeToProjectFile()
{
	map<string,multimap<string,pair<string,string>>>::iterator wholeFile;
	multimap<string,pair<string,string>>::iterator section;
	multimap<string,pair<string,string>>::iterator sectionToDelete;
	multimap<string,pair<string,string>>::iterator findIter;
	wholeFile = configsProj.find("project");
	if(wholeFile != configsProj.end())
	{
		section = configsProj["project"].find("firstLine");
		if(section != configsProj["project"].end())
			myFile<<section->second.first<<" "<<section->second.second<<endl;
		myFile<<endl;
		section = configsProj["project"].find("secondLine");
		if(section != configsProj["project"].end())
			myFile<<section->second.first<<" "<<section->second.second<<endl;
		myFile<<endl;
	}
    if(!projVar.empty())
    {
        for(auto x = projVar.begin(); x != projVar.end(); ++x)
            myFile<<x->first<<" "<<x->second<<endl;
        myFile<<endl;
    }
    else
    {
        wholeFile = configsProj.find("projectVar");
        if(wholeFile != configsProj.end())
        {
            for(section = configsProj["projectVar"].begin(); section != configsProj["projectVar"].end(); ++section)
                myFile<<section->second.first<<" "<<section->second.second<<endl;
            myFile<<endl;
        }
    }
    for(wholeFile = configsProj.begin(); wholeFile != configsProj.end(); ++wholeFile)
	{
		if(wholeFile->first != "project" &&  wholeFile->first != "projectVar")
		{
			section = configsProj[wholeFile->first].find("deviceNameBrackets");
			if(section != configsProj[wholeFile->first].end()){
				myFile<<section->second.second<<endl;
				configsProj[wholeFile->first].erase(section);
			}
			section = configsProj[wholeFile->first].find("template");
			if(section != configsProj[wholeFile->first].end()){
				myFile<<section->second.first<<" "<<section->second.second<<endl;
				configsProj[wholeFile->first].erase(section);
			}
			section = configsProj[wholeFile->first].find("numberOfPorts");
			if(section != configsProj[wholeFile->first].end()){
				myFile<<section->second.first<<" "<<section->second.second<<endl;
				configsProj[wholeFile->first].erase(section);
			}
			bool secondTime = false;
			section = configsProj[wholeFile->first].find("ranges");
			if(section != configsProj[wholeFile->first].end())
			{
                for(section = configsProj[wholeFile->first].find("ranges"); section != configsProj[wholeFile->first].end(); ++section)
					if(section->first == "ranges"){
						myFile<<section->second.first<<" "<<section->second.second<<endl;
						if(secondTime)
							configsProj[wholeFile->first].erase(sectionToDelete);
						sectionToDelete = section;
						secondTime = true;
					}
					else
						break;
				configsProj[wholeFile->first].erase(sectionToDelete);
            }
            section = configsProj[wholeFile->first].find("maxSlots");
            if(section != configsProj[wholeFile->first].end())
            {
                string limitPort = section->second.second;
                if(limitPort[0] == ' ')
                    limitPort = limitPort.substr(1);
                myFile<<section->second.first<<limitPort<<endl;
                configsProj[wholeFile->first].erase(section);
            }
			secondTime = false;
			section = configsProj[wholeFile->first].find("device");
			if(section != configsProj[wholeFile->first].end())
			{
                for(section = configsProj[wholeFile->first].find("device"); section != configsProj[wholeFile->first].end(); ++section)
					if(section->first == "device"){
						myFile<<section->second.first<<" "<<section->second.second<<endl;
						if(secondTime)
							configsProj[wholeFile->first].erase(sectionToDelete);
						sectionToDelete = section;
						secondTime = true;
					}
					else
						break;
				configsProj[wholeFile->first].erase(sectionToDelete);
			}
			while(!configsProj[wholeFile->first].empty()){
				section = configsProj[wholeFile->first].begin();
				string typePort = section->first;
				myFile<<"[["<<typePort<<"]]"<<endl;
				secondTime = false;
                for(section = configsProj[wholeFile->first].find(typePort); section != configsProj[wholeFile->first].end(); ++section)
					if(section->first == typePort){
						myFile<<section->second.first<<" "<<section->second.second<<endl;
						if(secondTime)
							configsProj[wholeFile->first].erase(sectionToDelete);
						sectionToDelete = section;
						secondTime = true;
					}
					else
						break;
				configsProj[wholeFile->first].erase(sectionToDelete);
			}
			myFile<<endl;
		}
	}
	myFile.close();
}

/*bool GenerateProjectFile::variableRepeated(string variableName) //spr czy zmienne powtarzaja sie poza zmiennymi globalnymi
{
	bool repeated = false;
	bool oneTime = false;
	map<string,vector<string>>::iterator it = var.begin();
	for(it; it != var.end(); ++it)
	{
		if(it->first != "global")
		{
            for(auto i=0; i < it->second.size(); ++i)
			{
				if(it->second[i] == variableName)
				{
					//vector <string>::iterator iv = it->second.begin()+i;
					if(oneTime)
					{
						repeated = true;
						return repeated;
					}
					oneTime = true;
				}
			}
		}
	}
	return repeated;
}*/


void GenerateProjectFile::outputsFoldersForExsistProj(string pathProj)
{
    CheckerFileProject chFileProject;
    unsigned int posSlash;
    string nameProj = "";
    replace(pathProj.begin(), pathProj.end(), '/', '\\');
    for(unsigned int i=pathProj.size(); i>0; i--)
    {
        if(pathProj[i] == '\\')
        {
            posSlash = i;
            nameProj.append(pathProj.begin()+int(posSlash+1), pathProj.end());//'nazwaProj'.prj
            break;
        }
    }
    if(nameProj == "")
        nameProj = pathProj;
    string pathOutput = "output";
    if(!chFileProject.exsistFolder(pathOutput))
        system("mkdir output");
    string nameProjWithoutExtension; //= nameProj;
    nameProjWithoutExtension.append(nameProj.begin(),nameProj.end()-4);
    string folderProj = pathOutput + "\\\"" + nameProjWithoutExtension + "\"";// output\'nazwaProj'
    string folderDevices = folderProj + "\\devices";
    if(!chFileProject.exsistFolder(folderProj))
    {
        string command = folderProj;
        command.insert(0, "mkdir ");
        string wholeCommand;
        wholeCommand = command;
        wholeCommand.append(" ");
        wholeCommand.append(folderDevices);
        system(wholeCommand.c_str());// mkdir output\'nazwaProj' output\'nazwaProj'\devices
    }
    else if (!chFileProject.exsistFolder(folderProj + "\\devices"))
    {
        string command;
        command.insert(0, "mkdir ");
        string wholeCommand;
        wholeCommand = command;
        wholeCommand.append(folderDevices);
        system(wholeCommand.c_str());// mkdir output\'nazwaProj' output\'nazwaProj'\devices
    }
}

void GenerateProjectFile::outputsFolders(const string &nameProj)
{
    string pathOutput = "output";
    if(!chFileProject.exsistFolder(pathOutput))
        system("mkdir output");
    //string nameProjWithoutExtension;
    //nameProjWithoutExtension.append(nameProj.begin(),nameProj.end()-4);
    string folderProj = pathOutput + "\\\"" + nameProj + '\"';//nameProjWithoutExtension;// output\"nazwaProj"
    pathProj = "output\\\\" + nameProj + "\\\\" + nameProj + ".prj";// output\"nazwaProj"\"nazwaProj".prj
    string command = folderProj;
    command.insert(0, "mkdir ");
    string wholeCommand = command;
    string folderDevices = folderProj + "\\devices";
    wholeCommand.append(" ");
    wholeCommand.append(folderDevices);
    if (!chFileProject.exsistFolder(folderProj + "\\devices"))
        system(wholeCommand.c_str());// mkdir output\"nazwaProj" output\"nazwaProj"\devices
    myFile.open("output\\" + nameProj + "\\" + nameProj + ".prj");
}

vector<string> GenerateProjectFile::sectionsFromTemplate(string templateName)
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

map<string,vector<string>> GenerateProjectFile::variablesFromTemplate(string templateName)
{
	ifstream f (templateName);
	if (!f.is_open())
	{
		throw runtime_error("Unable to open file: " + templateName);
	}
	map<string,vector<string>> templatesAndVar;
	string l;
	string sectionName;
	while(getline(f,l))
	{
		regex r("\\[[[:alpha:]\\-]+\\]"); //regex sections from template
		if (regex_match(l,r))
		{
			if(l != "[version]")
			{
				sectionName=l.substr(1,l.size()-2); //omit '[' and ']'
			}
		}
		//regex variables from template
		string varName;
		smatch c;
		regex x(".*(\\$[^ |^,]+).*");
		while (regex_match(l,c,x))
		{
			varName = c[1];
			size_t found = l.find(c[1]);
			if(found!=string::npos)
				l.erase(found,varName.size());
			varName = varName.substr(1,varName.size());
			if(varName != prefixVariables["port-number"])
			{
				if(varName.back() == 34)// usuwanie znaku "
					varName = varName.substr(0,varName.size()-1);
				string prefixFromTempVar;
				map<string,string>::iterator itPrefixVar;
				prefixFromTempVar = cutNameVar(varName);
                for(itPrefixVar = prefixVariables.begin(); itPrefixVar != prefixVariables.end(); ++itPrefixVar)//prefixy z funkcji setPrefixVariables()
				{
					if(prefixVariables["project"] == prefixFromTempVar)
					{
						if(!isStringInVector(templatesAndVar["project"],varName))//przeciw powtorzeniom
						{
							if(sectionName == "global" && (!isStringInVector(usedPortTypes,"global")))
							{
								if(!isStringInVector(usedProjectVariables,varName))
								{
									templatesAndVar["project"].push_back(varName);
									usedPortTypes.push_back("global");
								}
							}
							else if(isStringInVector(usedPortTypes,sectionName))
							{
								//if(!isStringInVector(usedProjectVariables,varName))
									templatesAndVar["project"].push_back(varName);
							}
						}
						break;
					}
					else if(prefixVariables["device"] == prefixFromTempVar)
					{
						if(!isStringInVector(templatesAndVar["device"],varName))
						{
							if(sectionName == "global" && (!isStringInVector(usedPortTypes,"global")))
							{
								usedPortTypes.push_back("global");
								templatesAndVar["device"].push_back(varName);
							}
							else if(isStringInVector(usedPortTypes,sectionName))
							{
								templatesAndVar["device"].push_back(varName);
							}
						}
						break;
					}
                    else if(prefixVariables["port"] == prefixFromTempVar) //&& isStringInVector(usedPortTypes,sectionName))
					{
						if(!isStringInVector(templatesAndVar[sectionName],varName))
						{
							templatesAndVar[sectionName].push_back(varName);
							break;
						}
						if(templatesAndVar[sectionName].empty())
						{
							map<string,vector<string>>::iterator iter = templatesAndVar.find("device");
							templatesAndVar.erase(iter);
						}
					}
				}
			}
		}
	}
	if(templatesAndVar["project"].empty())
	{
		map<string,vector<string>>::iterator iter = templatesAndVar.find("project");
		templatesAndVar.erase(iter);
	}
	if(templatesAndVar["device"].empty())
	{
		map<string,vector<string>>::iterator iter = templatesAndVar.find("device");
		templatesAndVar.erase(iter);
	}
	f.close();
	return templatesAndVar;
}

bool GenerateProjectFile::isStringInVector(vector<string> vec, const string& variable)
{
	vector<string>::iterator iterVector;
    for(iterVector = vec.begin(); iterVector != vec.end(); ++iterVector)
	{
		if(*iterVector == variable)
		{
			return true;
		}
	}
	return false;
}


string GenerateProjectFile::cutNameVar(string variable)//wydzielanie prefixu z nazwy pliku
{
    unsigned int posSlash;
	string prefix = " ";
	bool firstTime = false;
    for(unsigned int i = 0; i < variable.size(); ++i)
	{
		if(variable[i] == '_')
		{
			if(firstTime)
			{
				posSlash = i;
				prefix = "";
                prefix.append(variable.begin(), variable.begin()+int(posSlash));
				break;
			}
			firstTime = true;
		}
	}
	if(prefix != prefixVariables["project"] && prefix != prefixVariables["device"] && prefix != prefixVariables["port"])
        throw runtime_error("Variable not include declared prefixes.[GenerateProjectFile::cutNameVar] variable: " + variable);
	return prefix;
}

void GenerateProjectFile::setPrefixVariables()
{
	prefixVariables["project"] = "USER_PROJ";
	prefixVariables["device"] = "USER_SW";
	prefixVariables["port"] = "USER_PORT";
	prefixVariables["port-number"] = "P_PORT";
}

void GenerateProjectFile::initVariable(map<string,vector<string>>::iterator iteratorVariables)
{
	string userData;
    bool searchTypeInUsed = true;//= false;
//	if(!(iteratorVariables->first == "device" || iteratorVariables->first == "project"))
//	{
//		if(usedPortTypes.empty())
//			searchTypeInUsed = false;
//		else
//		{
//			vector<string>::iterator iterVector;
//			for(iterVector = usedPortTypes.begin(); iterVector != usedPortTypes.end(); ++iterVector)
//			{
//				if(*iterVector == iteratorVariables->first)
//				{
//					searchTypeInUsed = true;
//					break;
//				}
//				else
//					searchTypeInUsed = false;
//			}
//		}
//	}
    if(searchTypeInUsed || (iteratorVariables->first == "device" &&  !iteratorVariables->second.empty())|| (iteratorVariables->first == "project" &&  !iteratorVariables->second.empty()))
	{
		userData = " ";
		bool existUndeclaredVar = true;
		while(existUndeclaredVar)
		{
			if(iteratorVariables->second.empty())
			{
				existUndeclaredVar = false;
                //vector<string>::iterator iterVector;
//				for(iterVector = usedPortTypes.begin(); iterVector != usedPortTypes.end(); ++iterVector)
//					if(*iterVector == iteratorVariables->first)
//					{
//						usedPortTypes.erase(iterVector);
//						break;
//					}
			}
			else
			{
				string nameVariable = iteratorVariables->second.front();
				if(!(isStringInVector(usedSwitchVariables,nameVariable))&&!(iteratorVariables->first == "project" && isStringInVector(usedProjectVariables,nameVariable)))
				{
                    //for(unsigned int i=1; i < iteratorVariables->second.size(); ++i)//wyswietla bez pierwczego elementu, bo pierwszy wyswietlany i inicjowany jest nizej
						//cout<<"  "<<iteratorVariables->second[i]<<endl;
					if(iteratorVariables->first == "device")
					{
						usedSwitchVariables.push_back(nameVariable);
						if(nameVariable != variableSwitchName)
						{
							cout<<"Device "<<devicesNames.front()<<":"<<"global"<<":"<<commentVariable[nameVariable]<<"("<<nameVariable<<"):";
                            //cin>>userData;
						}
						else
						{
							userData = devicesNames.front();
						}
						nameVariable.append("=");
						declareProjectFileLine(nameVariable, userData, devicesNames.front(), iteratorVariables->first);
					}
					else if(iteratorVariables->first != "project")
					{
						cout<<"Device "<<devicesNames.front()<<":"<<iteratorVariables->first<<":"<<commentVariable[nameVariable]<<"("<<nameVariable<<"):";
                        //cin>>userData;
						nameVariable.append("=");
						declareProjectFileLine(nameVariable, userData, devicesNames.front(), iteratorVariables->first);
					}
					else
					{
						cout<<"Project:"<<nameOfProjFile<<":"<<commentVariable[nameVariable]<<"("<<nameVariable<<"):";
                        //cin>>userData;
						usedProjectVariables.push_back(nameVariable);
						nameVariable.append("=");
                        declareProjectFileLine(nameVariable, userData, "projectVar", "variables");
					}
					iteratorVariables->second.erase(iteratorVariables->second.begin());
					//usedVar[iteratorVariables->first].push_back(nameVariable);
				}
				else
					iteratorVariables->second.erase(iteratorVariables->second.begin());
			}
		}
	}
}


/*lista mozliwych do nadpisania zmiennych: globalne + portowe
 * else if(it->first != "global")
 * {
 * 		cout<<"Do you want to overwrite variable for particular number port?"<<endl;
 * 		cin>>
 *
 * }
 */

//if(iteratorVariables->second.empty())
//{
//	cout<<"There are no more variables to initialize in "<<iteratorVariables->first<<" section  Device: ["<<devicesNames.front()<<"]"<<endl<<endl;
	/*bool overwrite = false;
	vector<string>::iterator p;
	for(p = portTypesToOverwrite.begin(); p != portTypesToOverwrite.end(); ++p)
	{
		if(*p == iteratorVariables->first)
		{
			portTypesToOverwrite.erase(p);
			overwrite = true;
			break;
		}*/
	//}
	/*if(overwrite)
	{
		cout<<"Overwrite variable section"<<endl;
		map<int,vector<string>> portVarToOv; //mapa nr portow oraz jego zmiennych ktore mozna nadpisac
		map<string,vector<string>>::iterator b = usedVar.begin();
		for(itPortTypePortNumList = rangePort.begin(); itPortTypePortNumList != rangePort.end(); ++itPortTypePortNumList)
		{
			if(itPortTypePortNumList->first == iteratorVariables->first)
                for(b; b != usedVar.end(); ++b)
					if(b->first == iteratorVariables->first)
                        for(int i = 0; i< itPortTypePortNumList->second.size(); ++i)
							portVarToOv[itPortTypePortNumList->second[i]]=b->second;
		}
		bool existPortToOv = true;
		bool start = true;
		do
		{
			bool goodPort = false;
			string nrPortOv;
			int nrPort;
			bool exsistVarToOv = true;
			bool onlyOneNumPort = false;
			do
			{
				map<int,vector<string>>::iterator w;
				if(start)
				{
					if(portVarToOv.size() == 1)
						onlyOneNumPort = true;
					else
						onlyOneNumPort = false;
					start = false;
				}
				cout<<iteratorVariables->first<<" (used number of ports):"<<endl;
				w = portVarToOv.begin();
                for(w; w != portVarToOv.end(); ++w)
					cout<<" "<<w->first<<endl;
				if(!onlyOneNumPort)
				{
					cout<<"Enter port number to OVERWRITE:(no more enter 'n')"<<endl;
					cin>>nrPortOv;
					try
					{
						nrPort = stoi(nrPortOv);
					}
					catch (const invalid_argument& e)
					{
						exsistVarToOv = false;
						map<int,vector<string>>::iterator n = portVarToOv.find(nrPort);
						if(n != portVarToOv.end())
						portVarToOv.erase(n);
						if(portVarToOv.empty()) //jesli nie ma juz co nadpisywac NIGDZIE
							existPortToOv = false;
						break;
					}
					for(itPortTypePortNumList = rangePort.begin(); itPortTypePortNumList != rangePort.end(); ++itPortTypePortNumList)
					{
						if(itPortTypePortNumList->first == iteratorVariables->first)
                        for(w = portVarToOv.begin(); w != portVarToOv.end(); ++w)
							if(w->first == nrPort)
								goodPort = true;
					}
				}
				else
				{
					goodPort = true;
					w = portVarToOv.begin();
					nrPort = w->first;
					nrPortOv = to_string(nrPort);
				}
			}while(!goodPort);
			if(nrPortOv == "n")
			{
				userData = "n";
				break;
			}
			bool portTypeHasOneVar = false;
			if(portVarToOv[nrPort].size() == 1)
				portTypeHasOneVar = true;
			while(exsistVarToOv)
			{
				bool goodVarOv = false;
				do
				{
					vector<string> compareVarName = portVarToOv[nrPort];
					map<int,string> menu;
					if(!portTypeHasOneVar)
						cout<<"Possible variable to OVERWRITE on "<<iteratorVariables->first<<" nr:"<<nrPort<<"  Device: ["<<devicesNames.front()<<"] :"<<endl;
                    for(int s = 0; s < portVarToOv[nrPort].size(); ++s)
					{
						compareVarName = portVarToOv[nrPort];
						menu[s] = compareVarName[s];
						if(!portTypeHasOneVar)
							cout<<" "<<s<<". "<<compareVarName[s]<<endl;
						else
							cout<<" "<<compareVarName[s]<<endl;
					}
					int nr;
					if(!portTypeHasOneVar)
					{
						cout<<"Which variable do you want to OVERWRITE? (choose number / if no or no more enter 'n')"<<endl;
						string nrMenu;
						cin>>nrMenu;
						if(nrMenu != "n")
						{
							try
							{
								nr = stoi(nrMenu);
								userData = menu[nr];
							}
							catch (const invalid_argument& e)
							{
								userData = " ";
							}
						}
						else
							userData = "n";
					}
					else
					{
						menu[0] = compareVarName[0];
						userData = menu[0];
					}
					if(userData == "n")
					{
						exsistVarToOv = false;
						map<int,vector<string>>::iterator n = portVarToOv.find(nrPort);
						if(n != portVarToOv.end())
							portVarToOv.erase(n);
						if(portVarToOv.empty()) //jeslinie nie ma juz co nadpisywac NIGDZIE
							existPortToOv = false;
						break;
					}
                    for(int s = 0; s < portVarToOv[nrPort].size(); ++s)
					{
						compareVarName = portVarToOv[nrPort];
						if(userData == compareVarName[s])
							goodVarOv = true;
					}
				}while(!goodVarOv);
				if(userData == "n")
					break;
				string newNameVariable = "PORT_" + nrPortOv + "_" + userData + "= ";
				string newValue;
				cout<<"Enter "<<userData<<" variable value to OVERWRITE port nr="<< nrPort << ": "<<endl;
				cout<<userData<<" = ";
				cin>>newValue;
				newNameVariable += newValue;
				myFile<<newNameVariable<<endl;
				//portVarToOv[nrPort].pop_back();
				vector<string>::iterator vecIt = portVarToOv[nrPort].begin();
				for(vecIt; vecIt != portVarToOv[nrPort].end(); ++vecIt)
				{
					if(*vecIt == userData)
					{
						portVarToOv[nrPort].erase(vecIt);
						break;
					}
				}
				if(portVarToOv[nrPort].empty())
				{
					exsistVarToOv = false;
					map<int,vector<string>>::iterator n = portVarToOv.find(nrPort);
					if(n != portVarToOv.end())
						portVarToOv.erase(n);
				}
				else
					exsistVarToOv = true;
				if(portVarToOv.empty()) //jeslinie nie ma juz co nadpisywac NIGDZIE
					existPortToOv = false;
			}
		}while(existPortToOv);
	}
	else*/
//	userData = "n";//jesli nie nadpisujemy
//}
//}
//}

map<string,string> GenerateProjectFile::commentFromTemplate(string templateName)
{
	ifstream f (templateName);
	if (!f.is_open())
	{
		throw runtime_error("Unable to open file: " + templateName);
	}
	map<string,string> commentVar;
	string l;
	bool commentSection = false;
	while(getline(f,l))
	{
		regex r("\\[[[:alpha:]\\-]+\\]"); //regex sections from template
		if (regex_match(l,r))
		{
			if(l == "[comment]")
				commentSection = true;
		}
		string varName;
		smatch c;
		if (regex_match(l,c,regex(".*(\\$[^ ]+) *.*")) && commentSection)
		{
			varName = c[1];
            unsigned int sizeVar = varName.size();
			varName = varName.substr(1,varName.size());
			l = l.substr(sizeVar,l.size());
			commentVar[varName] = l;
		}
	}
	f.close();
	return commentVar;
}

map<string,string> GenerateProjectFile::commentPortFromTemplate(string templateName)
{
	ifstream f (templateName);
	if (!f.is_open())
	{
		throw runtime_error("Unable to open file: " + templateName);
	}
	map<string,string> commentPort;
	string l;
	bool commentSection = false;
	while(getline(f,l))
	{
		regex r("\\[[[:alpha:]\\-]+\\]"); //regex sections from template
		if (regex_match(l,r))
		{
			if(l == "[comment-port]")
				commentSection = true;
		}
		string portName;
		smatch c;
		if (regex_match(l,c,regex(".*(\\port-[^ ]+) *.*")) && commentSection)
		{
			portName = c[1];
            unsigned int sizePort = portName.size();
			l = l.substr(sizePort,l.size());
			commentPort[portName] = l;
		}
	}
	f.close();
	return commentPort;
}


string GenerateProjectFile::templateInput()
{
	string userData;
	string templatePath;
	bool argTemp = false;
	if(tempFile != " ")
	{
		tempFile = checkExtensionFile(tempFile, ".cfg");
		templatePath += tempFile;
		if(!chFileProject.chExistenceTemplate(templatePath))
		{
			cout<<"Failed opening template from -t terminal argument"<<endl;
		}
		else
			argTemp = true;
	}
	if(argTemp == false)
	{
		map<int,string> menuTemplate;
		cout<<"Searching 'templates' folder..."<<endl;
        //menuTemplate = chFileProject.showListOfTemplates();
//		if(menuTemplate.empty())
//		{
//			cout<<"Not found"<<endl;
//			do
//			{
//				templatePath = "";
//				cout<<"Enter path and name to template file:"<<endl;
//				cin>>userData;
//				userData = checkExtensionFile(userData, ".cfg");
//				templatePath += userData;
//			}while(!chFileProject.chExistenceTemplate(templatePath));
//		}
//		else
//		{
//			do
//			{
//				templatePath = "templates/";
//				cout<<"Enter number of template file:";
//				unsigned int chooseTem;
//				cin >>userData;
//				try{
//					chooseTem = stoi(userData);
//					isNumber = true;
//				}
//				catch (const invalid_argument& e)
//				{
//					isNumber = false;
//				}
//				if(chooseTem > menuTemplate.size())
//					isNumber = false;
//				templatePath += menuTemplate[chooseTem];
//			}while(!chFileProject.chExistenceTemplate(templatePath) || !isNumber);
//		}
    }
	return templatePath;
}


void GenerateProjectFile::createRanges(const string& templatePath)
{
	map<string,vector<int>> rangePort; //mapa na typ portu -> vector int porty
	map<string,vector<string>> rangePortSlots;
	vector<int> allUsedPorts;
	vector<string> allUsedPortSlots;
	map<string,string> ranges;
	vector<string> sections = sectionsFromTemplate(templatePath);
	string text;
	vector<string>::iterator itSection;
	bool reset = false;
	do
	{
		reset = false;
		for(itSection = sections.begin(); itSection != sections.end(); ++itSection)
		{
			if(reset)
				break;
			bool uncorrectInputRange = false;
			do
			{
				uncorrectInputRange = false;
				string actualSection = *itSection;
				if(actualSection.compare(0,4,"port")==0)//if start from 'port-..
				{
					bool uncorrectRangeFormat=false;
					do
					{
						cout<<"Enter ranges of  "<< "Device "<< devicesNames.front() << ": " <<*itSection<< " (" << commentPort[*itSection] << ") (no 'n'/reset all 'r'):";
						cin>>userData;
						uncorrectRangeFormat = false;
						try{
								if(userData == "r")
								{
									reset = true;
									allUsedPorts.clear();
									allUsedPortSlots.clear();
									rangePort.clear();
									rangePortSlots.clear();
									usedPortTypes.clear();
									//portTypes.clear();
									ranges.clear();
									break;
								}
								else if(userData != "n")
								{
                                    RangeConverter rConvert(userData,1, "output/nazwa_proj/nazwa_proj.prj");
									uncorrectInputRange = true;
									vector<int>::iterator itNumersPort;
                                    for(itNumersPort=rConvert.getNumbers().begin(); itNumersPort!=rConvert.getNumbers().end(); ++itNumersPort)
									{
										rangePort[*itSection].push_back(*itNumersPort);
										allUsedPorts.push_back(*itNumersPort);
										uncorrectInputRange = false;
									}
									if(rConvert.getNumbers().empty())
									{
										vector<string>::iterator itNumersPortSlots;
                                        for(itNumersPortSlots=rConvert.getNames().begin(); itNumersPortSlots!=rConvert.getNames().end(); ++itNumersPortSlots)
										{
											rangePortSlots[*itSection].push_back(*itNumersPortSlots);
											allUsedPortSlots.push_back(*itNumersPortSlots);
											uncorrectInputRange = false;
										}
									}
									//spr zakresow - czy port nr .. jest zadeklarowany 2 razy lub wiecej
									//dla slotow
									map<int,int> countMap;
									map<int,int>::iterator e;
									vector <int>::iterator i = allUsedPorts.begin();
                                    for(; i != allUsedPorts.end(); ++i)
									{
										e = countMap.find(*i);
										if(e != countMap.end())
                                            ++e->second;
										else
											countMap[*i]=1;
									}
									e=countMap.begin();
                                    for(; e != countMap.end(); ++e)
									{
										if(e->second > 1)
										{
											string text = to_string(e->first);
											uncorrectInputRange = true;
											cout<<"Redundant declaration of port "<<text<<endl;
                                            for(unsigned int i = 0; i < rangePort[*itSection].size(); ++i)
											{
												allUsedPorts.pop_back();
											}
											rangePort[*itSection].clear();
											cout<<"Redeclare range of ports again..."<<endl;
										}
									}
									//sprawdzanie powtorzen dla portow i slotow
									map<string,int> countMap2;
									map<string,int>::iterator e2;
									vector <string>::iterator i2 = allUsedPortSlots.begin();
                                    for(; i2 != allUsedPortSlots.end(); ++i2)
									{
										e2 = countMap2.find(*i2);
										if(e2 != countMap2.end())
                                            ++e2->second;
										else
											countMap2[*i2]=1;
									}
									e2=countMap2.begin();
                                    for(; e2 != countMap2.end(); ++e2)
									{
										if(e2->second > 1)
										{
											string text = e2->first;
											uncorrectInputRange = true;
											cout<<"Redundant declaration of port "<<text<<endl;
                                            for(unsigned int i = 0; i < rangePortSlots[*itSection].size(); ++i)
											{
												allUsedPortSlots.pop_back();
											}
											rangePortSlots[*itSection].clear();
											cout<<"Redeclare range of ports again..."<<endl;
										}
									}
									if(!uncorrectInputRange)
									{
										//portTypes.push_back(*itSection);
										usedPortTypes.push_back(*itSection);
										text += userData;
										ranges["number of ports of type "+ *itSection + ":"] = userData;
									}
								}
							}
                            catch(...){
								uncorrectRangeFormat = true;
							}
					}while(uncorrectRangeFormat);
				}
				else
				{
					//toDelete.push_back(itSection);//?
				}
			}while(uncorrectInputRange);
		}
		//info o niezadeklarowanych nr portow
		/*if(!reset)
		{
			if(allPossibleNumber.empty())
			{
				cout<<"You used ALL ports."<<endl;
			}
			else
			{
				cout<<"You do not use ALL ports for device: ["<<devicesNames.front()<<"]. Undeclared ports: ";
                for(unsigned int i = 0; i < allPossibleNumber.size(); ++i)
					cout<<allPossibleNumber[i]<<" ";
				cout<<endl;
				cout<<"Reset ranges of ports? (reset enter'r'/ no 'n'):";
				cin>>userData;
				if(userData == "r")
				{
					RangeConverter rConvert(allPorts);
					allPossibleNumber = rConvert.getNumbers();
					reset = true;
					allUsedPorts.clear();
					rangePort.clear();
					//portTypes.clear();
					ranges.clear();
				}
				userData = " ";
			}
		}
		if(rangePort.empty())
		{
			cout<<"You did not declared any port!"<<endl;
			reset = true;
		}*/
	}while(reset);
	map<string,string>::iterator itRanges;
	for(auto i : sections)
	{
        for(itRanges = ranges.begin(); itRanges != ranges.end(); ++itRanges)
		{
			if(("number of ports of type "+ i + ":") == itRanges->first)
				declareProjectFileLine(itRanges->first, itRanges->second, devicesNames.front(), "ranges");
		}
	}
	//show zadeklarowane numery portow do typow portu
	/*cout<<endl<<"Declared nr ports:"<<endl;
	cout<<"Device:["<<devicesNames.front()<<"]"<<endl;
	map<string,vector<int>>::iterator itPortTypePortNumList;
	for(itPortTypePortNumList = rangePort.begin(); itPortTypePortNumList != rangePort.end(); ++itPortTypePortNumList)
	{
		cout<<" "<<itPortTypePortNumList->first<<endl;
        for(unsigned int i = 0; i< itPortTypePortNumList->second.size(); ++i)
			cout<<"  "<<itPortTypePortNumList->second[i]<<endl;
	}cout<<endl;*/
}

void GenerateProjectFile::declareProjectFileLine(const string& arg1, const string& arg2, const string& arg3, const string& arg4)
{
	pair <string,string> pairPom;
	pairPom.first = arg1;
    pairPom.second = arg2;
    configsProj[arg3].insert(pair <string, pair<string,string>> (arg4,pairPom));
}

void GenerateProjectFile::declareEditedProjectFileLine(const string& arg1, const string& arg2, const string& arg3, const string& arg4)
{
    pair <string,string> pairPom;
    pairPom.first = arg1;
    pairPom.second = arg2;
    //auto z = configsProj[arg3].find(arg4);//map<string,multimap<string,pair<string,string>>> configsProj;
//    configsProj.erase(arg3);
//    auto z = configsProj.find(arg3);
//    if(z != configsProj.end())
//    {
//        for(multimap<string,pair<string,string>>::iterator x = configsProj[arg3].begin(); x != configsProj[arg3].end(); ++x)
//        {
//            cout<<x->first<<" :::::::: "<<x->second.first<<" : "<<x->second.second<<endl;
//            if(x->second.first == arg1)
//            {
//                multimap<string,pair<string,string>> temp = &x;
//                x.erase(arg1);
//                cout<<x->first<<" IIIIIIIIIIIII "<<x->second.first<<" : "<<x->second.second<<endl;
//                auto it = configsProj[arg3].find(x->first);
//                configsProj[arg3].erase(it);
//            }
//        }
//    }
    auto z = configsProj[arg3].find(arg4);
    if(z != configsProj[arg3].end())
        configsProj[arg3].erase(z);
    configsProj[arg3].insert(pair <string, pair<string,string>> (arg4,pairPom));
}

void GenerateProjectFile::clearAndSaveProjVar()
{
    auto z = configsProj.find("projectVar");
    if(z != configsProj.end())
    {
        for(multimap<string,pair<string,string>>::iterator x = configsProj["projectVar"].begin(); x != configsProj["projectVar"].end(); ++x)
        {
            pair <string,string> pairPom;
            pairPom.first = x->second.first;
            pairPom.second = x->second.second;
            bool exsist = false;
            for(auto y = projVar.begin(); y != projVar.end(); ++y)
                if(y->first == x->second.first)
                {
                    projVar.erase(y);
                    projVar.push_back(pairPom);
                    exsist = true;
                    break;
                }
                else
                    exsist = false;
            if(!exsist)
                projVar.push_back(pairPom);
        }
    }
    configsProj.erase("projectVar");
}

void GenerateProjectFile::declareProjectVar(const string& varName, const string& varValue)
{
    bool exist = false;
     for(auto x = projVar.begin(); x != projVar.end(); ++x)
     {
         if(x->first == varName)
         {
             projVar.erase(x);
             pair <string,string> pairPom;
             pairPom.first = varName;
             pairPom.second = varValue;
             projVar.push_back(pairPom);
             exist = true;
             break;
         }
     }
     if(!exist)
     {
         pair <string,string> pairPom;
         pairPom.first = varName;
         pairPom.second = varValue;
         projVar.push_back(pairPom);
     }
}

void GenerateProjectFile::copyDevConfigToOtherDev(string copyFrom, string copyTo)
{
    configsProj[copyTo].swap(configsProj[copyFrom]);
}

bool GenerateProjectFile::isDevProjectFileLine(string arg1, const string &arg2, const string &arg3)
{
    for(auto x = configsProj[arg1].begin(); x != configsProj[arg1].end(); ++x)
    {
        if(x->first == arg2)
            if(x->second.first == arg3)
                return true;
    }
    return false;
}

void GenerateProjectFile::eraseDevProjectFileLine(string arg1)
{
    auto iteratorEraseDev = configsProj.find(arg1);
    if(iteratorEraseDev != configsProj.end())
        configsProj.erase(iteratorEraseDev);
}

void GenerateProjectFile::findAndSubtituteDevName(string oldName, string newName)
{
    string oldDevList;
    auto s = configsProj["project"].find("secondLine");
    if(s != configsProj["project"].end())
    {
        oldDevList = s->second.second;
        configsProj["project"].erase(s);
    }

    size_t start_pos = oldDevList.find(oldName);
    if(start_pos != std::string::npos)
        oldDevList.replace(start_pos, oldName.length(), newName);
    pair <string,string> pairPom;
    pairPom.first = "names:";
    pairPom.second = oldDevList;
    configsProj["project"].insert(pair <string, pair<string,string>> ("secondLine",pairPom));
}

void GenerateProjectFile::findAndDeleteDevName(string devName)
{
    //changing device list in config structure
    string oldDevList;
    auto s = configsProj["project"].find("secondLine");
    if(s != configsProj["project"].end())
    {
        oldDevList = s->second.second;
        configsProj["project"].erase(s);
    }
    string partI,partII;
    size_t start_pos = oldDevList.find(devName);
    if(start_pos != std::string::npos)
    {
        partI = oldDevList.substr(0, start_pos);
        partII = oldDevList.substr(start_pos + devName.size());
        if(!partI.empty())//(partI[partI.size()-2] == ',')
            partI = partI.substr(0,partI.size()-2);
        else if(!partII.empty())
        {
           if(partII[0] == ',')
               partII = partII.substr(1);
           if(partII[0] == ' ')
               partII = partII.substr(1);
        }
        oldDevList = partI + partII;
    }
    pair <string,string> pairPom;
    pairPom.first = "names:";
    pairPom.second = oldDevList;
    configsProj["project"].insert(pair <string, pair<string,string>> ("secondLine",pairPom));

    //changing device amount in config structure
    string amount;
    int amountDev;
    s = configsProj["project"].find("firstLine");
    if(s != configsProj["project"].end())
    {
        amount = s->second.second;
        amountDev = stoi(amount);
        amountDev--;
        amount = to_string(amountDev);
        configsProj["project"].erase(s);
        declareProjectFileLine("number of devices:", amount, "project", "firstLine");
    }
}

void GenerateProjectFile::isTemplateFile()
{
	tempFile = checkExtensionFile(tempFile, ".cfg");
	if(!chFileProject.chExistenceTemplate(tempFile))
	{
		throw invalid_argument("Failed opening template from -t terminal argument");
	}
}

void GenerateProjectFile::loadConfigsProj(const map<string,multimap<string,pair<string,string>>> &newConfig)
{
    configsProj = newConfig;
}

vector<string> GenerateProjectFile::getDevFromConfigFile()
{
    vector<string> listaDev;
    string devList;
    auto s = configsProj["project"].find("secondLine");
    if(s != configsProj["project"].end())
    {
        devList = s->second.second;
    }

    size_t x = devList.find(",");
    size_t pos = devList.find_last_of(",");
    if(x != std::string::npos)
    {
        while(x != std::string::npos)
        {
            string dev = devList.substr(pos+2);//(0,pos);
            listaDev.push_back(dev);
            devList = devList.substr(0,pos);//(pos+1);
            pos = devList.find_last_of(",");
            x=devList.find(",");
        }
    }
    if(devList[0] == ' ')
        listaDev.push_back(devList.substr(1));
    else
        listaDev.push_back(devList);
    return listaDev;
}

void GenerateProjectFile::addDevNameToConfigFile(const string &newDev)
{
    string devList;
    auto s = configsProj["project"].find("secondLine");
    if(s != configsProj["project"].end())
    {
        devList = s->second.second;
    }
    if(!devList.empty())
        devList += ", " + newDev;
    else
        devList += newDev;
    pair <string,string> pairPom;
    pairPom.first = "names:";
    pairPom.second = devList;
    auto w = configsProj["project"].find("secondLine");
    if(w != configsProj["project"].end())
        configsProj["project"].erase(w);
    configsProj["project"].insert(pair <string, pair<string,string>> ("secondLine",pairPom));
}

void GenerateProjectFile::incrementDevAmountToConfigFile()
{
    string amount;
    int amountDev;
    auto s = configsProj["project"].find("firstLine");
    if(s != configsProj["project"].end())
    {
        amount = s->second.second;
        amountDev = stoi(amount);
        ++amountDev;
        amount = to_string(amountDev);
        configsProj["project"].erase(s);
        declareProjectFileLine("number of devices:", amount, "project", "firstLine");
    }
}

map<string,multimap<string,pair<string,string>>> GenerateProjectFile::getConfigsProj()
{
    return configsProj;
}

void GenerateProjectFile::clearConfigsProj()
{
    configsProj.clear();
}

string GenerateProjectFile::getMaxSlotsPerPort(string device)
{
    map<string,multimap<string,pair<string,string>>>::iterator wholeFile;
    multimap<string,pair<string,string>>::iterator section;
    wholeFile = configsProj.find(device);
    if(wholeFile != configsProj.end())
    {
        section = configsProj[wholeFile->first].find("maxSlots");
        if(section != configsProj[wholeFile->first].end())
            return section->second.second;
        else
            return "24";
    }
    else
        return "24";
}
