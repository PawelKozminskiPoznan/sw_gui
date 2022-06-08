#ifndef EDITPORTVARPAGE_H
#define EDITPORTVARPAGE_H

#include "GenerateProjectFile.h"

#include <QLabel>
#include <QLineEdit>
#include <QWizardPage>
#include <MyWizard.h>

class EditPortVarPage : public QWizardPage
{
    Q_OBJECT
public:
    EditPortVarPage(QWidget *parent = nullptr,GenerateProjectFile *gen = nullptr, QWizard* w = nullptr);
private:
    QLabel *classNameLabel;
    QLabel *baseClassLabel;
    QLineEdit *classNameLineEdit;
    QLineEdit *baseClassLineEdit;
    QWizard *wizard;
    QLayout *layout;
    int nextPage;
    int nextId() const;
    void initializePage();
    bool validatePage();
    GenerateProjectFile *generate;
    QString fieldProjName;
    vector<pair<string,string>> portVarAndValues;
    unsigned int numberActualDev;
    unsigned int numberPreviousDev;
    string proj;
    map<string,multimap<string,pair<string,string>>> projectFile;
    QString projFile;
    bool isComplete() const;
    vector <string> projFields;
};

#endif // EDITPORTVARPAGE_H
