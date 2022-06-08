#ifndef DEVICECONFIGURATION_H
#define DEVICECONFIGURATION_H

#include <QWizardPage>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QComboBox>

#include <MyWizard.h>

#include <QPushButton>
#include "Project.h"
#include "GenerateProjectFile.h"

class DeviceConfigurationPage : public QWizardPage
{
     Q_OBJECT
public:
    DeviceConfigurationPage(QWidget *parent = nullptr,GenerateProjectFile *gen = nullptr, QWizard* w = nullptr);
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

    QComboBox *comboTemp;
    QWidget *infoWindow;

    int nextPage;
    int nextId() const;
    void initializePage();
    bool validatePage();
    void repeatedDevConf();
    void showTemplates();
private slots:
    void okPress();
    void okP();
};

#endif // DEVICECONFIGURATION_H
