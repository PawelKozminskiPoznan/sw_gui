#ifndef VARCONFIGURATIONEND_H
#define VARCONFIGURATIONEND_H

#include "GenerateProjectFile.h"
#include <MyWizard.h>


#include <QLabel>
#include <QLineEdit>
#include <QWizardPage>

class VarConfigurationEnd : public QWizardPage
{
    Q_OBJECT
public:
    VarConfigurationEnd(QWidget *parent = nullptr,GenerateProjectFile *gen = nullptr,QWizard *w = nullptr, vector<string>* usedProjVar = nullptr, const string& = nullptr);
private slots:
    void editButtonPress();
    void menuButtonPress();
private:
    QLayout *layout;
    QWizard* wizard;
    string devAmount;
    int deviceAmount;
    string projectPath;
    int nextPage;
    GenerateProjectFile *generate;
    string previousDev;
    vector<string>* usedProjVarPage;
    QString fieldProjName;
    string pathProgram;

    int nextId() const;
    void initializePage();
    //void generateDataToProj();
    string getErrorLog();
    bool validatePage();
};

#endif // VARCONFIGURATIONEND_H
