#include "NewProjPage.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <iostream>

NewProjPage::NewProjPage(QWidget *parent, GenerateProjectFile *gen, QWizard *w) : QWizardPage(parent)
{
    wizard = w;
    generate = gen;
    setTitle(tr("Nowy projekt"));
    setSubTitle(tr("Tworzenie nowego pliku projektu."));

    layout = nullptr;
    amountDevInt = 1;
}


int NewProjPage::nextId() const
{
    return nextPage;
}

bool NewProjPage::validatePage()
{
    try
    {
        int inputAmountDev = stoi(amountDevLineEdit->text().toStdString());
        if(inputAmountDev < 1)
        {
            minAmountDev();
            return false;
        }
        projName = projNameLineEdit->text().toStdString();
        amountDev = amountDevLineEdit->text().toStdString();
        errorLabel->setText("");

        if(generate->newDev == true)
            numberActualDev = generate->devicesNames.size();
        else
            numberActualDev = generate->devicesNames.size()-1;
        if(amountDevInt > inputAmountDev && int(numberActualDev) == inputAmountDev)
        {
            nextPage = MyWizard::Page_VarDone;
        }
        else if(amountDevInt > inputAmountDev && int(numberActualDev) > inputAmountDev)
        {
            infoDevAmount();
            return false;
        }
        amountDevInt = inputAmountDev;
        return true;
    }
    catch (...)
    {
        errorLabel->setText(QString(QString::fromUtf8("Wprowadź liczbę w polu 'Liczba urządzeń:' ")));
        return false;
    }
}

void NewProjPage::initializePage()
{
    QString buttonText = QString::fromUtf8("Dalej");
    wizard->setButtonText(wizard->NextButton, buttonText);

    projNameLabel = new QLabel();
    projNameLabel->setText(QString("Nazwa pliku projektu: "));
    projNameLineEdit = new QLineEdit;

    amountDevLabel = new QLabel();
    amountDevLabel->setText(QString(QString::fromUtf8("Liczba urządzeń: ")));
    amountDevLineEdit = new QLineEdit;

    errorLabel = new QLabel();

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

    layout->addWidget(projNameLabel);
    layout->addWidget(projNameLineEdit);
    //projNameLineEdit->setFocusPolicy(Qt::StrongFocus);
    //projNameLineEdit->setFocus();
    layout->addWidget(amountDevLabel);
    layout->addWidget(amountDevLineEdit);
    layout->addWidget(errorLabel);
    setLayout(layout);

    nextPage = MyWizard::Page_DevConfig;

    if(!generate->devicesNames.empty())
    {
        projNameLineEdit->setText(QString::fromStdString(projName));
        amountDevLineEdit->setText(QString::fromStdString(amountDev));
    }

    if(projNameLineEdit->text().toStdString().empty())
    {
        registerField("projectName*", projNameLineEdit);
        registerField("devicesAmount*", amountDevLineEdit);
        generate->clearConfigsProj();
    }
    else
    {
        registerField("projectName", projNameLineEdit);
        registerField("devicesAmount", amountDevLineEdit);
    }
}


void NewProjPage::infoDevAmount()
{
    infoWidow = new QWidget;
    infoWidow->resize(440,200);

    QVBoxLayout* lRepeated = new QVBoxLayout;
    QLabel* info = new QLabel();
    string text;
    QPushButton *ok = new QPushButton("Ok");
    text = "Nie można ustawić mniejszej liczb urządzeń niż liczba stworzonych konfiguracji urządzeń: " + to_string(numberActualDev) +
            "\n Jeśli chcesz usunąć konfiguracje urządzenia to Stwórz aktualny projekt, Załaduj projekt->Edytuj projekt->Usuń konfigurację urządzenia";
    const char * c = text.c_str();
    QString t = QString(QString::fromUtf8(c));
    connect(ok, SIGNAL(released()), this, SLOT(okPress()));
    info->setText(t);
    info->setWordWrap(true);
    lRepeated->addWidget(info);
    ok->setDefault(true);
    lRepeated->addWidget(ok);
    infoWidow->setLayout(lRepeated);
    infoWidow->show();
    wizard->button(QWizard::NextButton)->setDisabled(true);
}


void NewProjPage::okPress()
{
    wizard->button(QWizard::NextButton)->setDisabled(false);
    infoWidow->hide();
}

void NewProjPage::minAmountDev()
{
    infoWidow = new QWidget;
    infoWidow->resize(440,200);

    QVBoxLayout* lRepeated = new QVBoxLayout;
    QLabel* info = new QLabel();
    string text;
    QPushButton *ok = new QPushButton("Ok");
    text = "Minimalna liczba konfigurowanych urządzęń to 1."
            "\n Wprowadź poprawną liczbę!";
    const char * c = text.c_str();
    QString t = QString(QString::fromUtf8(c));
    connect(ok, SIGNAL(released()), this, SLOT(okPress()));
    info->setText(t);
    info->setWordWrap(true);
    lRepeated->addWidget(info);
    ok->setDefault(true);
    lRepeated->addWidget(ok);
    infoWidow->setLayout(lRepeated);
    infoWidow->show();
    wizard->button(QWizard::NextButton)->setDisabled(true);
}
