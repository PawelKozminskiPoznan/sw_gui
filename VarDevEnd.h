#ifndef VARDEVEND_H
#define VARDEVEND_H

#include "GenerateProjectFile.h"
#include <MyWizard.h>

#include <QLabel>
#include <QLineEdit>
#include <QWizardPage>

class VarDevEnd : public QWizardPage
{
    Q_OBJECT

public:
    VarDevEnd(QWidget *parent = nullptr,GenerateProjectFile *gen = nullptr,QWizard *w = nullptr, vector<string>* usedProjVar = nullptr);
private:
    QLayout *layout;
    QWizard* wizard;
    string devAmount;
    int deviceAmount;
    int numberActualDev;
    string projectPath;
    int nextPage;
    GenerateProjectFile *generate;
    string previousDev;
    vector<string>* usedProjVarPage;
    QString fieldProjName;
    bool lastDev;

    int nextId() const;
    void initializePage();
    void generateDataToProj();
    bool validatePage();
    void cleanupPage();
};

#endif // VARDEVEND_H
