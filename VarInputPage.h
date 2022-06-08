#ifndef VARINPUTPAGE_H
#define VARINPUTPAGE_H

#include "GenerateProjectFile.h"

#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWizardPage>



class varInputPage : public QWizardPage
{
     Q_OBJECT
public:
    varInputPage(QWidget *parent = nullptr,GenerateProjectFile *gen = nullptr, QWizard *w = nullptr, vector<string>* usedProjVar = nullptr);
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
    bool showVarDeviceInputPage();
    bool showVarPortTypeInputPage();
    bool isComplete() const;
    vector <string> projFields;
};

#endif // VARINPUTPAGE_H
