#ifndef ADDDEVRANGESPAGE_H
#define ADDDEVRANGESPAGE_H

#include <QWizardPage>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include "MyWizard.h"
#include "GenerateProjectFile.h"
#include "RangeConverter.h"

class AddDevRangesPage : public QWizardPage
{
    Q_OBJECT
public:
    AddDevRangesPage(QWidget *parent = nullptr,GenerateProjectFile *gen = nullptr, vector<string>* usedProjVar = nullptr, QWizard* w = nullptr);
private:
    QLabel *classNameLabel;
    QLabel *baseClassLabel;
    QLineEdit *classNameLineEdit;
    QLineEdit *baseClassLineEdit;
    int nextPage;
    bool projVarInit;
    int nextId() const;
    void initializePage();
    GenerateProjectFile *generate;
    QLabel *errorFile;
    string errorMessage;
    QGridLayout *layout;
    RangeConverter *rConvert;
    vector<pair<string,string>> rangeValues;
    unsigned int numberActualDev;
    unsigned int numberPreviousDev;
    QString fieldProjName;
    string tempPath;
    vector<string>* usedProjVarPage;
    QLineEdit *limitPortsLineEdit;
    int maxSaved;
    QWizard* wizard;
    QWidget *infoWidow;

    bool checkPortRanges(vector<string>::iterator itSection,string userData);
    bool validatePage();
    vector<string> sections;
    map<string,vector<int>> rangePort; //mapa na typ portu -> vector int porty
    map<string,vector<string>> rangePortSlots;
    vector<int> allUsedPorts;
    vector<string> allUsedPortSlots;
    map<string,string> ranges;
    bool showVarInputPage();

    map<string,multimap<string,pair<string,string>>> projectFile;
    QString projFile;
    string proj;
    string project;
    string getErrorLog();
    void info(const string &error);
private slots:
    void okPress();
};

#endif // ADDDEVRANGESPAGE_H
