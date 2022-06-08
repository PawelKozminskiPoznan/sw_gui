#include "ExistProjEditPage.h"
#include "MyWizard.h"

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QVariant>
#include <iostream>


ExistProjEditPage::ExistProjEditPage(QWidget *parent, GenerateProjectFile *gen, QWizard *w) : QWizardPage(parent)
{    
    wizard = w;
    generate = gen;
    nextPage = MyWizard::Page_ExistProjDone;

    QString text = QString(QString::fromUtf8("Edycja konfiguracji projektu"));
    string textS = text.toStdString();
    const char * c = textS.c_str();
    setTitle(tr(c));
    layout = nullptr;
    addClicked = false;
    addNext = false;
    editNext = false;
}

int ExistProjEditPage::nextId() const
{
    return nextPage;
}


bool ExistProjEditPage::validatePage()
{
    if(comboDel->currentText().toStdString().empty() && addClicked != true)//(comboDel->currentIndex() == 0)
    {
        infoWindow = new QWidget;
        infoWindow->resize(240,200);

        QVBoxLayout* lDelete = new QVBoxLayout;
        QLabel* info = new QLabel();
        info->setText(QString::fromUtf8("Projekt musi zawierać konfiguracje dla przynajmniej jednego urządzenia!"));
        lDelete->addWidget(info);
        QPushButton *ok = new QPushButton("Ok");
        connect(ok, SIGNAL(released()), this, SLOT(closeWindowLuckDev()));
        infoWindow->setAttribute(Qt::WA_DeleteOnClose,true);
        connect(infoWindow , SIGNAL(destroyed()), this,SLOT(close()));
        ok->setDefault(true);
        lDelete->addWidget(ok);
        infoWindow->setLayout(lDelete);
        infoWindow->show();
        addNext = false;
        editNext = false;
        wizard->button(QWizard::NextButton)->setDisabled(true);
        return false;
    }
    else
    {
        addClicked = false;
        QString buttonText = QString::fromUtf8("Dalej");
        wizard->setButtonText(wizard->NextButton, buttonText);
        if(!(addNext || editNext))
           nextPage = MyWizard::Page_ExistProjDone;
        addNext = false;
        editNext = false;
        generate->outputsFolders(proj);
        generate->writeToProjectFile();
//        Project* proj = new Project(project,true);
//        projectFile = proj->getParsedProject();
        generate->loadConfigsProj(projectFile);
        return true;
    }
}

void ExistProjEditPage::initializePage()
{
    QString buttonText = QString::fromUtf8("Generuj");
    wizard->setButtonText(wizard->NextButton, buttonText);

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
        layout = new QGridLayout;
    projFile = field("path").toString();
    string project = projFile.toStdString();

    QLabel* projNameLabel = new QLabel();
    projNameLabel->setText(QString("Nazwa pliku projektu: "));
    projNameLineEdit = new QLineEdit;
    registerField("editedProjName", projNameLineEdit);

    if(project.empty())
    {
        project = "output/"+proj+"/"+proj+".prj";
    }

    string nameProj = project;
    uint pos = nameProj.find_last_of("/\\");
    nameProj = nameProj.substr(++pos);
    pos = nameProj.find_last_of(".");
    nameProj = nameProj.substr(0,pos);
    projNameLineEdit->setText(QString::fromStdString(nameProj));
    connect(projNameLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(saveNewProjName()));

    proj = nameProj;

    int row = 1;
    layout->addWidget(projNameLabel,row,1);
    layout->addWidget(projNameLineEdit,row,2);

    QPushButton *editProjVarButton = new QPushButton("Edytuj zmienne projektu");
    connect(editProjVarButton, SIGNAL(released()), this, SLOT(editProjVar()));
    layout->addWidget(editProjVarButton,++row,1,1,-1);

    QLabel* comandLabel = new QLabel();
    layout->addWidget(comandLabel,++row,1,1,-1);
    QString text = QString(QString::fromUtf8("Edytuj konfigurację urządzenia:"));
    comandLabel->setText(text);

    QPushButton *editButton = new QPushButton(QString(QString::fromUtf8("Edytuj konfigurację urządzenia")));
    connect(editButton, SIGNAL(released()), this, SLOT(edit()));
    bool error = false;
    try {
        Project* proj = new Project(project,true);
        projectFile = proj->getParsedProject();
    } catch (...) {
        errorReadError();
        wizard->setStartId(MyWizard::Page_Intro);
        wizard->restart();
        error = true;
    }
    combo = new QComboBox();
    comboDel = new QComboBox();
    map<string,multimap<string,pair<string,string>>>::iterator wholeFile;
    wholeFile = projectFile.find("project");
    for(wholeFile = projectFile.begin(); wholeFile != projectFile.end(); ++wholeFile)
    {
        string a = wholeFile->first;
        QString b = QString::fromStdString(a);
        if(a != "project" && a != "projectVar")
        {
            combo->addItem(b);
            comboDel->addItem(b);
        }
    }
    layout->addWidget(combo,++row,1);
    layout->addWidget(editButton,row,2);
    combo->setCurrentIndex(0);
    combo->setCurrentText(combo->itemText(combo->currentIndex()));

    orNewDev = new QLabel();
    if(!error)
    {
        wholeFile = projectFile.find("project");
        string amountDev = projectFile["project"].find("firstLine")->second.second;
        orNewDev->setText(QString::fromUtf8("Liczba konfiguracji urządzeń: ") + QString::fromStdString(amountDev));
    }
    layout->addWidget(orNewDev,++row,1);

    QPushButton *newDev = new QPushButton(QString::fromUtf8("Dodaj konfigurację urządzenie"));
    QPushButton *deleteDev = new QPushButton(QString::fromUtf8("Usuń konfigurację urządzenia"));

    connect(newDev, SIGNAL(released()), this, SLOT(addDevButtonPress()));
    connect(deleteDev, SIGNAL(released()), this, SLOT(deleteDevButtonPress()));

    registerField("combo", combo, "currentText", "activated");

    layout->addWidget(newDev,++row,1,1,-1);
    layout->addWidget(comboDel,++row,1);
    layout->addWidget(deleteDev,row,2);

    if(!error)
        setLayout(layout);

    if(error)//usuwanie poprzedniego layout'u
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

    generate->loadConfigsProj(projectFile);
    nextPage = MyWizard::Page_ExistProjEdit;
}


void ExistProjEditPage::edit()
{
    generate->loadConfigsProj(projectFile);
    nextPage = MyWizard::Page_EditDeviceConfiguration;
    editNext = true;
    wizard->next();
}

void ExistProjEditPage::editProjVar()
{
    generate->loadConfigsProj(projectFile);
    nextPage = MyWizard::Page_ExistProjEditVar;
    editNext = true;
    wizard->next();
}

void ExistProjEditPage::addDevButtonPress()
{
    generate->loadConfigsProj(projectFile);
    nextPage = MyWizard::Page_AddDeviceConfiguration;
    addClicked = true;
    addNext = true;
    wizard->next();
}

void ExistProjEditPage::deleteDevButtonPress()
{
    infoWindow = new QWidget;
    infoWindow->resize(240,200);

    QVBoxLayout* lDelete = new QVBoxLayout;
    QLabel* info = new QLabel();
    string devToDel = comboDel->currentText().toStdString();
    string text;
    QPushButton *ok = new QPushButton("Ok");
    if(devToDel.empty())
    {
        text = "Projekt nie zawiera konfiguracji żadnego urządzenia!";
        connect(ok, SIGNAL(released()), this, SLOT(closeWindowLuckDev()));
    }
    else
    {
        text = "Czy napewno chcesz usuną konfigurację urządzenia : ";
        text.append(devToDel);
        text.append(" ?");
        connect(ok, SIGNAL(released()), this, SLOT(closeWindow()));
    }
    infoWindow->setAttribute(Qt::WA_DeleteOnClose,true);
    connect(infoWindow , SIGNAL(destroyed()), this,SLOT(close()));
    const char * c = text.c_str();
    QString t = QString(QString::fromUtf8(c));
    info->setText(t);
    lDelete->addWidget(info);
    ok->setDefault(true);
    lDelete->addWidget(ok);
    if(!devToDel.empty())
    {
        QPushButton *anuluj = new QPushButton("Anuluj");
        lDelete->addWidget(anuluj);
        connect(anuluj, SIGNAL(released()), this, SLOT(closeWindowLuckDev()));
    }
    infoWindow->setLayout(lDelete);
    wizard->button(QWizard::NextButton)->setDisabled(true);
    infoWindow->show();
}

void ExistProjEditPage::closeWindow()
{
    string devToDel = comboDel->currentText().toStdString();
    generate->eraseDevProjectFileLine(devToDel);
    generate->findAndDeleteDevName(devToDel);
    generate->outputsFolders(proj);
    generate->writeToProjectFile();
    infoWindow->hide();
    comboDel->clear();
    combo->clear();
    actualDevList = generate->getDevFromConfigFile();
    while(!actualDevList.empty())
    {
        string devName = actualDevList.back();
        actualDevList.pop_back();
        comboDel->addItem(QString::fromStdString(devName));
        combo->addItem(QString::fromStdString(devName));
    }
    try {
        string project = "output/"+proj+"/"+proj+".prj";
        generate->originProjectName = proj;
        Project* proj = new Project(project,true);
        projectFile = proj->getParsedProject();
        generate->loadConfigsProj(projectFile);
    } catch (...) {
        cout<<"error"<<endl;
    }
    string amountDev = projectFile["project"].find("firstLine")->second.second;
    orNewDev->setText(QString::fromUtf8("Liczba konfiguracji urządzeń: ") + QString::fromStdString(amountDev));
    wizard->button(QWizard::NextButton)->setDisabled(false);
    generate->editedProj = true;
}

void ExistProjEditPage::closeWindowLuckDev()
{
    infoWindow->hide();
    wizard->button(QWizard::NextButton)->setDisabled(false);
}

void ExistProjEditPage::close()
{
    wizard->button(QWizard::NextButton)->setDisabled(false);
}

void ExistProjEditPage::saveNewProjName()
{
    if(!projNameLineEdit->text().toStdString().empty())
    {
        proj = projNameLineEdit->text().toStdString();
        generate->editedProj = true;
    }
}


void ExistProjEditPage::errorReadError()
{
    infoWindow = new QWidget;
    infoWindow->resize(240,200);

    QVBoxLayout* lRepeated = new QVBoxLayout;
    QLabel* info = new QLabel();
    string text;
    QPushButton *ok = new QPushButton("Ok");
    text = "Niepowodzenie...Nie można odtworzyć pliku projektu. Możliwe nieprawidłowości w strukturze pliku. \n";
    text += getErrorLog();
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
    wizard->button(QWizard::NextButton)->setDisabled(true);
    infoWindow->show();
}

void ExistProjEditPage::okPress()
{
    infoWindow->hide();
    QString buttonText = QString::fromUtf8("Dalej");
    wizard->setButtonText(wizard->NextButton, buttonText);
    wizard->button(QWizard::NextButton)->setDisabled(false);
    //wizard->setStartId(MyWizard::Page_Intro);
    //wizard->restart();
}

void ExistProjEditPage::okP()
{
    QString buttonText = QString::fromUtf8("Dalej");
    wizard->setButtonText(wizard->NextButton, buttonText);
    wizard->button(QWizard::NextButton)->setDisabled(false);
    //wizard->setStartId(MyWizard::Page_Intro);
    //wizard->restart();
}

void ExistProjEditPage::cleanupPage()
{
    QString buttonText = QString::fromUtf8("Dalej");
    wizard->setButtonText(wizard->NextButton, buttonText);
}

string ExistProjEditPage::getErrorLog()
{
    string path;
    string project = field("editedProjName").toString().toStdString();
    path = "output\\" + project + "\\logs";
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
    return "Problem z plikiem " + path;
}
