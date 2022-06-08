#include "AddDevPortVar.h"

#include "MyWizard.h"
#include <QLabel>
#include <QVBoxLayout>
#include <iostream>

AddDevPortVar::AddDevPortVar(QWidget *parent, GenerateProjectFile *gen, QWizard *w) : QWizardPage(parent)
{
    generate = gen;
    wizard = w;
    layout = nullptr;
    numberActualDev = 0;
    numberPreviousDev = 0;

    nextPage = MyWizard::Page_AddDevEnd;
}

int AddDevPortVar::nextId() const
{
    return nextPage;
}

bool AddDevPortVar::validatePage()
{
    QString temp = field("comboTempAdd").toString();
    string tempPath = "templates/" + temp.toStdString();
    generate->var = generate->variablesFromTemplate(tempPath);
    string actualSection;
    map<string,vector<string>>::iterator iteratorVariables;
    for(iteratorVariables = generate->var.begin(); iteratorVariables != generate->var.end(); ++iteratorVariables)
        if(iteratorVariables != generate->var.find("project") && iteratorVariables != generate->var.find("device"))
        {
            actualSection = iteratorVariables->first;
            string rangeSec = (field(QString::fromStdString(actualSection+"Add")).toString()).toStdString();
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
                        nameVar += "Add";
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

void AddDevPortVar::initializePage()
{
    projFields.clear();
    QString temp = field("comboTempAdd").toString();
    string tempPath = "templates/" + temp.toStdString();
    generate->var = generate->variablesFromTemplate(tempPath);
    string fieldProjNameTemp = field("editedProjName").toString().toStdString();
    if(!fieldProjNameTemp.empty())
        fieldProjName = QString::fromStdString(fieldProjNameTemp);
    QString nameProjQ = "Edycja projektu: " + QString::fromStdString(field("editedProjName").toString().toStdString());
    setTitle(nameProjQ);
    QString deviceName = (field("newDevName").toString());
    QString desc = QString(QString::fromUtf8("Tworzenie konfiguracji dla urządzenia: "));
    desc.append(deviceName);
    desc.append("\n Inicjacja zmiennych portowych");
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
    string actualSection;
    map<string,vector<string>>::iterator iteratorVariables;
    for(iteratorVariables = generate->var.begin(); iteratorVariables != generate->var.end(); ++iteratorVariables)
        if(iteratorVariables != generate->var.find("project") && iteratorVariables != generate->var.find("device"))
        {
            actualSection = iteratorVariables->first;
            string rangeSec = (field(QString::fromStdString(actualSection+"Add")).toString()).toStdString();
            if(!rangeSec.empty())
            {
                QLabel *portType = new QLabel;
                QString namePort = QString::fromStdString(actualSection);
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
                        nameVar = QString::fromStdString(nameVariable + actualSection + "Add");
                        QLineEdit *portTypeLineEdit = new QLineEdit();

                        layout->addWidget(portVar);
                        layout->addWidget(portTypeLineEdit);

                        registerField(nameVar, portTypeLineEdit);//+'*'
                        projFields.push_back(nameVariable + actualSection + "Add");
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

bool AddDevPortVar::isComplete() const
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
