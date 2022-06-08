#include "VarDevEnd.h"

#include <QLayoutItem>
#include <QVBoxLayout>
#include <iostream>

VarDevEnd::VarDevEnd(QWidget *parent, GenerateProjectFile *gen, QWizard *w, vector<string>* usedProjVar) : QWizardPage(parent)
{
    generate = gen;
    wizard = w;

    layout = nullptr;

    previousDev = " ";
    numberActualDev = 0;
    usedProjVarPage = usedProjVar;
    lastDev = false;
}

int VarDevEnd::nextId() const
{
    return nextPage;
}

bool VarDevEnd::validatePage()
{
    generateDataToProj();
    generate->newDev = true;
    generate->newDevRange = true;
    generate->newDevVar = true;
    generate->newPortVar = true;
    if(!lastDev)
    {
        wizard->setStartId(MyWizard::Page_NewProj);
        wizard->restart();
    }
    else
    {
        nextPage = MyWizard::Page_VarDone;
    }
    return true;
}

void VarDevEnd::initializePage()
{
    lastDev = false;
    nextPage = MyWizard::Page_VarDone;
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

    string fieldProjNameTemp = field("projectName").toString().toStdString();
    if(!fieldProjNameTemp.empty())
        fieldProjName = QString::fromStdString(fieldProjNameTemp);
    QString nameProj;
    QString deviceName = (field("devName").toString());
    QString desc = QString(QString::fromUtf8("Wprowadzono całą konfiguracje dla urządzenia: "));
    desc.append(deviceName);
    setSubTitle(desc);

    numberActualDev = int(generate->devicesNames.size());
    devAmount = (field("devicesAmount").toString()).toStdString();
    if(!devAmount.empty())
        deviceAmount = stoi(devAmount);
    else
        devAmount = to_string(deviceAmount);

    if(numberActualDev < deviceAmount)
    {
        QLabel *info = new QLabel();
        layout->addWidget(info);
        setLayout(layout);
        info->setText(QString(QString::fromUtf8("Zatwierdź i przejdź do następnego urządzenia   (Zatwierdź) \n \n Edycja konfiguracji urządzenia   (Wstecz)")));
        nameProj = "Tworzenie projeku: " + fieldProjName;
    }
    else
    {
        QLabel *info = new QLabel();
        layout->addWidget(info);
        setLayout(layout);
        info->setText(QString(QString::fromUtf8("Zatwierdź i generuj pliki konfiguracyjne   (Zatwierdź) \n \n Edycja   (Wstecz)")));
        lastDev = true;
        nameProj = "Stworzono projekt: " + fieldProjName;
    }
    setTitle(nameProj);
    QString buttonText = QString::fromUtf8("Zatwierdź");
    wizard->setButtonText(wizard->NextButton, buttonText);
}

void VarDevEnd::cleanupPage()
{
    QString buttonText = QString::fromUtf8("Dalej");
    wizard->setButtonText(wizard->NextButton, buttonText);
}

void VarDevEnd::generateDataToProj()
{
    string devName = (field("devName").toString()).toStdString();
    QString temp = field("comboTemp").toString();
    string tempPath = "templates/" + temp.toStdString();

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
            string range = (field(sectionName).toString()).toStdString();
            if(range != "")
            {
                generate->declareProjectFileLine("number of ports of type "+ *itSection + ":", range, devName, "ranges");
            }
        }
    }
    string limitPort = (field("limitPort").toString()).toStdString();
    generate->declareProjectFileLine("max slots per port:", limitPort, devName, "ranges");

    vector<string>::iterator b;
    int counter = 1;
    for(map<string,vector<string>>::iterator a = generate->var.begin(); a != generate->var.end(); ++a)
    {
        if(a->first == "project")
             for(b = a->second.begin(); b != a->second.end(); ++b)
             {
                 string varProjValue = "varProjValue"+std::to_string(counter);
                 QString varProjValueQ = QString::fromStdString(varProjValue);

                 if(!(std::find(usedProjVarPage->begin(), usedProjVarPage->end(), *b) != usedProjVarPage->end()))
                    generate->declareProjectFileLine(*b+'=', (field(varProjValueQ).toString()).toStdString(), "projectVar", "variables");
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
                 string varDevValue = "varDevValue"+std::to_string(counter);
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

                        string value = (field(nameVar).toString()).toStdString();
                        generate->declareProjectFileLine(nameVariable, value, devName, actualSection);
                        iteratorVariables->second.erase(iteratorVariables->second.begin());
                    }
                }
            }
        }
}
