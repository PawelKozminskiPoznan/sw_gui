#include "ExistProjGenOrEdit.h"
#include "MyWizard.h"

#include <QVariant>
#include <QPushButton>

ExistProjGenOrEdit::ExistProjGenOrEdit(QWidget *parent, GenerateProjectFile *gen, QWizard *w) : QWizardPage(parent)
{
    wizard = w;
    generate = gen;
    setTitle((QString::fromUtf8("Załadowano projekt")));
    //setSubTitle(tr("Edycja pliku projektu."));

    nextPage = MyWizard::Page_ExistProjDone;

    layout = nullptr;
}

int ExistProjGenOrEdit::nextId() const
{
    return nextPage;
}

bool ExistProjGenOrEdit::validatePage()
{
    return true;
}

bool ExistProjGenOrEdit::isComplete() const
{
    return false;
}

void ExistProjGenOrEdit::initializePage()
{
    QLabel *generateLabel = new QLabel();
    generateLabel->setText(QString("Generuj pliki konfiguracyjne: "));

    QLabel *editLabel = new QLabel();
    editLabel->setText(QString(QString::fromUtf8("Edytuj plik projektu lub konkretne urządzenie: ")));

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

    QPushButton *genButton = new QPushButton("&Generuj");
    QPushButton *editButton = new QPushButton("&Edytuj");

    connect(genButton, SIGNAL(released()), this, SLOT(genButtonPress()));
    connect(editButton, SIGNAL(released()), this, SLOT(editButtonPress()));

    layout->addWidget(generateLabel);
    layout->addWidget(genButton);
    layout->addWidget(editLabel);
    layout->addWidget(editButton);
    setLayout(layout);
}

void ExistProjGenOrEdit::cleanupPage()
{
    generate->clearConfigsProj();
    QString buttonText = QString::fromUtf8("Dalej");
    wizard->setButtonText(wizard->NextButton, buttonText);
    generate->editedProj = false;
}

void ExistProjGenOrEdit::genButtonPress()
{
    generate->editedProj = false;
    nextPage = MyWizard::Page_ExistProjDone;
    wizard->next();
}

void ExistProjGenOrEdit::editButtonPress()
{
    nextPage = MyWizard::Page_ExistProjEdit;
    wizard->next();
}
