/*
 * utilsHeader.h
 *
 *  Created on: 8 Jul 2019
 *      Author: pp
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <vector>

#define DEBUG_PREF "[" << __FUNCTION__ <<":" << __LINE__ << "]:"
#define DEBUG2_PREF "[" << __PRETTY_FUNCTION__ <<":" << __LINE__ << "]:"


#define TRIM_LEADING 1
#define TRIM_TRAILING 2

namespace pputils
{

	std::string trim(std::string& input,int opt);
	std::vector<std::string> tokenize(std::string &inputString,std::string regexExpAsDelimiters);
	bool        isThisAComment(std::string& input);

	std::string replace(std::string& in, std::string from, std::string to);
}

#endif /* UTILS_H_ */
