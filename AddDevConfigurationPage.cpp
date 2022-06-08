#include "AddDevConfigurationPage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>

AddDevConfigurationPage::AddDevConfigurationPage(QWidget *parent, GenerateProjectFile *gen, QWizard* w) : QWizardPage(parent)
{
    generate = gen;
    wizard = w;

    layout = nullptr;
    devName = "";
    templatePath = "";

    numberActualDev = 1;
    numberPreviousDev = 1;
    fieldProjName = "";
}


bool AddDevConfigurationPage::validatePage()
{
    numberPreviousDev = numberActualDev;
    tempFile = "templates/" + field("comboTempAdd").toString();
    ifstream f (tempFile.toStdString());
    if (!f.is_open())
    {
        errorFile->setText(QString::fromUtf8("Nie można odtworzyć pliku!"));
        return false;
    }
    else
    {
        errorFile->setText("");
        if(!deviceNameLineEdit->text().toStdString().empty() && !field("comboTempAdd").toString().toStdString().empty())
        {
            if(!checkRepeatedDevName(deviceNameLineEdit->text().toStdString()))
            {
                devName = deviceNameLineEdit->text().toStdString();
                templatePath = field("comboTempAdd").toString();
                return true;
            }
            else
            {
                errorFile->setText(QString::fromUtf8("Powtórzona nazwa urządzenia!"));
                return false;
            }
        }
        else
        {
            errorFile->setText(QString::fromUtf8("Wprowadź wartości!"));
            return false;
        }
    }
}

int AddDevConfigurationPage::nextId() const
{
    return nextPage;
}

void AddDevConfigurationPage::initializePage()
{
    projFile = field("path").toString();
    string project = projFile.toStdString();
    if(project.empty())
        project = "output/"+generate->originProjectName+"/"+generate->originProjectName+".prj";
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
    QString nameProjQ = "Edycja projektu: " + QString::fromStdString(field("editedProjName").toString().toStdString());
    setTitle(nameProjQ);
    QString desc = QString(QString::fromUtf8("Dodawanie konfiguracji nowego urządzenia."));
    setSubTitle(desc);

    deviceNameLabel = new QLabel();
    deviceNameLabel->setText(QString(QString::fromUtf8("Nazwa urządzenia:")));
    deviceNameLineEdit = new QLineEdit;
    QString editedDeviceName = field("combo").toString();
    deviceNameLabel->setBuddy(deviceNameLineEdit);

    templatePathLabel = new QLabel(tr("&Class name:"));
    templatePathLabel->setText(QString(QString::fromUtf8("Wybór pliku szablonu:")));
    comboTempAdd = new QComboBox();
    showTemplates();

    errorFile  = new QLabel();

    if(devName != deviceNameLineEdit->text().toStdString())
        numberActualDev = generate->devicesNames.size()+2; //+2 ???

    QLabel *numberDevice = new QLabel();

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

    layout->addWidget(numberDevice);
    layout->addWidget(deviceNameLabel);
    layout->addWidget(deviceNameLineEdit);
    layout->addWidget(templatePathLabel);
    layout->addWidget(comboTempAdd);
    layout->addWidget(errorFile);
    setLayout(layout);

    registerField("newDevName", deviceNameLineEdit);
    registerField("comboTempAdd", comboTempAdd, "currentText", "activated");

    nextPage = MyWizard::Page_AddDeviceRanges;
    setFinalPage(false);

    QString buttonText = QString::fromUtf8("Dalej");
    wizard->setButtonText(wizard->NextButton, buttonText);
}


void AddDevConfigurationPage::cleanupPage()
{
    QString buttonText = QString::fromUtf8("Generuj");
    wizard->setButtonText(wizard->NextButton, buttonText);
}


bool AddDevConfigurationPage::checkRepeatedDevName(string name)
{
    vector<string> actualDevList = generate->getDevFromConfigFile();
    for(auto x: actualDevList)
        if(x == name)
            return true;
    return false;
}


void AddDevConfigurationPage::showTemplates()
{
    struct dirent *entry;
    DIR *dir = opendir("templates/");
    if (dir == nullptr)
    {
        //return;
    }
    while ((entry = readdir(dir)) != nullptr)
    {
        string x = entry->d_name;
        size_t found = x.find(".cfg");
        if (found!=std::string::npos)
            comboTempAdd->addItem(entry->d_name);
    }
    closedir(dir);
}
