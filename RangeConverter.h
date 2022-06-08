/*
 * RangeConverter.h
 *
 *  Created on: 3 Jul 2019
 *      Author: pp
 */

#ifndef RANGECONVERTER_H_
#define RANGECONVERTER_H_

#include <string>
#include <vector>

using namespace std;

class RangeConverter {
public:
	RangeConverter();
    RangeConverter(string inputString);
    RangeConverter(string inputString, string proj);
    RangeConverter(string inputString, bool);
    RangeConverter(string inputString, bool, string proj);
    string communicateOut();
    vector<int>& getNumbers();
    vector<string>& getNames();
	virtual ~RangeConverter();
    vector<int> numbers;
private:
	void convert();
    void convertExtended(string& token);
    vector<std::string> tokenize(string &inputString,string regexExpAsDelimiters);
    vector<std::string> names;
    string* inputString;
	bool show;
    string comunicate;
    void logMsg(string msg);
    string projPath;
};

#endif /* RANGECONVERTER_H_ */
