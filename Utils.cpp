/*
 * Utils.cpp
 *
 *  Created on: 8 Jul 2019
 *      Author: pp
 */

#include <iostream>
#include <regex>
#include <vector>
#include <string>
#include "Utils.h"
namespace pputils
{
	using namespace std;
	string trim(string& input,int option)
	{
		std::string rv=input;
		if (option & TRIM_LEADING)
		{
			rv=std::regex_replace(rv,std::regex("^ +"), "");
		}
		if (option & TRIM_TRAILING)
		{
			rv=std::regex_replace(rv,std::regex(" +$"),"");
		}

		return rv;
	}

	vector<std::string> tokenize(std::string &inputString,std::string regexExpAsDelimiters)
	{
		vector<string> tokens;

		regex delimit(regexExpAsDelimiters);

		auto wordsBegin=sregex_iterator(inputString.begin(),inputString.end(),delimit);
		auto wordsEnd  =sregex_iterator();

		//cout << "xpp::t:input " << inputString << "(" << regexExpAsDelimiters << ")" << "\n";

		for (sregex_iterator i = wordsBegin; i!= wordsEnd; ++i)
		{
			//cout << "xpp::t: " << (*i).str() << "\n";
			tokens.push_back((*i).str());
		}

		return tokens;

	}


	bool isThisAComment(std::string& input)
	{
		if (input.at(0)=='#') return true;
		return false;
	}

	std::string replace(std::string& in, std::string from, std::string to)
	{
		string::size_type pos=0u;
		string result=in;
		while ((pos=result.find(from,pos)) != string::npos)
		{
			result.replace(pos,from.length(),to);
			pos+=from.length();
		}

		return result;
	}
}
