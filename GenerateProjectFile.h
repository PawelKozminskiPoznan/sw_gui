/*
 * GenerateProjectFile.h
 *
 *  Created on: Jun 15, 2020
 *      Author: pawelkoz
 */

#ifndef GENERATEPROJECTFILE_H_
#define GENERATEPROJECTFILE_H_
#include <string>
#include "Project.h"
#include <list>
#include <queue>

using namespace std;

class GenerateProjectFile {
public:
	GenerateProjectFile();
    GenerateProjectFile(const string& argumentT, const string& argumentO);
    GenerateProjectFile(short inputType, const string& argument);
	virtual ~GenerateProjectFile();
	std::string getFileName();
	Project* getProject();
	string userData;
    void static outputsFoldersForExsistProj(string);
    void createRanges(const string& templatePath);
    vector<string> sectionsFromTemplate(string templateName);
    map<string,string> commentVariable;
    map<string,string> commentPort;
    map<string,string> commentFromTemplate(string templateName);
    map<string,string> commentPortFromTemplate(string templateName);
    void declareProjectFileLine(const string&, const string&, const string&, const string&);
    void declareEditedProjectFileLine(const string&, const string&, const string&, const string&);
    void outputsFolders(const string&);
    queue <string> devicesNames;
    map<string,vector<string>> var;
    map<string,vector<string>> variablesFromTemplate(string templateName);
    void setPrefixVariables();
    string variableSwitchName; //nazwa zmiennej okreslajacej nazwe urzadzenia w funkcji 'createProjectFile'
    vector<string> usedSwitchVariables;
    vector<string> usedPortTypes;
    bool isStringInVector(vector<string> vec, const string& var);
    vector<string> usedProjectVariables;
    void writeToProjectFile();
    string pathProj;
    void loadConfigsProj(const map<string,multimap<string,pair<string,string>>>&);
    map<string,multimap<string,pair<string,string>>> getConfigsProj();
    void eraseDevProjectFileLine(string arg1);
    void copyDevConfigToOtherDev(string copyFrom, string copyTo);
    bool isDevProjectFileLine(string arg1, const string &arg2, const string &arg3);
    void findAndSubtituteDevName(string oldName, string newName);
    void findAndDeleteDevName(string devName);
    vector<string> getDevFromConfigFile();
    void addDevNameToConfigFile();
    void incrementDevAmountToConfigFile();
    void addDevNameToConfigFile(const string &newDev);
    void clearAndSaveProjVar();
    void declareProjectVar(const string& varName, const string& varValue);
    void clearConfigsProj();
    bool newDev;
    bool newDevRange;
    bool newDevProjVar;
    bool newDevVar;
    bool newPortVar;
    bool newProjectConfig;
    bool newDevDevVar;
    bool newDevPortVar;
    string originProjectName;
    map<string,int> portLimitations;
    bool editedProj;
    string getMaxSlotsPerPort(string);
private:
	Project* proj;
	ofstream myFile;
	CheckerFileProject chFileProject;
	string selectionVariable;
	string nameOfProjFile;
	string tempFile;
	map<string,string> prefixVariables;
	map<string,multimap<string,pair<string,string>>> configsProj;
	map<string,stringstream> configs;
	std::string checkExtensionFile(string name, string extention);
    void createProjectFile(const string &NameFile);
	//bool variableRepeated(string variableName);
	string cutNameVar(string var);
	void initVariable(map<string,vector<string>>::iterator iteratorVariables);
	string templateInput();
    void isTemplateFile();
    vector<pair<string,string>> projVar;
};

#endif /* GENERATEPROJECTFILE_H_ */
