#include <sstream>
#include <map>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <QTextCodec>
//#include <unistd.h>

#include <stdio.h>
#include "DeviceTemplate.h"
#include "Project.h"
#include "RangeConverter.h"
#include "GenerateProjectFile.h"
#include "InputArgs.h"

#include "MyWizard.h"
#include <QApplication>

//#include <windows.h>

using namespace std;

int main(int argc, char ** argv)
{
    //ShowWindow( GetConsoleWindow(), SW_HIDE );
    QTextCodec::setCodecForLocale ( QTextCodec::codecForName( "UTF-8" ) );
    QApplication a(argc, argv);
    MyWizard w(argv[0]);
    return a.exec();
}
