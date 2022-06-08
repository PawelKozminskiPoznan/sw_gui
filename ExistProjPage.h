#ifndef EXSISTPROJPAGE_H
#define EXSISTPROJPAGE_H

#include <QWizardPage>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <MyWizard.h>

#include <QPushButton>
#include "Project.h"
#include "GenerateProjectFile.h"

class ExistProjPage : public QWizardPage
{
       Q_OBJECT
public:
    ExistProjPage(QWidget *parent = nullptr, QWizard *w = nullptr, const string& path = nullptr, GenerateProjectFile *gen = nullptr);
private:
    GenerateProjectFile *generate;
    QLabel *infoForUserLabel;
    QLineEdit *pathToProjFileLineEdit;
    QLabel *errorFile;
    QWizard *wizard;
    QVBoxLayout *layout;

    QPushButton *browse;
    QString projFile;
    string project;
    int nextPage;
    int nextId() const;
    bool validatePage();
    // bool isComplete() const;
    string getErrorLog();
    QWidget *infoWindow;
    string pathProgram;
    string projName;
    void errorCommunicate();
    void initializePage();
    string getOnlyProjName(string proj);
private slots:
    void browseButtonPress();
    //void textLineChange();
    void closeWindow();
    void close();
};

#endif // EXSISTPROJPAGE_H
