#ifndef EXISTPROJEDITVAR_H
#define EXISTPROJEDITVAR_H

#include "GenerateProjectFile.h"

#include <QVBoxLayout>
#include <QWizardPage>

using namespace std;

class ExistProjEditVar : public QWizardPage
{
    Q_OBJECT
public:
    ExistProjEditVar(QWidget *parent = nullptr, GenerateProjectFile *generateFile = nullptr,QWizard *w = nullptr);
private:
    QVBoxLayout *layout;
    int nextPage;

    map<string,multimap<string,pair<string,string>>> projectFile;
    GenerateProjectFile * generate;
    string nameProj;
    QWizard* wizard;

    int nextId() const;
    bool validatePage();
    void initializePage();
    void cleanupPage();
};

#endif // EXISTPROJEDITVAR_H
