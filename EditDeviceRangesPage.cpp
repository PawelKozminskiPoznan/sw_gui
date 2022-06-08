#include "EditDeviceRangesPage.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVariant>

#include <iostream>

EditDeviceRangesPage::EditDeviceRangesPage(QWidget *parent,GenerateProjectFile *gen, vector<string>* usedProjVar, QWizard* w) : QWizardPage(parent)
{
    wizard = w;
    generate = gen;
    layout = nullptr;
    numberPreviousDev = 0;
    numberActualDev = 0;
    usedProjVarPage = usedProjVar;
}


int EditDeviceRangesPage::nextId() const
{
    return nextPage;
}

bool EditDeviceRangesPage::validatePage()
{
    generate->outputsFoldersForExsistProj(project);
    string devName = field("editDevName").toString().toStdString();
    string max = limitPortsLineEdit->text().toStdString();
    maxSaved = stoi(max);
    generate->portLimitations[devName] = maxSaved;
    string projName = field("editedProjName").toString().toStdString();
    rConvert = new RangeConverter("1/1m1M"+max,1,"output/"+projName+"/logs.txt");

    rangePort.clear();
    rangePortSlots.clear();
    allUsedPorts.clear();
    allUsedPortSlots.clear();
    ranges.clear();
    generate->usedPortTypes.clear();
    bool allFildEmpty = true;

    vector<string>::iterator itSection;
    for(itSection = sections.begin(); itSection != sections.end(); ++itSection)
    {
        QString itSec = QString::fromStdString(*itSection);
        itSec += "Edit";
        string actualSection = *itSection;
        if(actualSection.compare(0,4,"port")==0)
        {
            string range = (field(itSec).toString()).toStdString();
            vector<pair<string,string>>::iterator iterRangeAndValues;
            for(iterRangeAndValues = rangeValues.begin(); iterRangeAndValues != rangeValues.end(); ++iterRangeAndValues)
            {
                if(iterRangeAndValues->first == actualSection)
                {
                    iterRangeAndValues->second = range;
                    break;
                }
            }
            if(!range.empty())
            {
                allFildEmpty = false;
                if(!checkPortRanges(itSection,range))
                {
                    size_t found = errorMessage.find("Input string=");
                    if(found != string::npos)
                    {
                        errorMessage = "Nieprawidłowy zakres portów dla "+errorMessage;
                    }
                    info(errorMessage);
                    return false;
                }
            }
        }
    }
    if(allFildEmpty)
    {
        info("Wprowadź zakres dla minimum 1 typu portu!");
        return false;
    }
    else
    {
        if(showVarInputPage())
            nextPage = MyWizard::Page_EditProjVar;
        else
            nextPage = MyWizard::Page_EditDeviceVar;

        errorFile->setText("");
        numberPreviousDev = numberActualDev;
        return true;
    }
}


void EditDeviceRangesPage::initializePage()
{
    projFile = field("path").toString();
    project = projFile.toStdString();
    if(project.empty())
    {
        project = "output/"+generate->originProjectName+"/"+generate->originProjectName+".prj";
        proj = field("editedProjName").toString().toStdString();
    }
    try {
        Project* proj = new Project(project,true);
        projectFile = proj->getParsedProject();
    } catch (...) {
        errorFile->setText(QString(QString::fromUtf8("Niepowodzenie...Pliki projektu nie został poprawnie zczytany.")));
    }
    string nameProj = project;
    uint pos = nameProj.find_last_of("/\\");
    nameProj = nameProj.substr(pos+1,nameProj.size());
    pos = nameProj.find_last_of(".");
    nameProj = nameProj.substr(0,pos);
    proj = nameProj;

    QString nameProjQ = "Edycja projektu: " + QString::fromStdString(nameProj);
    setTitle(nameProjQ);
    QString deviceName = (field("editDevName").toString());
    QString desc = QString(QString::fromUtf8("Edycja konfiguracji dla urządzenia: "));
    desc.append(deviceName);
    desc.append(QString::fromUtf8("\n Edycja zakresów portów"));
    setSubTitle(desc);

    QString temp = field("comboTempEdit").toString();
    string templatePath = "templates/" + temp.toStdString();
    //if(!templatePath.empty())
        tempPath = templatePath;
    generate->commentVariable = generate->commentFromTemplate(tempPath);
    generate->commentPort = generate->commentPortFromTemplate(tempPath);
    sections = generate->sectionsFromTemplate(tempPath);
    vector<string>::iterator itSection;
    string actualSection;

    if(layout != nullptr)//usuwanie poprzedniego layout'u
    {
        QLayoutItem* child;
            while(layout->count()!=0)
            {
                child = layout->takeAt(0);
                if(child->layout() != nullptr)
                {
                    delete (child->layout());
                }
                else if(child->widget() != nullptr)
                {
                    delete child->widget();
                }

                delete child;
            }
    }
    else
        layout = new QGridLayout;

    int row = 1;
    QLabel *legend = new QLabel;
    string legendString = "Definicja zakresów: \n"
      " -gdy w pliku szablonu mamy zdefiniowane porty w ten sposób: 1/$P_PORT \n"
      "  1,2,5            = 1, 2, 5 \n"
      "  1-5               = 1, 2, 3, 4, 5 \n"
      "  1-5,!2-3       = 1, 4, 5 \n"
      "  1-2,4-5         = 1, 2, 4, 5 \n"
      " -gdy w pliku szablonu mamy zdefiniowane porty w ten sposób: $P_PORT (wiele slotów i portów): \n"
      "  1/1-1/5          = 1/1, 1/2, 1/3, 1/4, 1/5 \n"
      "  1/1-2/3          = 1/1, 1/2, 1/3, 1/4, 2/1, 2/2, 2/3 (Maksymalnej liczba portów = 4) \n"
      "  1/1-2/2M3     = 1/1, 1/2, 1/3, 2/1, 2/2 (M3 -max nr portu = 3) \n"
      "  1/1-2/2m2     = 1/2, 1/3, 2/2, 2/3 (m2 -min nr portu = 2) (Maksymalnej liczba portów = 4) \n"
      "  1/1-3/5m2M3 = 1/2, 1/3, 2/2, 2/3, 3/2, 3/3 (m2 -min nr portu = 2, M3 -max nr portu = 3) \n"
      "  1/1-1/3,!1/2  = 1/1, 1/3 (! - oznacza negacje zakresu)\n"
      "  1/1-1/5,!1/2-1/4   = 1/1, 1/5 \n"
      " liczby po 'M' i 'm' zmieniają ograniczenia dla portów danego zakresu \n";
    const char * p = legendString.c_str();
    legend->setText(QString(QString::fromUtf8(p)));
    //legend->setWordWrap(true);
    layout->addWidget(legend,++row,1,14,-1);
    row = 15;


    string deviceNameOld = (field("combo").toString().toStdString());
    numberActualDev = generate->devicesNames.size()+1;
    for(itSection = sections.begin(); itSection != sections.end(); ++itSection)
    {
        actualSection = *itSection+":";
        if(actualSection.compare(0,4,"port")==0)
        {
            ++row;
            actualSection = *itSection + ": " +"("+ generate->commentPort[*itSection]+")";
            QString section = QString::fromStdString(*itSection);
            QLabel *portType = new QLabel;
            portType->setText(QString(QString::fromStdString(actualSection)));
            QLineEdit *portTypeLineEdit = new QLineEdit;


            vector<pair<string,string>>::iterator iterRangeAndValues;
            for(iterRangeAndValues = rangeValues.begin(); iterRangeAndValues != rangeValues.end(); ++iterRangeAndValues)
            {
                if(iterRangeAndValues->first == *itSection && numberPreviousDev == numberActualDev)
                {
                    string a = iterRangeAndValues->second;
                    QString b = QString::fromStdString(a);
                    portTypeLineEdit->setText(b);
                }
            }

            if(portTypeLineEdit->text().toStdString().empty())
            {
                auto wholeFile = projectFile.find(deviceNameOld)->second;
                for(auto a = wholeFile.begin(); a != wholeFile.end(); ++a)
                {
                    if(a->first == "ranges")
                        if(a->second.first == "number of ports of type "+*itSection+":")
                        {
                            portTypeLineEdit->setText(QString::fromStdString(a->second.second));
                        }
                }
            }

            layout->addWidget(portType,row,1,1,-1);
            ++row;
            layout->addWidget(portTypeLineEdit,row,1,1,-1);


            registerField(section+"Edit", portTypeLineEdit);

            if(numberPreviousDev != numberActualDev)
            {
                pair<string,string> rangeAndValue;
                rangeAndValue.first = *itSection;
                rangeValues.push_back(rangeAndValue);
            }
        }
    }
    QLabel *limitPortsLegendLabel = new QLabel;
    QLabel *limitPortsLabel = new QLabel;
    string maxPorts = "Maksymalna liczba portów:";
    const char * b = maxPorts.c_str();
    limitPortsLabel->setText(QString(QString::fromUtf8(b)));
    maxPorts = "Należy zdefinować (Maksymalną liczbę portów), podczas wprowadzania zakresów portów dla urządzeń z wieloma slotami np. 1/2-2/3";
    b = maxPorts.c_str();
    limitPortsLegendLabel->setText(QString(QString::fromUtf8(b)));
    limitPortsLegendLabel->setWordWrap(true);
    limitPortsLineEdit = new QLineEdit;
    if(numberPreviousDev != numberActualDev || generate->newDevRange)
    {
        generate->newDevRange = false;
        string maxSlots = generate->getMaxSlotsPerPort(deviceName.toStdString());
        if(maxSlots[0] == ' ')
            maxSlots = maxSlots.substr(1);
        limitPortsLineEdit->setText(QString::fromStdString(maxSlots));
    }
    else
    {
        string x = to_string(maxSaved);
        const char * y = x.c_str();
        limitPortsLineEdit->setText(QString(y));
    }
    registerField("limitPortEdit", limitPortsLineEdit);
    ++row;++row;
    layout->addWidget(limitPortsLabel,++row,1);
    layout->addWidget(limitPortsLineEdit,row,2);
    layout->addWidget(limitPortsLegendLabel,++row,1,2,-1);

    errorFile  = new QLabel();
    ++row;
    layout->addWidget(errorFile,++row,1);

    setLayout(layout);

    nextPage = MyWizard::Page_EditProjVar;
}

bool EditDeviceRangesPage::checkPortRanges(vector<string>::iterator itSection,string userData)
{
    string text;
    string actualSection = *itSection;
    bool uncorrectInputRange = true;
    if(actualSection.compare(0,4,"port")==0)//if start from 'port-..
    {
            try{
                        rConvert = new RangeConverter(userData,1,"output/"+field("editedProjName").toString().toStdString()+"/logs.txt");
                        vector<int>::iterator itNumersPort;
                        for(itNumersPort=rConvert->getNumbers().begin(); itNumersPort!=rConvert->getNumbers().end(); ++itNumersPort)
                        {
                            rangePort[*itSection].push_back(*itNumersPort);
                            allUsedPorts.push_back(*itNumersPort);
                            uncorrectInputRange = false;
                        }
                        if(rConvert->getNumbers().empty())
                        {
                            vector<string>::iterator itNumersPortSlots;
                            for(itNumersPortSlots=rConvert->getNames().begin(); itNumersPortSlots!=rConvert->getNames().end(); ++itNumersPortSlots)
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
                                errorMessage = "Redeklaracja portu  " + text;
                                for(unsigned int i = 0; i < rangePort[*itSection].size(); ++i)
                                {
                                    allUsedPorts.pop_back();
                                }
                                rangePort[*itSection].clear();
                                return false;
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
                                errorMessage = "Redeklaracja portu  " + text;
                                for(unsigned int i = 0; i < rangePortSlots[*itSection].size(); ++i)
                                {
                                    allUsedPortSlots.pop_back();
                                }
                                rangePortSlots[*itSection].clear();
                                return false;
                            }
                        }
                        if(!uncorrectInputRange)
                        {
                            generate->usedPortTypes.push_back(*itSection);
                            text += userData;
                            ranges["number of ports of type "+ *itSection + ":"] = userData;
                        }
                }
                catch(...){
                    errorMessage = getErrorLog();
                    return false;
                }
    }
    return true;
}

bool EditDeviceRangesPage::showVarInputPage()
{
    QString temp = field("comboTempEdit").toString();
    string templatePath = "templates/" + temp.toStdString();
    generate->variableSwitchName = "USER_SW_SWITCH_NAME";
    generate->setPrefixVariables();
    generate->var = generate->variablesFromTemplate(tempPath);
    map<string,vector<string>>::iterator iteratorVariables;
    iteratorVariables = generate->var.find("project");
    bool anyNewProjVar = false;
    numberActualDev = generate->devicesNames.size()+1;
    if(iteratorVariables != generate->var.end())
    {
        bool existUndeclaredVar = true;
        while(existUndeclaredVar)
        {
            if(iteratorVariables->second.empty())
                existUndeclaredVar = false;
            else
            {
                string nameVariable = iteratorVariables->second.front();
                string nameVariableWithoutEqualSign = nameVariable;
                if(!(iteratorVariables->first == "project" && generate->isStringInVector(generate->usedProjectVariables,nameVariable)))
                {
                    anyNewProjVar = true;
                    return anyNewProjVar;
                }
                iteratorVariables->second.erase(iteratorVariables->second.begin());
            }
        }
    }
    return anyNewProjVar;
}

string EditDeviceRangesPage::getErrorLog()
{
    string path;
    string project = field("editedProjName").toString().toStdString();
    path = "output\\" + project + "\\logs";
    string extension = ".txt";
    path.append(extension);
    string errorLog;
    ifstream f(path);
    if (f.is_open())
    {
        string tempError;
        while (getline(f,tempError))
        {
            if(!tempError.empty())
                errorLog = tempError;
        }
        return errorLog;
    }
    return "Problem z plikiem " + path;
}


void EditDeviceRangesPage::info(const string &error)
{
    infoWidow = new QWidget;
    infoWidow->resize(240,200);

    QVBoxLayout* lRepeated = new QVBoxLayout;
    QLabel* info = new QLabel();
    string text;
    QPushButton *ok = new QPushButton("Ok");
    text = error;

    const char * c = text.c_str();
    QString t = QString(QString::fromUtf8(c));
    connect(ok, SIGNAL(released()), this, SLOT(okPress()));
    info->setText(t);
    lRepeated->addWidget(info);
    ok->setDefault(true);
    lRepeated->addWidget(ok);
    infoWidow->setLayout(lRepeated);
    infoWidow->show();
    wizard->button(QWizard::NextButton)->setDisabled(true);
}

void EditDeviceRangesPage::okPress()
{
    wizard->button(QWizard::NextButton)->setDisabled(false);
    infoWidow->hide();
}
