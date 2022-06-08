#ifndef ADDDEVPROJVAR_H
#define ADDDEVPROJVAR_H

#include "GenerateProjectFile.h"

#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWizardPage>


class AddDevProjVar: public QWizardPage
{
    Q_OBJECT
public:
    AddDevProjVar(QWidget *parent = nullptr,GenerateProjectFile *gen = nullptr, QWizard *w = nullptr, vector<string>* usedProjVar = nullptr);
private:
    QLabel *classNameLabel;
    QLabel *baseClassLabel;
    QLineEdit *classNameLineEdit;
    QLineEdit *baseClassLineEdit;
    QVBoxLayout *layout;
    QWizard *wizard;
    vector<string>* usedProjVarPage;
    vector<pair<string,string>> projVarAndValues;
    string actualDev;
    int nextPage;
    int nextId() const;
    void initializePage();
    bool validatePage();
    GenerateProjectFile *generate;
    QString fieldProjName;
    unsigned int numberActualDev;
    unsigned int numberPreviousDev;
    vector <string> projFields;

    QLabel *errorFile;
    map<string,multimap<string,pair<string,string>>> projectFile;
    QString projFile;
    bool showVarPortTypeInputPage();
    bool showVarDeviceInputPage();
    string proj;
    bool isComplete() const;
};

#endif // ADDDEVPROJVAR_H
