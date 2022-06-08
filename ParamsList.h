/*
 * ParamsList.h
 *
 *  Created on: 9 Jul 2019
 *      Author: pp
 */

#ifndef PARAMSLIST_H_
#define PARAMSLIST_H_

#include <map>
#include <vector>
#include <string>

class ParamsList : public std::map<std::string,std::string>{
public:
	ParamsList();
	virtual ~ParamsList();
};

#endif /* PARAMSLIST_H_ */
