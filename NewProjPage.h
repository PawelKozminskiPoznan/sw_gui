#ifndef NEWPROJ_H
#define NEWPROJ_H

#include <QWizardPage>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include "MyWizard.h"

class NewProjPage : public QWizardPage
{
    Q_OBJECT
public:
    NewProjPage(QWidget *parent = nullptr, GenerateProjectFile *gen = nullptr, QWizard *w = nullptr);
private slots:
    void okPress();
private:
    QLabel *projNameLabel;
    QLabel *amountDevLabel;
    QLineEdit *projNameLineEdit;
    QLineEdit *amountDevLineEdit;
    QLabel *errorLabel;
    QVBoxLayout *layout;
    QWidget *infoWidow;
    int nextPage;
    QWizard* wizard;

    string projName;
    string amountDev;
    int amountDevInt;

    GenerateProjectFile *generate;
    uint numberActualDev;

    int nextId() const;
    bool validatePage();
    void initializePage();
    void infoDevAmount();
    void minAmountDev();
};

#endif // NEWPROJ_H
