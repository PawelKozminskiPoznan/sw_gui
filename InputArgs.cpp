/*
 * InputArgs.cpp
 *
 *  Created on: Jul 6, 2020
 *      Author: pawelkoz
 */

#include "InputArgs.h"
#include <iostream>
#include <bitset>

using namespace std;

InputArgs::InputArgs() {
	// TODO Auto-generated constructor stub
}

InputArgs::InputArgs(int argc, char ** argv) {
	this->argc = argc;
	this->argv = argv;
}

InputArgs::~InputArgs() {
	// TODO Auto-generated destructor stub
}

short InputArgs::start()
{
	string actual;
	string prev = "";
	auto result = 0;
	bool commandR = false;
	bool commandOT = false;
	bool commandHelp = false;
	for(int i = 1; i < argc; i++)
	{
		auto c = 0;
		actual = argv[i];
		if((actual == "r" && prev == "-") || actual == "-r")
		{
			c=1;
			commandR = true;
			if(commandOT)
				throw invalid_argument("Error - command '-r' no match with others");
		}
		else if((actual == "o" && prev == "-") || actual == "-o")
		{
			c=2;
			commandOT = true;
			if(commandR)
				throw invalid_argument("Error - command '-r' no match with others");
		}
		else if((actual == "t" && prev == "-") || actual == "-t")
		{
			c=3;
			commandOT = true;
			if(commandR)
				throw invalid_argument("Error - command '-r' no match with others");
		}
		else if((actual == "h" && prev == "-") || actual == "-h")
		{
			c=4;
			commandHelp = true;
			if(commandR || commandOT)
				throw invalid_argument("Error - command '-h' no match with others");
		}
		string pom;
		if(c == 1 || c == 2 || c == 3)
		{
			if(i <= argc -2)
			pom = argv[i+1];
			else if(i != 0)
			{
				throw invalid_argument("Error - lack of argument");
			}
		}
		else if(!(prev=="r"|| prev=="o"||prev=="t"||prev=="-r"||prev=="-o"||prev=="-t"||prev=="-h"||prev=="h")&&c!=4)
		{
			c=5;
		}
		switch(c)
		{
			case 1:
				if(pom != "-" && pom != "-t" && pom != "-o")
				{
					argumentR = pom;
					result = result || 1;
				}
				else
					throw invalid_argument("Error invalid argument");
				break;
			case 2:
				if(pom != "-" && pom != "-t" && pom != "-r")
				{
					argumentO = pom;
					result = result || 2;
				}
				else
					throw invalid_argument("Error invalid argument");
				break;
			case 3:
				if(pom != "-" && pom != "-r" && pom != "-o")
				{
					argumentT = pom;
					result = result || 4;
				}
				else
					throw invalid_argument("Error invalid argument");
				break;
			case 4:
				return result = result || 8;
			case 5:
				throw invalid_argument("Error invalid argument");
		}
		prev = actual;
	}
	argType = result;
	return result;
}

string InputArgs::getArgumentR()
{
	return argumentR;
}
string InputArgs::getArgumentT()
{
	return argumentT;
}
string InputArgs::getArgumentO()
{
	return argumentO;
}

string InputArgs::getArgument()
{
	switch(argType)
	{
	case 1:
		return argumentR;
	case 2:
		return argumentO;
	case 4:
		return argumentT;
	}
}
