#ifndef EXISTPROJDONEPAGE_H
#define EXISTPROJDONEPAGE_H

#include "GenerateProjectFile.h"

#include <QWizardPage>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLineEdit>

#include <MyWizard.h>

class ExistProjDonePage : public QWizardPage
{
    Q_OBJECT
public:
    ExistProjDonePage(QWidget *parent = nullptr, QWizard *w = nullptr, GenerateProjectFile *gen = nullptr, string = nullptr, vector<string>* usedProjVar = nullptr);
private slots:
    void menuButtonPress();
private:
    QLabel *communicateLabel;
    QLabel *pathToConfigFilesLabel;
    QWizard *wizard;
    QVBoxLayout *layout;
    GenerateProjectFile *generate;
    QString projFile;
    string pathProgram;
    vector<string>* usedProjVarPage;

    int nextPage;
    int nextId() const;
    map<string,multimap<string,pair<string,string>>> configsGenerate;

    void initializePage();
    void cleanupPage();
    string getErrorLog();
    bool validatePage();
};

#endif // EXISTPROJDONEPAGE_H


