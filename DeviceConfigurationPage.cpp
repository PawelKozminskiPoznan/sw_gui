#include "DeviceConfigurationPage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>
#include <queue>

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>


DeviceConfigurationPage::DeviceConfigurationPage(QWidget *parent, GenerateProjectFile *gen, QWizard* w) : QWizardPage(parent)
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


bool DeviceConfigurationPage::validatePage()
{
    numberPreviousDev = numberActualDev;
    tempFile = "templates/" + field("comboTemp").toString();
    ifstream f (tempFile.toStdString());
    if (!f.is_open())
    {
        errorFile->setText(QString::fromUtf8("Nie można odtworzyć pliku!"));
        return false;
    }
    else
    {
        errorFile->setText("");
        string oldDevName = devName;
        devName = deviceNameLineEdit->text().toStdString();
        if(!deviceNameLineEdit->text().toStdString().empty())
        {
            templatePath = field("comboTemp").toString();
            queue<string> tempDevicesNames = generate->devicesNames;
            bool del = false;
            if(!generate->newDev && generate->devicesNames.size() >= numberActualDev)
            {
                tempDevicesNames = generate->devicesNames;
                queue<string> withoutDeleteName;
                while (!tempDevicesNames.empty())
                {
                    string nameDev = tempDevicesNames.front();
                    if(oldDevName == nameDev && !del)
                        del = true;
                    else
                        withoutDeleteName.push(nameDev);
                    tempDevicesNames.pop();
                }
                if(del)
                    generate->devicesNames = withoutDeleteName;
            }
            bool is = false;
            tempDevicesNames = generate->devicesNames;
            while (!tempDevicesNames.empty())
            {
                string nameDev = tempDevicesNames.front();
                if(devName == nameDev){
                    is = true;
                    break;
                }
                tempDevicesNames.pop();
            }
            if(!is)
                generate->devicesNames.push(devName);
            else
            {
                repeatedDevConf();
                return false;
            }
            generate->newDev = false;
            generate->newProjectConfig = false;
            return true;
        }
        else
        {
            errorFile->setText(QString::fromUtf8("Wprowadź wartości!"));
            return false;
        }
    }
}

int DeviceConfigurationPage::nextId() const
{
    return nextPage;
}

void DeviceConfigurationPage::initializePage()
{
    wizard->setButtonText(wizard->NextButton, "Dalej");

    string devAmount = (field("devicesAmount").toString()).toStdString();
    if(!devAmount.empty())
        numberAllDev = devAmount;

    string fieldProjNameTemp = field("projectName").toString().toStdString();
    if(!fieldProjNameTemp.empty())
        fieldProjName = QString::fromStdString(fieldProjNameTemp);
    QString nameProj;
    QString desc = QString(QString::fromUtf8("Rozpoczęcie konfiguracji urządzenia."));
    setSubTitle(desc);

    deviceNameLabel = new QLabel();
    deviceNameLabel->setText(QString(QString::fromUtf8("Nazwa urządzenia:")));
    deviceNameLineEdit = new QLineEdit;
    deviceNameLabel->setBuddy(deviceNameLineEdit);

    templatePathLabel = new QLabel(tr("&Class name:"));
    templatePathLabel->setText(QString(QString::fromUtf8("Wybór pliku szablonu:")));

    comboTemp = new QComboBox();
    showTemplates();

    errorFile  = new QLabel();

    if(generate->devicesNames.empty())
    {
        numberActualDev = 1;
        numberPreviousDev = 0;
    }
    else if(generate->newDev)
        numberActualDev = generate->devicesNames.size()+1;
    QLabel *numberDevice = new QLabel();

    string actualDevNr = to_string(numberActualDev);
    string infoNrDev = "Urządzenie nr:";
    infoNrDev.append(actualDevNr+" z "+numberAllDev);
    const char * c = infoNrDev.c_str();
    QString nrDev = QString(QString::fromUtf8(c));
    numberDevice->setText(nrDev);
    if(numberActualDev > 1)
        nameProj = "Tworzenie projektu: " + fieldProjName;
    else
        nameProj = "Nowy projekt: " + fieldProjName;
    setTitle(nameProj);

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
    layout->addWidget(comboTemp);
    layout->addWidget(errorFile);
    if(!generate->newDevProjVar)
    {
        QLabel *info = new QLabel(QString::fromUtf8("Info: W celu zmiany nazwy projektu i/lub liczby urządzeń kliknij przycisk 'Wstecz' "));
        info->setWordWrap(true);
        layout->addWidget(info);
    }
    setLayout(layout);

    registerField("devName", deviceNameLineEdit);
    registerField("comboTemp", comboTemp, "currentText", "activated");

    nextPage = MyWizard::Page_DevRanges;
    setFinalPage(false);

    if(!generate->newProjectConfig)//numberPreviousDev == numberActualDev &&
    {
        deviceNameLineEdit->setText(QString::fromStdString(devName));
        comboTemp->setCurrentText(templatePath);
    }
}

void DeviceConfigurationPage::repeatedDevConf()
{
    infoWindow = new QWidget;
    infoWindow->resize(240,200);

    QVBoxLayout* lRepeated = new QVBoxLayout;
    QLabel* info = new QLabel();
    string text;
    QPushButton *ok = new QPushButton("Ok");
    text = "Nazwa urządzenia: " + devName + " została wykorzystana wcześniej dla innego urządzenia."+"\n"+
                                            "Wpisz inną nazwe urządzenia!";
    const char * c = text.c_str();
    QString t = QString(QString::fromUtf8(c));
    connect(ok, SIGNAL(released()), this, SLOT(okPress()));
    infoWindow->setAttribute(Qt::WA_DeleteOnClose,true);
    connect(infoWindow , SIGNAL(destroyed()), this,SLOT(okP()));
    info->setText(t);
    lRepeated->addWidget(info);
    ok->setDefault(true);
    lRepeated->addWidget(ok);
    infoWindow->setLayout(lRepeated);
    infoWindow->show();
    wizard->button(QWizard::NextButton)->setDisabled(true);
}

void DeviceConfigurationPage::okPress()
{
    wizard->button(QWizard::NextButton)->setDisabled(false);
    infoWindow->hide();
}

void DeviceConfigurationPage::okP()
{
    wizard->button(QWizard::NextButton)->setDisabled(false);
}

void DeviceConfigurationPage::showTemplates()
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
            comboTemp->addItem(entry->d_name);
    }
    closedir(dir);
}
