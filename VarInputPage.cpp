#include "VarInputPage.h"
#include "MyWizard.h"
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <iostream>

varInputPage::varInputPage(QWidget *parent, GenerateProjectFile *gen, QWizard *w, vector<string>* usedProjVar) : QWizardPage(parent)
{
    generate = gen;
    layout = nullptr;
    wizard = w;
    numberActualDev = 0;
    numberPreviousDev = 0;
    usedProjVarPage = usedProjVar;

    nextPage = MyWizard::Page_VarDevice;
}


int varInputPage::nextId() const
{
    return nextPage;
}

bool varInputPage::validatePage()
{
    QString temp = field("comboTemp").toString();
    string tempPath = "templates/" + temp.toStdString();
    generate->var = generate->variablesFromTemplate(tempPath);
    map<string,vector<string>>::iterator iteratorVariables;
    iteratorVariables = generate->var.find("project");
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

                string varProjValue = "varProjValue"+std::to_string(counter);
                QString varProjValueQ = QString::fromStdString(varProjValue);
                string valueVariable = (field(varProjValueQ).toString()).toStdString();
                vector<pair<string,string>>::iterator iterVarAndValues;
                for(iterVarAndValues = projVarAndValues.begin(); iterVarAndValues != projVarAndValues.end(); ++iterVarAndValues)
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
    if(showVarDeviceInputPage())
        nextPage = MyWizard::Page_VarDevice;
    else if(showVarPortTypeInputPage())
        nextPage = MyWizard::Page_VarPort;
    else
        nextPage = MyWizard::Page_DevDone;
    numberPreviousDev = numberActualDev;
    return true;
}

void varInputPage::initializePage()
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
    desc.append("\n Inicjacja zmiennych projektowych");
    setSubTitle(desc);

    actualDev = deviceName.toStdString();

    QString temp = field("comboTemp").toString();
    string tempPath = "templates/" + temp.toStdString();
    generate->variableSwitchName = "USER_SW_SWITCH_NAME";
    generate->setPrefixVariables();
    generate->var = generate->variablesFromTemplate(tempPath);

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

    map<string,vector<string>>::iterator iteratorVariables;
    iteratorVariables = generate->var.find("project");
    numberActualDev = generate->devicesNames.size();
    if(generate->newDevProjVar)
    {
        projVarAndValues.clear();
        generate->newDevProjVar = false;
    }
    if(iteratorVariables != generate->var.end())
    {
        bool existUndeclaredVar = true;
        int counter = 0;
        while(existUndeclaredVar)
        {
            if(iteratorVariables->second.empty())
            {
                existUndeclaredVar = false;
            }
            else
            {
                string nameVariable = iteratorVariables->second.front();
                string nameVariableWithoutEqualSign = nameVariable;
                if(!(iteratorVariables->first == "project" && generate->isStringInVector(generate->usedProjectVariables,nameVariable)))
                {
                    counter++;

                    if(!(std::find(usedProjVarPage->begin(), usedProjVarPage->end(), nameVariable) != usedProjVarPage->end()))
                    {
                        QString description = QString::fromStdString(generate->commentVariable[nameVariable]);
                        nameVariable.append("=");

                        QLabel *portType = new QLabel;
                        QString nameVar = QString::fromStdString(nameVariable);

                        portType->setText(nameVar+" ("+description+")");
                        QLineEdit *portTypeLineEdit = new QLineEdit();

                        layout->addWidget(portType);
                        layout->addWidget(portTypeLineEdit);

                        string varProjValue = "varProjValue"+std::to_string(counter);
                        QString varProjValueQ = QString::fromStdString(varProjValue);

                        registerField(varProjValueQ, portTypeLineEdit);//+'*'
                        projFields.push_back(varProjValue);
                        connect(portTypeLineEdit, SIGNAL(textChanged(const QString &)),this, SIGNAL(completeChanged()));

                        string x = (field(varProjValueQ).toString()).toStdString();

                        vector<pair<string,string>>::iterator iterProjVarAndValues;
                        bool isVarInProjVarAndValues = false;
                        bool focusOnFirst = true;
                        for(iterProjVarAndValues = projVarAndValues.begin(); iterProjVarAndValues != projVarAndValues.end(); ++iterProjVarAndValues)
                        {
                            isVarInProjVarAndValues = false;
                            if(iterProjVarAndValues->first == nameVariableWithoutEqualSign )
                            {
                                string a = iterProjVarAndValues->second;
                                QString b = QString::fromStdString(a);
                                portTypeLineEdit->setText(b);
                                isVarInProjVarAndValues = true;
                                if(focusOnFirst)
                                {
                                    QTimer::singleShot(0, portTypeLineEdit, SLOT(setFocus()));// działa?
                                    focusOnFirst = false;
                                }
                                break;
                            }
                        }
                        if(numberPreviousDev != numberActualDev || !isVarInProjVarAndValues)
                        {
                            pair<string,string> varAndValue;
                            varAndValue.first = nameVariableWithoutEqualSign;
                            varAndValue.second = portTypeLineEdit->text().toStdString();
                            projVarAndValues.push_back(varAndValue);
                        }
                    }
                }
                iteratorVariables->second.erase(iteratorVariables->second.begin());
            }
        }
    }
        setLayout(layout);
}

bool varInputPage::showVarDeviceInputPage()
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
    iteratorVariables = generate->var.find("device");
    if(iteratorVariables != generate->var.end())
        anythingToDisplay = true;
    return anythingToDisplay;
}

bool varInputPage::showVarPortTypeInputPage()
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


bool varInputPage::isComplete() const
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
