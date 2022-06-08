#ifndef EDITDEVEND_H
#define EDITDEVEND_H

#include "GenerateProjectFile.h"
#include <MyWizard.h>

#include <QLabel>
#include <QLineEdit>
#include <QWizardPage>

class EditDevEnd : public QWizardPage
{
    Q_OBJECT
public:
    EditDevEnd(QWidget *parent = nullptr,GenerateProjectFile *gen = nullptr,QWizard *w = nullptr, vector<string>* usedProjVar = nullptr);
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
    string nameProj;

    int nextId() const;
    void initializePage();
    void generateDataToProj();
    bool validatePage();
    void cleanupPage();
};

#endif // EDITDEVEND_H



