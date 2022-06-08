#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "GenerateProjectFile.h"

#include <QWizardPage>

class MyWizard : public QWizard
{
    Q_OBJECT

public:
     MyWizard(char*, QWidget *parent = nullptr);
    ~MyWizard();
    enum { Page_Intro, Page_NewProj, Page_ExistProj, Page_ExistProjGenOrEdit, Page_ExistProjEditVar, Page_ExistProjEdit, Page_ExistProjDone,
           Page_EditDeviceConfiguration, Page_EditDeviceRanges, Page_EditProjVar, Page_EditDeviceVar, Page_EditPortVar, Page_EditDevEnd,
           Page_DevConfig, Page_DevRanges, Page_VarInput, Page_VarDevice, Page_VarPort, Page_DevDone, Page_VarDone,
           Page_AddDeviceConfiguration, Page_AddDeviceRanges, Page_AddProjVar, Page_AddDeviceVar, Page_AddPortVar, Page_AddDevEnd,
           Page_VerifyProj, Page_VerifyFiles};
    GenerateProjectFile *generateProj;
    vector<string>* usedProjVar;
private:
    string pathProgram;
};

#endif // MAINWINDOW_H
