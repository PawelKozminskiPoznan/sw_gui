#include "NewOrExistProjPage.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

NewOrExistProjPage::NewOrExistProjPage(QWidget *parent) : QWizardPage(parent)
{  
    //setTitle("Plik projektu");

    QLabel *label = new QLabel(QString::fromUtf8("Na podstawie pliku projektu oraz plików szablonów generowane są konfiguracje poszczególnych przełączników. \n\n"
                               "Plik projektu zawiera takie informację jak: nazwy przełączników, przypisany plik szablonu, definicje typów portów oraz wartości zmiennych konfiguracyjnych."));
    label->setWordWrap(true);

    newProj = new QPushButton("Stwórz nowy plik projekt");
    connect(newProj, SIGNAL(released()), this, SLOT(newProjButtonPress()));
    existProj = new QPushButton(QString::fromUtf8("Załaduj plik projekt w celu edycji"));
    connect(existProj, SIGNAL(released()), this, SLOT(existProjButtonPress()));
    verifyProj = new QPushButton(QString::fromUtf8("Weryfikuj konfigurację"));
    connect(verifyProj, SIGNAL(released()), this, SLOT(verifyProjButtonPress()));


    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addWidget(newProj);
    layout->addWidget(existProj);
    layout->addWidget(verifyProj);
    setLayout(layout);

    nextPage = 0;//Wizard::Page_NewProj;

    registerField("newProj*", newProj);
    registerField("existProj*", existProj);
    registerField("verifyProj*", verifyProj);
    setMaximumSize(500,450);
}


int NewOrExistProjPage::nextId() const
{
        return nextPage;
}


void NewOrExistProjPage::newProjButtonPress()
{
    nextPage = MyWizard::Page_NewProj;
    wizard()->next();
}

void NewOrExistProjPage::existProjButtonPress()
{
    nextPage = MyWizard::Page_ExistProj;
    wizard()->next();
}

void NewOrExistProjPage::verifyProjButtonPress()
{
    nextPage = MyWizard::Page_VerifyProj;
    wizard()->next();
}

