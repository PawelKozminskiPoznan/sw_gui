/*
 * RangeConverter.cpp
 *
 *  Created on: 3 Jul 2019
 *      Author: pp
 */

#include "RangeConverter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <string>
#include <algorithm>

using namespace std;

RangeConverter::RangeConverter() {
	// TODO Auto-generated constructor stub
    comunicate = " ";

}

vector<string> RangeConverter::tokenize(string &inputString,string regexExpAsDelimiters)
{

	vector<string> tokens;

	regex delimit(regexExpAsDelimiters);

	auto wordsBegin=sregex_iterator(inputString.begin(),inputString.end(),delimit);
	auto wordsEnd  =sregex_iterator();

	for (sregex_iterator i = wordsBegin; i!= wordsEnd; ++i)
	{
		tokens.push_back((*i).str());
	}

	return tokens;

}

static vector<string> convertRange(string prefix,pair<int,int> min,pair<int,int> max, int maxPortsPerSlot,int firstPortNumberInSlot,bool show)
{
	vector<string> vals;

	if (max<min)
	{
		auto x=max;
		max=min;
		min=x;
		auto y=maxPortsPerSlot;
		maxPortsPerSlot = firstPortNumberInSlot;
		firstPortNumberInSlot = y;
	}

	for (int l=min.first,r=firstPortNumberInSlot;l<=max.first;l++)
	{
		if(r>maxPortsPerSlot)
			if(show)
            {
             cout<<"Numer slotu "<<min.first<<"/"<<min.second<<" wiekszy niz maksymalna liczba slotow = "<<maxPortsPerSlot<<" !"<<endl;

            }
		for(;r<=maxPortsPerSlot;r++)
		{
			//cout << l << "/" << r << endl;
			ostringstream oss;
			oss<<l<<"/"<<r;
			vals.push_back(oss.str());
		}
		r=firstPortNumberInSlot;
	}
	//for (int l=firstPortNumberInSlot;l<=max.second;l++)
	/*for (int l=min.second;l<=max.second;l++)
	{
		//cout << max.first << "/" << l <<endl;
		ostringstream oss;
		oss<<max.first << "/" << l;
		vals.push_back(oss.str());
	}*/

	return vals;
}

void RangeConverter::convertExtended(std::string& token)
{
    static int maxPortsPerSlot=4;
	static int firstPortsinSlotNumer=1;
	pair<int,int> min(0,0);
	pair<int,int> max(0,0);

	vector<string> toRemoveValues;
	size_t found = token.find('M');
	if (found!=string::npos)
	{
		string temp = "";
        for(unsigned int i=found+1; i<token.size(); i++)
			temp += token[i];
		maxPortsPerSlot=stoi(temp);
	}
	found = token.find('m');
	if (found!=string::npos)
	{
		string temp = "";
        for(unsigned int i=found+1; i<token.size(); i++)
			temp += token[i];
		firstPortsinSlotNumer=stoi(temp);
	}
	if (4==sscanf(token.c_str(),"%d/%d-%d/%d",&min.first,&min.second,&max.first,&max.second))
	{
		for(int i = min.first; i<=max.first; i++)
		{
			int firstSlot = firstPortsinSlotNumer;
			int maxSlot = maxPortsPerSlot;
			pair<int,int> one,two;
			one.first = i;
			two.first = i;
			if(i==min.first && min.second > firstPortsinSlotNumer)
			{
				one.second = min.second;
				firstSlot = min.second;
			}
			else
			{
				one.second = firstPortsinSlotNumer;
				if(min.second < firstSlot && i==min.first)
				{
					if(show)
                    {
						cout<<"Number of slot "<<min.first<<"/"<<min.second<<" is smaller than default = "<<firstSlot<<" and will not be used"<<endl;
                        comunicate = "Numer slotu " + to_string(min.first) + "/" + to_string(min.second) + " mniejszy niz maksymalna liczba slotow = " + to_string(firstSlot)  + " !";
                    }
                    size_t found = token.find(to_string(i)+"/"+to_string(min.second));
					if(found != string::npos)
					{
                        unsigned int sizeOfBadValue = (to_string(min.first)).size();
						sizeOfBadValue += (to_string(min.second)).size()+1;
						token.replace(found,sizeOfBadValue,(to_string(i)+"/"+to_string(firstSlot)));
					}
				}
			}
			bool tooBig = false;
			if(one.second > maxPortsPerSlot)
			{
				if(show)
                {
					cout<<"Number of slot "<<one.first<<"/"<<one.second<<" is bigger than default = "<<maxSlot<<" and will not be used"<<endl;
                    comunicate = "Numer slotu " + to_string(one.first) + "/" + to_string(one.second) + " wiekszy niz maksymalna liczba slotow = " + to_string(maxSlot)  + " !";
                }
                tooBig = true;
			}
			if(max.second < firstSlot && i==max.first)
			{
                if(show)
                {
                    cout<<"Number of slot "<<max.first<<"/"<<max.second<<" is smaller than default = "<<firstSlot<<" and will not be used"<<endl;
                    comunicate = "Numer slotu " + to_string(max.first) + "/" + to_string(max.second) + " mniejszy niz maksymalna liczba slotow = " + to_string(firstSlot)  + " !";
                }
			}
			else if(!tooBig)
			{
				if(i==max.first && max.second < maxPortsPerSlot && max.second  > firstPortsinSlotNumer)
				{
					two.second = max.second;
					maxSlot = max.second;
				}
				else
				{
					two.second = maxSlot;
					if(max.second > maxSlot && i==max.first)
					{
                        if(show)
                        {
                            cout<<"Number of slot "<<max.first<<"/"<<max.second<<" is bigger than default = "<<maxSlot<<" and will not be used"<<endl;
                            comunicate = "Numer slotu " + to_string(max.first) + "/" + to_string(max.second) + " wiekszy niz maksymalna liczba slotow = " + to_string(maxSlot)  + " !";
                        }
						size_t found = token.find(to_string(max.second));
						if(found != string::npos)
						{
                            unsigned int sizeOfBadValue = (to_string(max.second)).size();
							token.replace(found+1,sizeOfBadValue,to_string(maxSlot));
						}
					}
				}
				vector<string> r=convertRange("",one,two,maxSlot,firstSlot,show);
				names.insert(names.end(),r.begin(),r.end());
			}
		}
	}
	else if (4==sscanf(token.c_str(),"!%d/%d-%d/%d",&min.first,&min.second,&max.first,&max.second))
	{
		for(int i = min.first; i<=max.first; i++)
		{
			int firstSlot = firstPortsinSlotNumer;
			int maxSlot = maxPortsPerSlot;
			pair<int,int> one,two;
			one.first = i;
			two.first = i;
			if(i==min.first && min.second > firstPortsinSlotNumer)
			{
				one.second = min.second;
				firstSlot = min.second;
			}
			else
				one.second = firstPortsinSlotNumer;
			if(i==max.first && max.second < maxPortsPerSlot)
			{
				two.second = max.second;
				maxSlot = max.second;
			}
			else
				two.second = maxSlot;
			vector<string> r=convertRange("",one,two,maxSlot,firstSlot,show);
			toRemoveValues.insert(toRemoveValues.end(),r.begin(),r.end());
		}
	}
	else if (2==sscanf(token.c_str(),"%d/%d",&min.first,&min.second))
	{
		string temp;
		if(min.second > maxPortsPerSlot)
		{
            if(show)
            {
                cout<<"Number of slot "<<min.first<<"/"<<min.second<<" is bigger than default = "<<maxPortsPerSlot<<" and will not be used"<<endl;
                comunicate = "Numer slotu " + to_string(max.first) + "/" + to_string(max.second) + " wiekszy niz maksymalna liczba slotow = " + to_string(maxPortsPerSlot)  + " !";
            }
		}
		else if(min.second < firstPortsinSlotNumer)
		{
            if(show)
            {
                cout<<"Number of slot "<<min.first<<"/"<<min.second<<" is smaller than default = "<<firstPortsinSlotNumer<<" and will not be used"<<endl;
                comunicate = "Numer slotu " + to_string(max.first) + "/" + to_string(max.second) + " mniejszy niz maksymalna liczba slotow = " + to_string(maxPortsPerSlot)  + " !";
            }
		}
		else
		{
			temp = to_string(min.first)+"/"+to_string(min.second);
			names.push_back(temp);
		}
	}
	else if (2==sscanf(token.c_str(),"!%d/%d",&min.first,&min.second))
	{
		string portToRemove = to_string(min.first)+"/"+to_string(min.second);
		toRemoveValues.push_back(portToRemove);
	}

	if (toRemoveValues.size())
	{
		for (auto v: toRemoveValues)
		{
			auto it=find(names.begin(),names.end(),v);
			if (it!=names.end())
				names.erase(it);
		}
	}

}

void RangeConverter::convert() // nie zwraca boola
{

    logMsg("");
    logMsg("+++++++++Convert string to range of int+++++++++");
    logMsg("Input string= " + *inputString);
	//pobieramy tokeny rozdzielone przecinkiem
	vector<string> mainTokens=tokenize(*inputString,"[^,]+");

	this->numbers.clear();
    this->names.clear();

	vector<int> toRemoveValues;

	//dla kazdego takiego tokena sprawdzamy czy nie zawiera zakresu w sobie
	for (auto t: mainTokens)
	{
		//usuwamy white spaces

		t.erase(remove_if(t.begin(),t.end(),::isspace),t.end());

		int i1,i2;


		if (find(t.begin(),t.end(),'/')!=t.end()  || find(t.begin(),t.end(),'m' )!=t.end()|| find(t.begin(),t.end(),'M')!=t.end())
			convertExtended(t);
		else if (sscanf(t.c_str(),"%d-%d",&i1,&i2)==2)
		{
			auto range=minmax(i1,i2);

			for (int i=range.first;i<=range.second;i++)
			{
				this->numbers.push_back(i);
			}
            logMsg("range: " + to_string(i1) + "-" + to_string(i2));
		}
		else if (sscanf(t.c_str(),"!%d-%d",&i1,&i2)==2)
		{
            logMsg("neg range: " + to_string(i1) + "-" + to_string(i2));
			auto range=minmax(i1,i2);

			for (int i=range.first;i<=range.second;i++)
			{
				toRemoveValues.push_back(i);
			}

		}
		else if (sscanf(t.c_str(),"!%d",&i1)==1)
		{
            logMsg("neg: " + to_string(i1));
			toRemoveValues.push_back(i1);
		}
		else if (sscanf(t.c_str(),"%d",&i1)==1)
		{
            logMsg("val: " + to_string(i1));
			numbers.push_back(i1);
		}

		if (toRemoveValues.size())
		{
			for (auto i: toRemoveValues)
			{
				vector<int>::iterator it=find(numbers.begin(),numbers.end(),i);

				if (it!=numbers.end())
				{
					numbers.erase(it);
				}
			}
		}
	}
	string ports = "";
	for (auto i: names)
	{
		ports += ", " + i;
	}
	for (auto i: numbers)
	{
		names.push_back(to_string(i));
		ports += ", " + to_string(i);
	}
	ports = ports.substr(2,ports.size()-2);
    logMsg("Ports: " + ports);
    logMsg("+++++++++Convert end+++++++++");
    logMsg("");

}

vector<int>& RangeConverter::getNumbers()
{
	return numbers;
}

vector<string>& RangeConverter::getNames()
{
	return names;
}

RangeConverter::RangeConverter(string inputString, string proj)
{
    projPath = proj;
	this->inputString=&inputString;
	convert();
}

RangeConverter::RangeConverter(string inputString)
{
	this->inputString=&inputString;
	vector<string> a;
	convert();
}

RangeConverter::RangeConverter(string inputString, bool show, string proj)
{
    projPath = proj;
	this->inputString=&inputString;
	this->show = show;
    comunicate = " ";
	convert();
}


RangeConverter::~RangeConverter() {
	// TODO Auto-generated destructor stub
}

std::string RangeConverter::communicateOut()
{
    string a = comunicate;
    return a;
}

void RangeConverter::logMsg(string msg)
{
    ofstream myFile;

//    string path;
//    size_t posSlash = projPath.find_last_of("/\\");
//    path.append(projPath.begin()+posSlash+1,projPath.end()-4);//-4 jesli rozszerzenie .prj
//    if(path[0] == '\\')
//        path = "output" + path + "\\logs";
//    else
//        path = "output\\" + path + "\\logs";
//    string extension = ".txt";
//    path.append(extension);
    myFile.open(projPath, ios::app);
    if (myFile.is_open())
    {
        myFile <<msg<< endl;
        myFile.close();
    }
}



RangeConverter::RangeConverter(string inputString, bool show)
{
	this->inputString=&inputString;
	this->show = show;
    comunicate = " ";
	convert();
}
