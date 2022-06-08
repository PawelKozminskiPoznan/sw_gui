#include "Verifyproj.h"

#include <QFileDialog>
#include <QPushButton>
#include <QTimer>

VerifyProj::VerifyProj(QWidget *parent, QWizard *w, const string& path) : QWizardPage(parent)
{
    layout = nullptr;
    setTitle(tr("Weryfikacja konfiguracji przełącznika"));
    setSubTitle(tr("Ładowanie pliku projektu oraz pliku z konfiguracją urządzenia"));
    pathProgram = path;
    wizard = w;
}

int VerifyProj::nextId() const
{
    return nextPage;
}

bool VerifyProj::validatePage()
{
    ifstream f;
    ifstream f2;
    string firstConfig = pathToFirstConfigFileLineEdit->text().toStdString();
    string secondConfig = pathToSecondConfigFileLineEdit->text().toStdString();
    f.open(firstConfig, ios::in);
    f2.open(secondConfig, ios::in);
    if (!f.is_open())
    {
        confFile = QString::fromStdString(firstConfig);
        errorCommunicate();
        return false;
    }
    else if(!f2.is_open())
    {
        confFile = QString::fromStdString(secondConfig);
        errorCommunicate();
        return false;
    }
    else
    return true;
}

void VerifyProj::initializePage()
{
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

    QLabel *label = new QLabel(QString::fromUtf8("Weryfikacja polega na porównaniu:\n1. Konfiguracji konkretnego urządzenia np. wygenerowanego z pliku projektu (podstawowa)\n2. Konfiguracją przełącznika np. zgraną z urządzenia (weryfikowana/sprawdzana)\n"));

    label->setWordWrap(true);

    infoForUserLabel = new QLabel();
    infoForUserLabel->setText(QString(QString::fromUtf8("Ścieżka do pliku z konfiguracją podstawową:")));
    pathToFirstConfigFileLineEdit = new QLineEdit(this);
    infoForUserLabel->setBuddy(pathToFirstConfigFileLineEdit);

    browse = new QPushButton("Szukaj...");
    connect(browse, SIGNAL(released()), this, SLOT(browseButtonPress()));

    pathToFirstConfigFileLineEdit->setEchoMode(QLineEdit::Normal);

    infoForUserLabel2 = new QLabel();
    infoForUserLabel2->setText(QString(QString::fromUtf8("Ścieżka do pliku z konfiguracją weryfikowaną:")));
    pathToSecondConfigFileLineEdit = new QLineEdit(this);
    infoForUserLabel->setBuddy(pathToSecondConfigFileLineEdit);

    browse2 = new QPushButton("Szukaj...");
    connect(browse2, SIGNAL(released()), this, SLOT(browseButtonPress2()));

    pathToFirstConfigFileLineEdit->setEchoMode(QLineEdit::Normal);

    layout->addWidget(label);
    layout->addWidget(infoForUserLabel);
    layout->addWidget(pathToFirstConfigFileLineEdit);
    layout->addWidget(browse);
    layout->addWidget(infoForUserLabel2);
    layout->addWidget(pathToSecondConfigFileLineEdit);
    layout->addWidget(browse2);
    setLayout(layout);

    QTimer::singleShot(0, browse, SLOT(setFocus()));

    nextPage = MyWizard::Page_VerifyFiles;

    registerField("pathFirstConfig*", pathToFirstConfigFileLineEdit);
    registerField("pathSecondConfig*", pathToSecondConfigFileLineEdit);

    //connect(pathToFirstConfigFileLineEdit, SIGNAL(textEdited(QString)), this, SLOT(textLineChange()));
    //connect(pathToSecondConfigFileLineEdit, SIGNAL(textEdited(QString)), this, SLOT(textLineChange()));
}

void VerifyProj::browseButtonPress()
{
    CheckerFileProject checkFolder;
    size_t lastSlash = pathProgram.find_last_of("\\");
    string pathProjects;
    pathProjects.assign(pathProgram.begin(), pathProgram.begin()+int(lastSlash));
    pathProjects += "\\output";
    if(checkFolder.exsistFolder(pathProjects))
        confFile = QFileDialog::getOpenFileName(this, "Wybierz plik konfiguracji podstawowej",QString::fromStdString(pathProjects));
    else
        confFile = QFileDialog::getOpenFileName(this, "Wybierz plik konfiguracji podstawowej",QString::fromStdString(pathProgram));
    pathToFirstConfigFileLineEdit->setText(confFile);

    wizard->button(QWizard::NextButton)->setFocus();
}

void VerifyProj::browseButtonPress2()
{
    CheckerFileProject checkFolder;
    size_t lastSlash = pathProgram.find_last_of("\\");
    string pathProjects;
    pathProjects.assign(pathProgram.begin(), pathProgram.begin()+int(lastSlash));
    pathProjects += "\\output";
    if(checkFolder.exsistFolder(pathProjects))
        confFile = QFileDialog::getOpenFileName(this, "Wybierz plik konfiguracji weryfikowanej",QString::fromStdString(pathProjects));
    else
        confFile = QFileDialog::getOpenFileName(this, "Wybierz plik konfiguracji weryfikowanej",QString::fromStdString(pathProgram));
    pathToSecondConfigFileLineEdit->setText(confFile);

    wizard->button(QWizard::NextButton)->setFocus();
}


void VerifyProj::errorCommunicate()
{
    infoWindow = new QWidget;
    infoWindow->resize(240,200);

    QVBoxLayout* lDelete = new QVBoxLayout;
    QLabel* info = new QLabel();
    string text;
    QPushButton *ok = new QPushButton("Ok");
    QString t = QString::fromUtf8("Błąd podczas próby otworzenia pliku konfiguracji:\n");
    t += confFile;
    info->setText(t);
    connect(ok, SIGNAL(released()), this, SLOT(closeWindow()));
    infoWindow->setAttribute(Qt::WA_DeleteOnClose,true);
    connect(infoWindow , SIGNAL(destroyed()), this,SLOT(close()));
    lDelete->addWidget(info);
    ok->setDefault(true);
    lDelete->addWidget(ok);
    infoWindow->setLayout(lDelete);
    wizard->button(QWizard::NextButton)->setDisabled(true);
    wizard->button(QWizard::BackButton)->setDisabled(true);
    infoWindow->show();
}

void VerifyProj::closeWindow()
{
    wizard->button(QWizard::NextButton)->setDisabled(false);
    wizard->button(QWizard::BackButton)->setDisabled(false);
    infoWindow->hide();
}

void VerifyProj::close()
{
    wizard->button(QWizard::NextButton)->setDisabled(false);
    wizard->button(QWizard::BackButton)->setDisabled(false);
}
