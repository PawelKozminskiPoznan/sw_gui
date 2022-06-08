#ifndef ADDDEVEND_H
#define ADDDEVEND_H

#include "GenerateProjectFile.h"
#include <MyWizard.h>

#include <QLabel>
#include <QLineEdit>
#include <QWizardPage>

class AddDevEnd : public QWizardPage
{
    Q_OBJECT
public:
    AddDevEnd(QWidget *parent = nullptr,GenerateProjectFile *gen = nullptr,QWizard *w = nullptr, vector<string>* usedProjVar = nullptr);
//private slots:
    //    void backActionText();
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

    void cleanupPage();
    int nextId() const;
    void initializePage();
    void generateDataToProj();
    bool validatePage();
};

#endif // ADDDEVEND_H



