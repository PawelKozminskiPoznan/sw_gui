#include "ExistProjEditVar.h"
#include "MyWizard.h"

#include <QLabel>
#include <QLineEdit>

#include <iostream>

ExistProjEditVar::ExistProjEditVar(QWidget *parent, GenerateProjectFile* generateFile, QWizard *w) : QWizardPage(parent)
{
    wizard = w;
    setSubTitle(tr("Edycja zmiennych projektowych."));

    nextPage = MyWizard::Page_ExistProjDone;
    generate = generateFile;
    layout = nullptr;
}

int ExistProjEditVar::nextId() const
{
    return nextPage;
}

bool ExistProjEditVar::validatePage()
{
    auto wholeFile = projectFile.find("projectVar")->second;
    for(auto a = wholeFile.begin(); a != wholeFile.end(); ++a)
    {
        string nameVariable = a->second.first;
        string varValue = (field(QString::fromStdString(nameVariable)).toString()).toStdString();
        generate->declareProjectVar(nameVariable, varValue);
    }
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
    wizard->restart();
    return true;
}

void ExistProjEditVar::initializePage()
{
    QString projFile = field("path").toString();
    string project = projFile.toStdString();
    string proj;
    if(project.empty())
        project = "output/"+generate->originProjectName+"/"+generate->originProjectName+".prj";
    try {
        Project* proj = new Project(project,true);
        projectFile = proj->getParsedProject();
    } catch (...) {

    }
    nameProj = project;
    uint pos = nameProj.find_last_of("/\\");
    nameProj = nameProj.substr(++pos,nameProj.size());
    pos = nameProj.find_last_of(".");
    nameProj = nameProj.substr(0,pos);
    proj = nameProj;

    QString nameProjQ = "Projekt: " + QString::fromStdString(nameProj);
    setTitle(nameProjQ);
    QString desc = QString(QString::fromUtf8("Edycja zmiennych projektowych"));
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
    generate->loadConfigsProj(projectFile);
    map<string,multimap<string,pair<string,string>>>::iterator fileIter;
    for(fileIter = projectFile.begin(); fileIter != projectFile.end(); ++fileIter)
    {
        string devicesAndProj = fileIter->first;
        if(devicesAndProj != "project" && devicesAndProj != "projectVar")
        {
            auto devData = projectFile[devicesAndProj].find("template");
            string tempPath = devData->second.second;
//            if(tempPath.at(0) == ' ')
//                tempPath = tempPath.substr(1);
            generate->commentVariable = generate->commentFromTemplate(tempPath);
        }
    }
    auto wholeFile = projectFile.find("projectVar")->second;
    for(auto a = wholeFile.begin(); a != wholeFile.end(); ++a)
    {
        string nameVariable = a->second.first;
        string nameVarWithoutEqualSign = nameVariable.substr(0,nameVariable.size()-1);
        QString description = QString::fromStdString(generate->commentVariable[nameVarWithoutEqualSign]);
        QLineEdit *portTypeLineEdit = new QLineEdit(QString::fromStdString(a->second.second));
        registerField(QString::fromStdString(nameVariable), portTypeLineEdit);
        //nameVariable.append("=");
        QLabel *portType = new QLabel;
        QString nameVar = QString::fromStdString(nameVariable);

        portType->setText(nameVar+" ("+description+")");

        layout->addWidget(portType);
        layout->addWidget(portTypeLineEdit);
    }
    setLayout(layout);
}

void ExistProjEditVar::cleanupPage()
{
    QString buttonText = QString::fromUtf8("Generuj");
    wizard->setButtonText(wizard->NextButton, buttonText);
}
