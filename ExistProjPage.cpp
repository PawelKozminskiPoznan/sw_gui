#include "ExistProjPage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include "CheckerFileProject.h"

#include <QTimer>
#include <iostream>

ExistProjPage::ExistProjPage(QWidget *parent, QWizard *w, const string& path, GenerateProjectFile *gen) : QWizardPage(parent)
{
    generate = gen;
    QString text = QString(QString::fromUtf8("Wybór gotowego pliku projektu"));
    string textS = text.toStdString();
    const char * c = textS.c_str();
    setTitle(tr(c));
    wizard = w;
    layout = nullptr;
    pathProgram = path;
}

int ExistProjPage::nextId() const
{
    return nextPage;
}

bool ExistProjPage::validatePage()
{
    projFile = pathToProjFileLineEdit->text();
    project = projFile.toStdString();
    try {
        projName = getOnlyProjName(project);
        generate->outputsFoldersForExsistProj(projName+".prj");
        new Project(project,true);
        return true;
    } catch (...) {
        errorCommunicate();
        return false;
    }
}


void ExistProjPage::initializePage()
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

    infoForUserLabel = new QLabel();
    infoForUserLabel->setText(QString(QString::fromUtf8("Ścieżka do pliku projektu:")));
    pathToProjFileLineEdit = new QLineEdit(this);
    infoForUserLabel->setBuddy(pathToProjFileLineEdit);

    browse = new QPushButton("Szukaj...");
    connect(browse, SIGNAL(released()), this, SLOT(browseButtonPress()));

    pathToProjFileLineEdit->setEchoMode(QLineEdit::Normal);

    layout->addWidget(infoForUserLabel);
    layout->addWidget(pathToProjFileLineEdit);
    layout->addWidget(browse);
    setLayout(layout);

    QTimer::singleShot(0, browse, SLOT(setFocus()));

    nextPage = MyWizard::Page_ExistProjGenOrEdit;

    registerField("path*", pathToProjFileLineEdit);
}

void ExistProjPage::browseButtonPress()
{
    CheckerFileProject checkFolder;
    size_t lastSlash = pathProgram.find_last_of("\\");
    string pathProjects;
    pathProjects.assign(pathProgram.begin(), pathProgram.begin()+int(lastSlash));
    pathProjects += "\\output";
    if(checkFolder.exsistFolder(pathProjects))
        projFile = QFileDialog::getOpenFileName(this, "Wybierz plik projektu",QString::fromStdString(pathProjects));
    else
        projFile = QFileDialog::getOpenFileName(this, "Wybierz plik projektu",QString::fromStdString(pathProgram));
    pathToProjFileLineEdit->setText(projFile);

    wizard->button(QWizard::NextButton)->setFocus();
}

string ExistProjPage::getErrorLog()
{
    string path = projName;
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
    return "Problem z plikiem " + path;
}


void ExistProjPage::errorCommunicate()
{
    infoWindow = new QWidget;
    infoWindow->resize(240,200);

    QVBoxLayout* lDelete = new QVBoxLayout;
    QLabel* info = new QLabel();
    QLabel* info2 = new QLabel();
    string text;
    QPushButton *ok = new QPushButton("Ok");
    QString t = QString::fromUtf8("Komunikat błedu ładowania pliku projektu:");
    info->setText(t);
    text = getErrorLog();
    const char * c = text.c_str();
    t = QString(QString::fromUtf8(c));
    connect(ok, SIGNAL(released()), this, SLOT(closeWindow()));
    infoWindow->setAttribute(Qt::WA_DeleteOnClose,true);
    connect(infoWindow , SIGNAL(destroyed()), this,SLOT(close()));
    info2->setText(t);
    lDelete->addWidget(info);
    lDelete->addWidget(info2);
    ok->setDefault(true);
    lDelete->addWidget(ok);
    infoWindow->setLayout(lDelete);
    wizard->button(QWizard::NextButton)->setDisabled(true);
    infoWindow->show();
}

void ExistProjPage::closeWindow()
{
    wizard->button(QWizard::NextButton)->setDisabled(false);
    infoWindow->hide();
}

void ExistProjPage::close()
{
    wizard->button(QWizard::NextButton)->setDisabled(false);
}

string ExistProjPage::getOnlyProjName(string proj)
{
    size_t posSlash = proj.find_last_of("/\\");
    size_t posDot = proj.find_last_of(".");
    string name;
    if(posDot != string::npos)
    {
        name.append(proj.begin()+int(posSlash)+1,proj.begin()+int(posDot));
        return name;
    }
    else if(posSlash != string::npos)
    {
        name.append(proj.begin()+int(posSlash)+1,proj.end());
        return name;
    }
    else
        return proj;
}
