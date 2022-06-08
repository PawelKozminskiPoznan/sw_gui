#include "EditDeviceConfigurationPage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>

#include <dirent.h>
#include <sys/types.h>


EditDeviceConfigurationPage::EditDeviceConfigurationPage(QWidget *parent, GenerateProjectFile *gen, QWizard* w) : QWizardPage(parent)
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


bool EditDeviceConfigurationPage::validatePage()
{
    numberPreviousDev = numberActualDev;
    tempFile = "templates/" + field("comboTempEdit").toString();
    ifstream f (tempFile.toStdString());
    if (!f.is_open())
    {
        errorFile->setText(QString::fromUtf8("Nie można odtworzyć pliku!"));
        return false;
    }
    else
    {
        errorFile->setText("");
        if(!deviceNameLineEdit->text().toStdString().empty() && !field("comboTempEdit").toString().toStdString().empty())//templatePathLineEdit->text() != "")
        {
            if(!checkRepeatedDevName(deviceNameLineEdit->text().toStdString()))
            {
                devName = deviceNameLineEdit->text().toStdString();
                templatePath = field("comboTempEdit").toString();
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

int EditDeviceConfigurationPage::nextId() const
{
    return nextPage;
}

void EditDeviceConfigurationPage::initializePage()
{
    projFile = field("path").toString();
    string project = projFile.toStdString();
    if(project.empty())
    {
        project = "output/"+generate->originProjectName+"/"+generate->originProjectName+".prj";
        proj = field("editedProjName").toString().toStdString();
    }
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

    QString nameProjQ = "Edycja projektu: " + QString::fromStdString(nameProj);
    setTitle(nameProjQ);
    QString desc = QString(QString::fromUtf8("Edycja konfiguracji urządzenia."));
    setSubTitle(desc);

    deviceNameLabel = new QLabel();
    deviceNameLabel->setText(QString(QString::fromUtf8("Nazwa urządzenia:")));
    deviceNameLineEdit = new QLineEdit;
    editedDeviceName = field("combo").toString();
    deviceNameLineEdit->setText(editedDeviceName);
    deviceNameLabel->setBuddy(deviceNameLineEdit);

    templatePathLabel = new QLabel(tr("&Class name:"));
    templatePathLabel->setText(QString(QString::fromUtf8("Wybór pliku szablonu:")));
    comboTempEdit = new QComboBox();
    showTemplates();
    setTempFromProj();
    errorFile  = new QLabel();

    if(devName != deviceNameLineEdit->text().toStdString())
        numberActualDev = generate->devicesNames.size()+1;

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
    layout->addWidget(comboTempEdit);
    layout->addWidget(errorFile);
    setLayout(layout);

    registerField("editDevName", deviceNameLineEdit);
    registerField("comboTempEdit", comboTempEdit, "currentText", "activated");

    nextPage = MyWizard::Page_EditDeviceRanges;
    setFinalPage(false);
}

void EditDeviceConfigurationPage::cleanupPage()
{
    QString buttonText = QString::fromUtf8("Generuj");
    wizard->setButtonText(wizard->NextButton, buttonText);
}


bool EditDeviceConfigurationPage::checkRepeatedDevName(string name)
{
    vector<string> actualDevList = generate->getDevFromConfigFile();
    string oldname = field("combo").toString().toStdString();
    for(auto x: actualDevList)
        if(x == name && x != oldname)
            return true;
    return false;
}

void EditDeviceConfigurationPage::showTemplates()
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
            comboTempEdit->addItem(entry->d_name);
    }
    closedir(dir);
}

void EditDeviceConfigurationPage::setTempFromProj()
{
    string editedDeviceNameString = editedDeviceName.toStdString();
    auto wholeFile = projectFile.find(editedDeviceNameString)->second;
    auto x = wholeFile.find("template")->second;
    string tempFromProj = x.second;
    size_t v = tempFromProj.find_last_of("\\/");
    tempFromProj = tempFromProj.substr(v+1);

    int z = comboTempEdit->count();
    if(z>0)
        z--;
    for(z; z >= 0; z--)
    {
        comboTempEdit->setCurrentIndex(z);
        string currentTemp = comboTempEdit->currentText().toStdString();
        if(currentTemp == tempFromProj)
            return;
    }
    luckOfTemp(tempFromProj);
}

void EditDeviceConfigurationPage::luckOfTemp(string tempFile)
{
    infoWidow = new QWidget;
    infoWidow->resize(240,200);

    QVBoxLayout* lRepeated = new QVBoxLayout;
    QLabel* info = new QLabel();
    string text;
    QPushButton *ok = new QPushButton("Ok");
    text = "Uwaga! Pliku szablonu: " + tempFile + " nie ma w folderze templates/...";

    const char * c = text.c_str();
    QString t = QString(QString::fromUtf8(c));
    connect(ok, SIGNAL(released()), this, SLOT(okPress()));
    info->setText(t);
    lRepeated->addWidget(info);
    ok->setDefault(true);
    lRepeated->addWidget(ok);
    infoWidow->setLayout(lRepeated);
    infoWidow->show();
    wizard->button(QWizard::NextButton)->setDisabled(true);
}

void EditDeviceConfigurationPage::okPress()
{
    wizard->button(QWizard::NextButton)->setDisabled(false);
    infoWidow->hide();
}
