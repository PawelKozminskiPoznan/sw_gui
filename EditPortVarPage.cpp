#include "EditPortVarPage.h"

#include "MyWizard.h"
#include <QLabel>
#include <QVBoxLayout>
#include <iostream>

EditPortVarPage::EditPortVarPage(QWidget *parent, GenerateProjectFile *gen, QWizard *w) : QWizardPage(parent)
{
    generate = gen;
    wizard = w;
    layout = nullptr;
    numberActualDev = 0;
    numberPreviousDev = 0;

    nextPage = MyWizard::Page_EditDevEnd;
}

int EditPortVarPage::nextId() const
{
    return nextPage;
}

bool EditPortVarPage::validatePage()
{
    QString temp = field("comboTempEdit").toString();
    string tempPath = "templates/" + temp.toStdString();
    generate->var = generate->variablesFromTemplate(tempPath);
    string actualSection;
    map<string,vector<string>>::iterator iteratorVariables;
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
                        string nameVariable = iteratorVariables->second.front();
                        string nameVariableWithoutEqualSign = nameVariable + actualSection;
                        nameVariable.append("=");

                        QString nameVar = QString::fromStdString(nameVariable + actualSection);
                        string valueVariable = (field(nameVar).toString()).toStdString();
                        vector<pair<string,string>>::iterator iterVarAndValues;
                        for(iterVarAndValues = portVarAndValues.begin(); iterVarAndValues != portVarAndValues.end(); ++iterVarAndValues)
                        {
                            if(iterVarAndValues->first == nameVariableWithoutEqualSign)
                            {
                                iterVarAndValues->second = valueVariable;
                            }
                        }
                        iteratorVariables->second.erase(iteratorVariables->second.begin());
                    }
                }
            }
        }
    numberPreviousDev = numberActualDev;
    return true;
}

void EditPortVarPage::initializePage()
{
    projFields.clear();
    projFile = field("path").toString();
    string project = projFile.toStdString();
    if(project.empty())
        project = "output/"+generate->originProjectName+"/"+generate->originProjectName+".prj";
    try {
        Project* proj = new Project(project,true);
        projectFile = proj->getParsedProject();
    } catch (...) {
        cout<<"error"<<endl;
    }
    string nameProj = project;
    uint pos = nameProj.find_last_of("/\\");
    nameProj = nameProj.substr(pos+1,nameProj.size());
    pos = nameProj.find_last_of(".");
    nameProj = nameProj.substr(0,pos);
    proj = nameProj;

    QString temp = field("comboTempEdit").toString();
    string tempPath = "templates/" + temp.toStdString();
    generate->var = generate->variablesFromTemplate(tempPath);
//    string fieldProjNameTemp = field("projectName").toString().toStdString();
//    if(!fieldProjNameTemp.empty())
//        fieldProjName = QString::fromStdString(fieldProjNameTemp);
    QString nameProje = "Edycja projektu: " + QString::fromStdString(nameProj);
    setTitle(nameProje);
    QString deviceName = (field("editDevName").toString());
    QString desc = QString(QString::fromUtf8("Edycja konfiguracji dla urządzenia: "));
    desc.append(deviceName);
    desc.append("\n Edycja zmiennych portowych");
    setSubTitle(desc);

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

    numberActualDev = generate->devicesNames.size()+1;
    if(numberPreviousDev != numberActualDev)
        portVarAndValues.clear();
    string deviceNameOld = (field("combo").toString().toStdString());
    string actualSection;
    map<string,vector<string>>::iterator iteratorVariables;
    for(iteratorVariables = generate->var.begin(); iteratorVariables != generate->var.end(); ++iteratorVariables)
        if(iteratorVariables != generate->var.find("project") && iteratorVariables != generate->var.find("device"))
        {
            actualSection = iteratorVariables->first  + "Edit";
            string rangeSec = (field(QString::fromStdString(actualSection)).toString()).toStdString();
            if(!rangeSec.empty())
            {
                QLabel *portType = new QLabel;
                QString namePort = QString::fromStdString(iteratorVariables->first);
                portType->setText(namePort);
                layout->addWidget(portType);

                bool existUndeclaredVar = true;
                while(existUndeclaredVar)
                {
                    if(iteratorVariables->second.empty())
                        existUndeclaredVar = false;
                    else
                    {
                        string nameVariable = iteratorVariables->second.front();
                        string nameVariableWithoutEqualSign = nameVariable + actualSection;
                        generate->usedProjectVariables.push_back(nameVariable);
                        QString description = QString::fromStdString(generate->commentVariable[nameVariable]);
                        nameVariable.append("=");

                        QLabel *portVar = new QLabel;
                        QString nameVar = QString::fromStdString(nameVariable);
                        portVar->setText(nameVar+" ("+description+")");
                        nameVar = QString::fromStdString(nameVariable + actualSection + "Edit");
                        QLineEdit *portTypeLineEdit = new QLineEdit();

                        layout->addWidget(portVar);
                        layout->addWidget(portTypeLineEdit);

                        registerField(nameVar, portTypeLineEdit);//+'*'
                        projFields.push_back(nameVariable + actualSection + "Edit");
                        connect(portTypeLineEdit, SIGNAL(textChanged(const QString &)),this, SIGNAL(completeChanged()));

                        vector<pair<string,string>>::iterator iterPortVarAndValues;
                        bool isVarInProjVarAndValues;
                        for(iterPortVarAndValues = portVarAndValues.begin(); iterPortVarAndValues != portVarAndValues.end(); ++iterPortVarAndValues)
                        {
                            isVarInProjVarAndValues = false;
                            if(iterPortVarAndValues->first == nameVariableWithoutEqualSign )
                            {
                                string a = iterPortVarAndValues->second;
                                QString b = QString::fromStdString(a);
                                portTypeLineEdit->setText(b);
                                isVarInProjVarAndValues = true;
                            }
                        }
                        if(portTypeLineEdit->text().toStdString().empty())
                        {
                            auto wholeFile = projectFile.find(deviceNameOld)->second;
                            for(auto a = wholeFile.begin(); a != wholeFile.end(); a++)
                            {
                                if(a->first + "Edit" == actualSection)
                                    if(a->second.first == nameVariable)
                                    {
                                        portTypeLineEdit->setText(QString::fromStdString(a->second.second));
                                        break;
                                    }
                            }
                        }
                        if(numberPreviousDev != numberActualDev || !isVarInProjVarAndValues)
                        {
                            pair<string,string> varAndValue;
                            varAndValue.first = nameVariableWithoutEqualSign;
                            varAndValue.second = portTypeLineEdit->text().toStdString();
                            portVarAndValues.push_back(varAndValue);
                        }
                        iteratorVariables->second.erase(iteratorVariables->second.begin());
                    }
                }
            }
        }
    setLayout(layout);

}

bool EditPortVarPage::isComplete() const
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
