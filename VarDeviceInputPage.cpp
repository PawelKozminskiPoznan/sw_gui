#include "VarDeviceInputPage.h"
#include "MyWizard.h"

#include <iostream>

varDeviceInputPage::varDeviceInputPage(QWidget *parent, GenerateProjectFile *gen) : QWizardPage(parent)
{
    generate = gen;
    layout = nullptr;
    numberActualDev = 0;
    numberPreviousDev = 0;

    nextPage = MyWizard::Page_VarPort;
}

int varDeviceInputPage::nextId() const
{
    return nextPage;
}

bool varDeviceInputPage::validatePage()
{
    numberPreviousDev = numberActualDev;
    QString temp = field("comboTemp").toString();
    string tempPath = "templates/" + temp.toStdString();
    generate->var = generate->variablesFromTemplate(tempPath);
    map<string,vector<string>>::iterator iteratorVariables;
    iteratorVariables = generate->var.find("device");
    if(iteratorVariables != generate->var.end())
    {
        bool existUndeclaredVar = true;
        int counter = 0;
        while(existUndeclaredVar)
        {
            if(iteratorVariables->second.empty())
                existUndeclaredVar = false;
            else
            {
                counter++;
                string nameVariable = iteratorVariables->second.front();

                string varProjValue = "varDevValue"+std::to_string(counter);
                QString varProjValueQ = QString::fromStdString(varProjValue);
                string valueVariable = (field(varProjValueQ).toString()).toStdString();
                vector<pair<string,string>>::iterator iterVarAndValues;
                for(iterVarAndValues = switchVarAndValues.begin(); iterVarAndValues != switchVarAndValues.end(); ++iterVarAndValues)
                {
                    if(iterVarAndValues->first == nameVariable)
                    {
                        iterVarAndValues->second = valueVariable;
                    }
                }
                iteratorVariables->second.erase(iteratorVariables->second.begin());
            }
        }
    }
    if(showVarPortTypeInputPage())
        nextPage = MyWizard::Page_VarPort;
    else
        nextPage = MyWizard::Page_DevDone;
    generate->newDevVar = false;
    generate->newDevDevVar = false;
    return true;
}

void varDeviceInputPage::initializePage()
{
    projFields.clear();
    string fieldProjNameTemp = field("projectName").toString().toStdString();
    if(!fieldProjNameTemp.empty())
        fieldProjName = QString::fromStdString(fieldProjNameTemp);
    QString nameProj = "Nowy projekt: " + fieldProjName;
    setTitle(nameProj);
    QString deviceName = (field("devName").toString());
    QString desc = QString(QString::fromUtf8("Tworzenie konfiguracji dla urządzenia: "));
    desc.append(deviceName);
    desc.append(QString::fromUtf8("\n Inicjacja zmiennych urządzeniowych"));
    setSubTitle(desc);
    numberActualDev = generate->devicesNames.size();
//    if(numberPreviousDev != numberActualDev)
//        switchVarAndValues.clear();
    int counter = 1;
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

    if(generate->newDevDevVar)
        switchVarAndValues.clear();
    map<string,vector<string>>::iterator iteratorVariables;
    iteratorVariables = generate->var.find("device");
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
                if(!(generate->isStringInVector(generate->usedSwitchVariables,nameVariable))/*&&!(iteratorVariables->first == "project" && generate->isStringInVector(generate->usedProjectVariables,nameVariable))*/)
                {
                    generate->usedProjectVariables.push_back(nameVariable);
                    QString description = QString::fromStdString(generate->commentVariable[nameVariable]);
                    nameVariable.append("=");

                    QLabel *portType = new QLabel;
                    QString nameVar = QString::fromStdString(nameVariable);
                    portType->setText(nameVar+" ("+description+")");
                    QLineEdit *portTypeLineEdit = new QLineEdit();

                    layout->addWidget(portType);
                    layout->addWidget(portTypeLineEdit);

                    string varDevValue = "varDevValue"+std::to_string(counter);
                    QString varDevValueQ = QString::fromStdString(varDevValue);

                    registerField(varDevValueQ, portTypeLineEdit);//+'*'
                    projFields.push_back(varDevValue);
                    connect(portTypeLineEdit, SIGNAL(textChanged(const QString &)),this, SIGNAL(completeChanged()));

                    string x = (field(varDevValueQ).toString()).toStdString();
                    counter++;

                    vector<pair<string,string>>::iterator iterSwitchVarAndValues;
                    for(iterSwitchVarAndValues = switchVarAndValues.begin(); iterSwitchVarAndValues != switchVarAndValues.end(); ++iterSwitchVarAndValues)
                    {
                        if(iterSwitchVarAndValues->first == nameVariableWithoutEqualSign )
                        {
                            string a = iterSwitchVarAndValues->second;
                            QString b = QString::fromStdString(a);
                            portTypeLineEdit->setText(b);
                        }
                    }
                    if(generate->newDevVar)//numberPreviousDev != numberActualDev ||
                    {
                        pair<string,string> varAndValue;
                        varAndValue.first = nameVariableWithoutEqualSign;
                        switchVarAndValues.push_back(varAndValue);
                    }
                }
                iteratorVariables->second.erase(iteratorVariables->second.begin());
            }
        }
    }
    setLayout(layout);
}


bool varDeviceInputPage::showVarPortTypeInputPage()
{
    QString temp = field("comboTemp").toString();
    string tempPath = "templates/" + temp.toStdString();
    generate->variableSwitchName = "USER_SW_SWITCH_NAME";
    generate->setPrefixVariables();
    generate->var = generate->variablesFromTemplate(tempPath);
    map<string,vector<string>>::iterator iteratorVariables;
    vector<string> sections = generate->sectionsFromTemplate(tempPath);
    bool anythingToDisplay = false;
    string actualSection;
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
                        existUndeclaredVar = true;
                        anythingToDisplay = true;
                        iteratorVariables->second.erase(iteratorVariables->second.begin());
                    }
                }
            }
        }
    return anythingToDisplay;
}


bool varDeviceInputPage::isComplete() const
{
    bool isEmpty = false;
    for(auto fieldName = projFields.begin(); fieldName != projFields.end(); ++fieldName)
    {
        QString fieldNameString = QString::fromStdString(*fieldName);
        string z = (field(fieldNameString).toString()).toStdString();
        isEmpty = isEmpty || z.empty();
    }
    return !isEmpty;
}
