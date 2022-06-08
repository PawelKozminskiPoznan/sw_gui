#ifndef VERIFYPROJ_H
#define VERIFYPROJ_H

#include "GenerateProjectFile.h"

#include <QLabel>
#include <QLineEdit>
#include <QWizardPage>
#include <MyWizard.h>
#include <QVBoxLayout>

class VerifyProj : public QWizardPage
{
    Q_OBJECT
public:
    VerifyProj(QWidget *parent = nullptr, QWizard *w = nullptr, const string& path = nullptr);
private:
    QWizard *wizard;
    QVBoxLayout *layout;
    QLabel *infoForUserLabel;
    QLineEdit *pathToFirstConfigFileLineEdit;
    QLabel *infoForUserLabel2;
    QLineEdit *pathToSecondConfigFileLineEdit;
    QPushButton* browse;
    QPushButton* browse2;
    int nextPage;
    string pathProgram;
    QString confFile;
    QWidget *infoWindow;
    void initializePage();
    int nextId() const;
    bool validatePage();
    void errorCommunicate();
private slots:
    void browseButtonPress();
    void browseButtonPress2();
    void closeWindow();
    void close();
    //void textLineChange();
};

#endif // VERIFYPROJ_H
