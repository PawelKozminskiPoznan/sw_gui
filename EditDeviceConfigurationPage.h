#ifndef EDITDEVICECONFIGURATIONPAGE_H
#define EDITDEVICECONFIGURATIONPAGE_H

#include <QWizardPage>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QComboBox>

#include <MyWizard.h>

#include <QPushButton>
#include "Project.h"
#include "GenerateProjectFile.h"

class EditDeviceConfigurationPage : public QWizardPage
{
    Q_OBJECT
public:
    EditDeviceConfigurationPage(QWidget *parent = nullptr,GenerateProjectFile *gen = nullptr, QWizard* w = nullptr);
private slots:
    void okPress();
private:
    void cleanupPage();
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
    bool validatePage();

    map<string,multimap<string,pair<string,string>>> projectFile;
    QString projFile;
    string proj;
    bool checkRepeatedDevName(string name);
    QString editedDeviceName;
    QWidget *infoWidow;

    QComboBox* comboTempEdit;
    void showTemplates();
    void setTempFromProj();
    void luckOfTemp(string tempFile);
};

#endif // EDITDEVICECONFIGURATIONPAGE_H
