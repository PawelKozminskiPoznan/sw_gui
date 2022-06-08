QT       += core gui widgets

TARGET = sw_gui
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
CONFIG += c++11
#CONFIG -= app_bundle
#CONFIG -= qt

SOURCES += \
    ConfigLinesList.cpp \
    Utils.cpp \
    ParamsList.cpp \
    DeviceParams.cpp \
    DeviceTemplate.cpp \
    InputArgs.cpp \
    RangeConverter.cpp \
    GenerateProjectFile.cpp \
    Project.cpp \
    CheckerFileProject.cpp \
    swcfg.cpp \
    DeviceConfigurationPage.cpp \
    ExistProjDonePage.cpp \
    NewOrExistProjPage.cpp \
    NewProjPage.cpp \
    DeviceRangesPage.cpp \
    ExistProjPage.cpp \
    VarDeviceInputPage.cpp \
    VarInputPage.cpp \
    VarPortTypeInputPage.cpp \
    VarConfigurationEnd.cpp \
    MyWizard.cpp \
    VarDevEnd.cpp \
    ExistProjEditPage.cpp \
    ExistProjGenOrEdit.cpp \
    ExistProjEditVar.cpp \
    EditDeviceConfigurationPage.cpp \
    EditDeviceRangesPage.cpp \
    EditProjVarPage.cpp \
    EditDeviceVarPage.cpp \
    EditPortVarPage.cpp \
    EditDevEnd.cpp \
    AddDevConfigurationPage.cpp \
    AddDevRangesPage.cpp \
    AddDevProjVar.cpp \
    AddDevDevVar.cpp \
    AddDevPortVar.cpp \
    AddDevEnd.cpp \
    VerifyProj.cpp \
    VerifyFiles.cpp

HEADERS += \
    CheckerFileProject.h \
    GenerateProjectFile.h \
    RangeConverter.h \
    Project.h \
    InputArgs.h \
    DeviceParams.h \
    ParamsList.h \
    ConfigLinesList.h \
    DeviceTemplate.h \
    Utils.h \
    DeviceConfigurationPage.h \
    DeviceRangesPage.h \
    ExistProjPage.h \
    ExistProjDonePage.h \
    NewOrExistProjPage.h \
    NewProjPage.h \
    VarConfigurationEnd.h \
    VarDeviceInputPage.h \
    VarInputPage.h \
    VarPortTypeInputPage.h \
    MyWizard.h \
    VarDevEnd.h \
    ExistProjEditPage.h \
    ExistProjGenOrEdit.h \
    ExistProjEditVar.h \
    EditDeviceConfigurationPage.h \
    EditDeviceRangesPage.h \
    EditProjVarPage.h \
    EditDeviceVarPage.h \
    EditPortVarPage.h \
    EditDevEnd.h \
    AddDevConfigurationPage.h \
    AddDevRangesPage.h \
    AddDevProjVar.h \
    AddDevDevVar.h \
    AddDevPortVar.h \
    AddDevEnd.h \
    DeviceRangesPage.h \
    VerifyProj.h \
    VerifyFiles.h

FORMS +=

#Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

