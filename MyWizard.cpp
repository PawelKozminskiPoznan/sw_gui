#include "MyWizard.h"
#include "NewOrExistProjPage.h"
#include "DeviceConfigurationPage.h"
#include "VarDeviceInputPage.h"
#include "VarPortTypeInputPage.h"
#include "VarConfigurationEnd.h"
#include "NewProjPage.h"
#include "ExistProjPage.h"
#include "ExistProjDonePage.h"
#include "DeviceRangesPage.h"
#include "VarInputPage.h"
#include "DeviceConfigurationPage.h"
#include "VarConfigurationEnd.h"
#include "VarDevEnd.h"
#include "ExistProjEditPage.h"
#include "ExistProjGenOrEdit.h"
#include "ExistProjEditVar.h"
#include "EditDeviceConfigurationPage.h"
#include "EditDeviceRangesPage.h"
#include "EditProjVarPage.h"
#include "EditDeviceVarPage.h"
#include "EditPortVarPage.h"
#include "EditDevEnd.h"
#include "AddDevConfigurationPage.h"
#include "AddDevRangesPage.h"
#include "AddDevProjVar.h"
#include "AddDevDevVar.h"
#include "AddDevPortVar.h"
#include "AddDevEnd.h"
#include "VerifyProj.h"
#include "VerifyFiles.h"


MyWizard::MyWizard(char* path, QWidget *parent) :
    QWizard(parent)
{
    generateProj = new GenerateProjectFile();
    usedProjVar = new vector<string>();
    pathProgram = path;

    //setPixmap(QWizard::WatermarkPixmap, QPixmap("images\\logo1.png"));
    setWizardStyle(QWizard::ClassicStyle);
    setPage(Page_Intro, new NewOrExistProjPage());
    setPage(Page_NewProj, new NewProjPage(nullptr, generateProj, this));

    setPage(Page_ExistProj, new ExistProjPage(nullptr, this, pathProgram, generateProj));
    setPage(Page_ExistProjGenOrEdit, new ExistProjGenOrEdit(nullptr, generateProj, this));
    setPage(Page_ExistProjEdit, new ExistProjEditPage(nullptr, generateProj, this));
    setPage(Page_ExistProjEditVar, new ExistProjEditVar(nullptr, generateProj, this));
    setPage(Page_ExistProjDone, new ExistProjDonePage(nullptr, this, generateProj, pathProgram, usedProjVar));

    setPage(Page_EditDeviceConfiguration, new EditDeviceConfigurationPage(nullptr,generateProj, this));
    setPage(Page_EditDeviceRanges, new EditDeviceRangesPage(nullptr,generateProj, usedProjVar, this));
    setPage(Page_EditProjVar, new EditProjVarPage(nullptr,generateProj, this, usedProjVar));
    setPage(Page_EditDeviceVar, new EditDeviceVarPage(nullptr,generateProj));
    setPage(Page_EditPortVar, new EditPortVarPage(nullptr,generateProj, this));
    setPage(Page_EditDevEnd, new EditDevEnd(nullptr,generateProj, this, usedProjVar));

    setPage(Page_AddDeviceConfiguration, new AddDevConfigurationPage(nullptr,generateProj, this));
    setPage(Page_AddDeviceRanges, new AddDevRangesPage(nullptr,generateProj, usedProjVar, this));
    setPage(Page_AddProjVar, new AddDevProjVar(nullptr,generateProj, this, usedProjVar));
    setPage(Page_AddDeviceVar, new AddDevDevVar(nullptr,generateProj));
    setPage(Page_AddPortVar, new AddDevPortVar(nullptr,generateProj, this));
    setPage(Page_AddDevEnd, new AddDevEnd(nullptr,generateProj, this, usedProjVar));

    setPage(Page_DevConfig, new DeviceConfigurationPage(nullptr,generateProj, this));
    setPage(Page_DevRanges, new DeviceRangesPage(nullptr,generateProj, usedProjVar, this));
    setPage(Page_VarInput, new varInputPage(nullptr,generateProj, this, usedProjVar));
    setPage(Page_VarDevice, new varDeviceInputPage(nullptr,generateProj));
    setPage(Page_VarPort, new varPortTypeInputPage(nullptr,generateProj, this));
    setPage(Page_DevDone, new VarDevEnd(nullptr,generateProj, this, usedProjVar));
    setPage(Page_VarDone, new VarConfigurationEnd(nullptr,generateProj, this, usedProjVar, pathProgram));

    setPage(Page_VerifyProj, new VerifyProj(nullptr, this, pathProgram));
    setPage(Page_VerifyFiles, new VerifyFiles(nullptr, this));


    QList<QWizard::WizardButton> l;
    l << QWizard::BackButton <<  QWizard::NextButton; //<< QWizard::FinishButton;
    setButtonLayout(l);

    setButtonText(NextButton,"Dalej");
    setButtonText(BackButton,"Wstecz");
    setButtonText(FinishButton,"Koniec");
    setWindowTitle(QString::fromUtf8("SwCfg - konfiguracja przełączników"));
    //setMaximumSize(300,300);
    show();
}

MyWizard::~MyWizard()
{

}
