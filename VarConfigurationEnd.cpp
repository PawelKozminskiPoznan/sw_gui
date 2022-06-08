#include "VarConfigurationEnd.h"
#include <QLayoutItem>
#include <QVBoxLayout>
#include <queue>

#include <QPushButton>
#include <QTimer>
#include <iostream>

VarConfigurationEnd::VarConfigurationEnd(QWidget *parent, GenerateProjectFile *gen, QWizard *w, vector<string>* usedProjVar, const string &path) : QWizardPage(parent)
{
    generate = gen;
    wizard = w;

    layout = nullptr;

    nextPage = -1;
    previousDev = " ";
    usedProjVarPage = usedProjVar;

    pathProgram = path;
    uint pos = pathProgram.find_last_of("/\\");
    pathProgram = pathProgram.substr(0, pos);
}

int VarConfigurationEnd::nextId() const
{
    return nextPage;
}


bool VarConfigurationEnd::validatePage()
{
    QString buttonText = QString::fromUtf8("Dalej");
    wizard->setButtonText(wizard->NextButton, buttonText);
    return true;
}

void VarConfigurationEnd::initializePage()
{
    generate->newDevProjVar = true;
    generate->newProjectConfig = true;
    generate->newDevDevVar = true;
    generate->newDevPortVar = true;
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
    QString nameProj = "Projekt: " + fieldProjName;
    setTitle(nameProj);
    QString deviceName = (field("devName").toString());
    QString desc = QString(QString::fromUtf8(""));
    //desc.append(deviceName);
    setSubTitle(desc);

    devAmount = (field("devicesAmount").toString()).toStdString();
    if(!devAmount.empty())
        deviceAmount = stoi(devAmount);
    else
        devAmount = to_string(deviceAmount);
    setFinalPage(true);

    QList<QWizard::WizardButton> l;
    //l << QWizard::BackButton <<  QWizard::NextButton << QWizard::FinishButton;
    l <<  QWizard::NextButton << QWizard::FinishButton;
    wizard->setButtonLayout(l);

    QPushButton *edit = new QPushButton(QString::fromUtf8("Edytuj projekt"));
    QPushButton *menu = new QPushButton(QString::fromUtf8("Strona główna"));


    connect(edit, SIGNAL(released()), this, SLOT(editButtonPress()));
    connect(menu, SIGNAL(released()), this, SLOT(menuButtonPress()));

    nextPage = -1;
    string allDev = "";
    bool firstElement = true;
    int counter = 0;
    while(!generate->devicesNames.empty())
    {
        counter++;
        if(!firstElement)
        {
            allDev += ", ";
        }
        firstElement = false;
        allDev += generate->devicesNames.front();
        generate->devicesNames.pop();
        if(deviceAmount == counter)
            break;
    }
    generate->declareProjectFileLine("names:", allDev, "project", "secondLine");
    generate->declareProjectFileLine("number of devices:", devAmount, "project", "firstLine");
    string projName = (field("projectName").toString()).toStdString();
    if(!projName.empty())
        generate->outputsFolders(projName);
    generate->writeToProjectFile();
    generate->clearConfigsProj();

    QLabel *info = new QLabel();
    QLabel *pathProj = new QLabel();
    QLabel *pathToProjFile = new QLabel();
    pathToProjFile->setTextInteractionFlags(Qt::TextInteractionFlag(1));
    QLabel *pathDev = new QLabel();
    QLabel *pathToDevFile = new QLabel();
    pathToDevFile->setTextInteractionFlags(Qt::TextInteractionFlag(1));
    layout->addWidget(info);
    layout->addWidget(new QLabel());
    layout->addWidget(pathProj);
    layout->addWidget(pathToProjFile);
    pathToProjFile->setWordWrap(true);
    layout->addWidget(new QLabel());
    layout->addWidget(pathDev);
    layout->addWidget(pathToDevFile);
    pathToDevFile->setWordWrap(true);
    layout->addWidget(new QLabel());
    //layout->addWidget(edit);
    edit->setDefault(false);
    menu->setDefault(false);
    layout->addWidget(menu);
    wizard->setOptions(QWizard::NoDefaultButton);
    //wizard->setOption(QWizard::NoDefaultButton,true);
    QTimer::singleShot(0, wizard->button(QWizard::FinishButton), SLOT(setFocus()));
    setLayout(layout);
    usedProjVarPage->clear();
    //setMaximumSize(500,450);
    try
    {
        projectPath = generate->pathProj;
        Project* proj = new Project(projectPath,true);
        map<string,stringstream> configs = proj->createConfigFiles(generate->portLimitations);
        generate->portLimitations.clear();
        proj->saveOutputFile(configs);
        info->setText(QString(QString::fromUtf8("Pliki konfiguracyjne zostały stworzone!")));
        pathProj->setText(QString(QString::fromUtf8("Ścieżka do pliku projektu: ")));
        pathToProjFile->setText(QString::fromStdString(pathProgram)+QString("\\")
                                        +QString::fromStdString(generate->pathProj));
        string folderProj = projectPath;
        uint pos = folderProj.find_last_of("/\\");
        folderProj = folderProj.substr(0, pos);
        folderProj.append("\\devices");
        pathDev->setText(QString(QString::fromUtf8("Ścieżka do plików konfiguracyjnych: ")));
        pathToDevFile->setText(QString::fromStdString(pathProgram)+QString("\\")
                                        +QString::fromStdString(folderProj));
    }
    catch (...)
    {
        info->setText(QString(QString::fromUtf8("Niepowodzenie...     Pliki konfiguracyjne nie zostały stworzone."//n
                                                "Problem z prawidłowym zczytaniem pliku projektu; może zawiera nie pełną lub niepoprawną konfigurację.")));
        info->setWordWrap(true);
        QString t = QString::fromUtf8("Komunikat błedu z pliku logs.txt odnośnie ładowania pliku projektu:");
        pathToProjFile->setText(t);
        pathToProjFile->setWordWrap(true);
        string text = getErrorLog();
        const char * c = text.c_str();
        t = QString(QString::fromUtf8(c));
        pathToDevFile->setText(t);
    }
}

void VarConfigurationEnd::editButtonPress()
{
    wizard->setOptions(nullptr);
    nextPage = MyWizard::Page_ExistProjEdit;
    wizard->next();
}

void VarConfigurationEnd::menuButtonPress()
{
    wizard->setOptions(nullptr);
    QList<QWizard::WizardButton> l;
    l << QWizard::BackButton <<  QWizard::NextButton << QWizard::FinishButton;
    wizard->setButtonLayout(l);
    wizard->setStartId(MyWizard::Page_Intro);
    wizard->restart();
}

string VarConfigurationEnd::getErrorLog()
{
    string path;
    size_t posSlash = generate->pathProj.find_last_of("/\\");
    path.append(generate->pathProj.begin()+int(posSlash+1),generate->pathProj.end()-4);//-4 jesli rozszerzenie .prj
    path = "output\\" + path + "\\logs";
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
    return "Problem z plikiem: " + path + " uniemożliwiający wyświetlenie komunikatu błedu";
}
