#ifndef VARDEVICEINPUTPAGE_H
#define VARDEVICEINPUTPAGE_H

#include "GenerateProjectFile.h"


#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWizardPage>



class varDeviceInputPage : public QWizardPage
{
    Q_OBJECT
public:
    varDeviceInputPage(QWidget *parent = nullptr,GenerateProjectFile *gen = nullptr);
private:
    QLabel *classNameLabel;
    QLabel *baseClassLabel;
    QLineEdit *classNameLineEdit;
    QLineEdit *baseClassLineEdit;
    int nextPage;
    QVBoxLayout *layout;
    GenerateProjectFile *generate;
    QString fieldProjName;
    uint numberPreviousDev;
    uint numberActualDev;
    vector<pair<string,string>> switchVarAndValues;
    int nextId() const;
    void initializePage();
    bool validatePage();
    bool showVarPortTypeInputPage();
    vector <string> projFields;
    bool isComplete() const;
};

#endif // VARDEVICEINPUTPAGE_H
