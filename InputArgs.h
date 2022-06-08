/*
 * InputArgs.h
 *
 *  Created on: Jul 6, 2020
 *      Author: pawelkoz
 */

#ifndef INPUTARGS_H_
#define INPUTARGS_H_

#include <string>

using namespace std;

class InputArgs {
public:
	InputArgs();
	InputArgs(int argc, char ** argv);
	virtual ~InputArgs();
	short start();
	string getArgumentR();
	string getArgumentT();
	string getArgumentO();
	string getArgument();
private:
	int argc;
	char ** argv;
	string argumentR;
	string argumentO;
	string argumentT;
	int argType;
};

#endif /* INPUTARGS_H_ */
