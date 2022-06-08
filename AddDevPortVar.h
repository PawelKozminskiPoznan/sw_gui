#ifndef ADDDEVPORTVAR_H
#define ADDDEVPORTVAR_H

#include "GenerateProjectFile.h"

#include <QLabel>
#include <QLineEdit>
#include <QWizardPage>
#include <MyWizard.h>

class AddDevPortVar : public QWizardPage
{
    Q_OBJECT
public:
    AddDevPortVar(QWidget *parent = nullptr,GenerateProjectFile *gen = nullptr, QWizard* w = nullptr);
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
    bool isComplete() const;
    vector <string> projFields;
};

#endif // ADDDEVPORTVAR_H
