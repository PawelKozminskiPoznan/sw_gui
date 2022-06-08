#include "EditProjVarPage.h"

#include "MyWizard.h"
#include <QLabel>
#include <QVBoxLayout>
#include <iostream>

EditProjVarPage::EditProjVarPage(QWidget *parent, GenerateProjectFile *gen, QWizard *w, vector<string>* usedProjVar) : QWizardPage(parent)
{
    generate = gen;
    layout = nullptr;
    wizard = w;
    numberActualDev = 0;
    numberPreviousDev = 0;
    usedProjVarPage = usedProjVar;

    nextPage = MyWizard::Page_EditDeviceVar;
}


int EditProjVarPage::nextId() const
{
    return nextPage;
}

bool EditProjVarPage::validatePage()
{
    QString temp = field("comboTempEdit").toString();
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
                QString varProjValueQ = QString::fromStdString(varProjValue) + "Edit";
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
        nextPage = MyWizard::Page_EditDeviceVar;
    else if(showVarPortTypeInputPage())
        nextPage = MyWizard::Page_EditPortVar;
    else
        nextPage = MyWizard::Page_EditDevEnd;
    numberPreviousDev = numberActualDev;
    return true;
}

void EditProjVarPage::initializePage()
{
    projFields.clear();
    projFile = field("path").toString();
    string project = projFile.toStdString();
    errorFile = new QLabel;
    if(project.empty())
        project = "output/"+generate->originProjectName+"/"+generate->originProjectName+".prj";
    try {
        Project* proj = new Project(project,true);
        projectFile = proj->getParsedProject();
        errorFile->setText("");
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
    desc.append(QString::fromUtf8("\n Edycja zmiennych projektowych"));
    setSubTitle(desc);

    QString temp = field("comboTempEdit").toString();
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
    numberActualDev = generate->devicesNames.size()+1;
    if(numberPreviousDev != numberActualDev)
        projVarAndValues.clear();
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
                string nameVariable = iteratorVariables->second.front();
                string nameVariableWithoutEqualSign = nameVariable;
                if(!(iteratorVariables->first == "project" && generate->isStringInVector(generate->usedProjectVariables,nameVariable)))
                {
                    counter++;

                    QString description = QString::fromStdString(generate->commentVariable[nameVariable]);
                    nameVariable.append("=");

                    QLabel *portType = new QLabel;
                    QString nameVar = QString::fromStdString(nameVariable);

                    portType->setText(nameVar+" ("+description+")");
                    QLineEdit *portTypeLineEdit = new QLineEdit();

                    layout->addWidget(portType);
                    layout->addWidget(portTypeLineEdit);

                    string varProjValue = "varProjValue"+std::to_string(counter)+ "Edit";
                    QString varProjValueQ = QString::fromStdString(varProjValue);

                    registerField(varProjValueQ, portTypeLineEdit);
                    projFields.push_back(varProjValue);
                    connect(portTypeLineEdit, SIGNAL(textChanged(const QString &)),this, SIGNAL(completeChanged()));

                    vector<pair<string,string>>::iterator iterProjVarAndValues;
                    bool isVarInProjVarAndValues;
                    for(iterProjVarAndValues = projVarAndValues.begin(); iterProjVarAndValues != projVarAndValues.end(); ++iterProjVarAndValues)
                    {
                        isVarInProjVarAndValues = false;
                        if(iterProjVarAndValues->first == nameVariableWithoutEqualSign )
                        {
                            string a = iterProjVarAndValues->second;
                            QString b = QString::fromStdString(a);
                            portTypeLineEdit->setText(b);
                            isVarInProjVarAndValues = true;
                        }
                    }
                    if(portTypeLineEdit->text().toStdString().empty())
                    {
                        auto wholeFile = projectFile.find("projectVar")->second;
                        for(auto a = wholeFile.begin(); a != wholeFile.end(); a++)
                        {
                            if(a->first == "variables")
                                if(a->second.first == nameVariable)
                                    portTypeLineEdit->setText(QString::fromStdString(a->second.second));
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
                iteratorVariables->second.erase(iteratorVariables->second.begin());
            }
        }
    }
    layout->addWidget(errorFile);
    setLayout(layout);
    numberPreviousDev = numberActualDev;
}


bool EditProjVarPage::showVarDeviceInputPage()
{
    QString temp = field("comboTempEdit").toString();
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

bool EditProjVarPage::showVarPortTypeInputPage()
{
    QString temp = field("comboTempEdit").toString();
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
            actualSection = iteratorVariables->first + "Edit";
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


bool EditProjVarPage::isComplete() const
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
