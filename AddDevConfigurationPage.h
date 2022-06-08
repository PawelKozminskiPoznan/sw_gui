#ifndef ADDDEVCONFIGURATIONPAGE_H
#define ADDDEVCONFIGURATIONPAGE_H

#include <QWizardPage>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QComboBox>

#include <MyWizard.h>

#include <QPushButton>
#include "Project.h"
#include "GenerateProjectFile.h"

class AddDevConfigurationPage : public QWizardPage
{
    Q_OBJECT
public:
    AddDevConfigurationPage(QWidget *parent = nullptr,GenerateProjectFile *gen = nullptr, QWizard* w = nullptr);
private:
    QLabel *deviceNameLabel;
    QLabel *templatePathLabel;
    QLineEdit *deviceNameLineEdit;
    QLineEdit *templatePathLineEdit;
    QVBoxLayout *layout;
    QLabel *errorFile;

    QWizard* wizard;

    QPushButton *browse;
    QString tempFile;

    GenerateProjectFile *generate;
    unsigned int numberActualDev;
    unsigned int numberPreviousDev;
    string numberAllDev;

    string devName;
    QString templatePath;
    QString fieldProjName;

    int nextPage;
    int nextId() const;
    void initializePage();
    void cleanupPage();
    bool validatePage();

    map<string,multimap<string,pair<string,string>>> projectFile;
    QString projFile;
    string proj;
    bool checkRepeatedDevName(string name);
    QComboBox *comboTempAdd;
    void showTemplates();
};

#endif // ADDDEVCONFIGURATIONPAGE_H
