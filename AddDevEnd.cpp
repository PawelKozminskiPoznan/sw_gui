#include "AddDevEnd.h"

#include <QLayoutItem>
#include <QVBoxLayout>
#include <iostream>

AddDevEnd::AddDevEnd(QWidget *parent, GenerateProjectFile *gen, QWizard *w, vector<string>* usedProjVar) : QWizardPage(parent)
{
    generate = gen;
    wizard = w;

    layout = nullptr;

    previousDev = " ";
    numberActualDev = 0;
    usedProjVarPage = usedProjVar;
    lastDev = false;
}

int AddDevEnd::nextId() const
{
    return nextPage;
}

bool AddDevEnd::validatePage()
{
    generate->newDev = true;
    generate->newDevRange = true;
    generate->newDevVar = true;
    generate->newPortVar = true;
    generateDataToProj();
    try {
        string proj = field("editedProjName").toString().toStdString();
        generate->originProjectName = proj;
        generate->outputsFolders(proj);
        generate->writeToProjectFile();
    }
    catch (...) {
            cout<<"error"<<endl;
        }
    generate->editedProj = true;
    wizard->setStartId(MyWizard::Page_ExistProjEdit);
    bool genEdit = generate->editedProj;
    wizard->restart();
    generate->editedProj = genEdit;
    return true;
}

void AddDevEnd::initializePage()
{
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
        layout = new QVBoxLayout;

    QString projFile = field("path").toString();
    nameProj = projFile.toStdString();
    uint pos = nameProj.find_last_of("/\\");
    nameProj = nameProj.substr(++pos,nameProj.size());
    pos = nameProj.find_last_of(".");
    nameProj = nameProj.substr(0,pos);

    if(nameProj.empty())
        nameProj = field("editedProjName").toString().toStdString();

    QString nameProjQ = "Edycja projektu: " + QString::fromStdString(field("editedProjName").toString().toStdString());
    setTitle(nameProjQ);
    QString deviceName = (field("newDevName").toString());
    QString desc = QString(QString::fromUtf8("Tworzenie konfiguracji dla urządzenia: "));
    desc.append(deviceName);
    desc.append(QString::fromUtf8("\n Zatwierdzenie zmian lub edycja"));
    setSubTitle(desc);

    QLabel *info = new QLabel();
    layout->addWidget(info);
    setLayout(layout);
    info->setText(QString(QString::fromUtf8("Zatwierdź i zapisz zmiany  (Zatwierdź) \n \n Edycja konfiguracji urządzenia   (Wstecz)")));

    QString buttonText = QString::fromUtf8("Zatwierdź");
    wizard->setButtonText(wizard->NextButton, buttonText);
}

void AddDevEnd::cleanupPage()
{
    QString buttonText = QString::fromUtf8("Dalej");
    wizard->setButtonText(wizard->NextButton, buttonText);
}

void AddDevEnd::generateDataToProj()
{
    string devName = (field("newDevName").toString()).toStdString();
    QString temp = field("comboTempAdd").toString();
    string tempPath = "templates/" + temp.toStdString();
    generate->devicesNames.push(devName);//?
    generate->addDevNameToConfigFile(devName);
    generate->incrementDevAmountToConfigFile();
    generate->declareProjectFileLine(devName, "[" + devName + "]", devName, "deviceNameBrackets");
    generate->declareProjectFileLine("template:", tempPath, devName, "template");

    generate->var = generate->variablesFromTemplate(tempPath);
    vector<string> sections = generate->sectionsFromTemplate(tempPath);
    vector<string>::iterator itSection;
    for(itSection = sections.begin(); itSection != sections.end(); ++itSection)
    {
        QString sectionName = QString::fromStdString(*itSection);
        map<string,string>::iterator itRanges;
        string actualSection = *itSection+":";
        if(actualSection.compare(0,4,"port")==0)
        {
            sectionName += "Add";
            string range = (field(sectionName).toString()).toStdString();
            if(range != "")
            {
                generate->declareProjectFileLine("number of ports of type "+ *itSection + ":", range, devName, "ranges");
            }
        }
    }
    string limitPort = (field("limitPortAdd").toString()).toStdString();
    generate->declareProjectFileLine("max slots per port: ", limitPort, devName, "ranges");

    vector<string>::iterator b;
    int counter = 1;
    generate->clearAndSaveProjVar();
    for(map<string,vector<string>>::iterator a = generate->var.begin(); a != generate->var.end(); ++a)
    {
        if(a->first == "project")
             for(b = a->second.begin(); b != a->second.end(); ++b)
             {
                 string varProjValue = "varAddProjValue"+std::to_string(counter);
                 QString varProjValueQ = QString::fromStdString(varProjValue);
                 generate->declareProjectVar(*b+'=', (field(varProjValueQ).toString()).toStdString());
                 ++counter;
                 usedProjVarPage->push_back(*b);
             }
    }

    counter = 1;
    for(map<string,vector<string>>::iterator a = generate->var.begin(); a != generate->var.end(); ++a)
    {
        if(a->first == "device")
             for(b = a->second.begin(); b != a->second.end(); ++b)
             {
                 string varDevValue = "varAddDevValue"+std::to_string(counter);
                 QString varDevValueQ = QString::fromStdString(varDevValue);

                 string x = (field(varDevValueQ).toString()).toStdString();
                 generate->declareProjectFileLine(*b+'=', (field(varDevValueQ).toString()).toStdString(), devName, "device");
                 ++counter;
             }
    }

    string actualSection;
    map<string,vector<string>>::iterator iteratorVariables;
    for(iteratorVariables = generate->var.begin(); iteratorVariables != generate->var.end(); ++iteratorVariables)
        if(iteratorVariables != generate->var.find("project") && iteratorVariables != generate->var.find("device"))
        {
            actualSection = iteratorVariables->first;
            actualSection += "Add";
            string rangeSec = (field(QString::fromStdString(actualSection)).toString()).toStdString();
            if(!rangeSec.empty())
            {
                bool existUndeclaredVar = true;
                while(existUndeclaredVar)
                {
                    if(iteratorVariables->second.empty())
                        existUndeclaredVar = false;
                    else
                    {
                        if(iteratorVariables->first != actualSection)
                        {
                            actualSection = iteratorVariables->first;
                            QString namePort = QString::fromStdString(actualSection);
                        }
                        string nameVariable = iteratorVariables->second.front();
                        nameVariable.append("=");

                        QString nameVar = QString::fromStdString(nameVariable + actualSection);
                        nameVar += "Add";
                        string value = (field(nameVar).toString()).toStdString();
                        generate->declareProjectFileLine(nameVariable, value, devName, actualSection);
                        iteratorVariables->second.erase(iteratorVariables->second.begin());
                    }
                }
            }
        }
}
