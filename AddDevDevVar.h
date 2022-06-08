#ifndef ADDDEVDEVVAR_H
#define ADDDEVDEVVAR_H

#include "GenerateProjectFile.h"


#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWizardPage>

class AddDevDevVar : public QWizardPage
{
    Q_OBJECT
public:
    AddDevDevVar(QWidget *parent = nullptr,GenerateProjectFile *gen = nullptr);
private:
    QLabel *classNameLabel;
    QLabel *baseClassLabel;
    QLineEdit *classNameLineEdit;
    QLineEdit *baseClassLineEdit;
    int nextPage;
    QVBoxLayout *layout;
    GenerateProjectFile *generate;
    QString fieldProjName;
    int numberPreviousDev;
    int numberActualDev;
    vector<pair<string,string>> switchVarAndValues;
    int nextId() const;
    void initializePage();
    bool validatePage();
    vector <string> projFields;

    QLabel *errorFile;
    map<string,multimap<string,pair<string,string>>> projectFile;
    QString projFile;
    bool showVarPortTypeInputPage();
    string proj;
    bool isComplete() const;
};

#endif // ADDDEVDEVVAR_H
