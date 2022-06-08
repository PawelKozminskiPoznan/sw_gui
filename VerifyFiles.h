#ifndef VERIFYFILES_H
#define VERIFYFILES_H

#include "GenerateProjectFile.h"

#include <QLabel>
#include <QLineEdit>
#include <QWizardPage>
#include <MyWizard.h>
#include <QVBoxLayout>

#include <QTextBrowser>
#include <fstream>

class VerifyFiles : public QWizardPage
{
     Q_OBJECT
public:
    VerifyFiles(QWidget *parent = nullptr, QWizard *w = nullptr);
private:
    QWizard *wizard;
    QVBoxLayout *layout;
    int nextPage;
    ifstream primaryFile;
    ifstream secondaryFile;
    vector<string> primarySettings;
    vector<string> secondarySettings;
    map<string,vector<string>> primaryPortsSettings;
    map<string,vector<string>> secondaryPortsSettings;
    void initializePage();
    int nextId() const;
    bool validatePage();
    void parseHirschmannPrimaryConfigFile(ifstream& configFile);
    void parseHirschmannSecondaryConfigFile(ifstream& configFile);
    void checkDeviceOfFile(ifstream& configPrimaryFile, ifstream& configSecondaryFile);
    bool primaryHirschmann;
    bool primaryRuggedcom;
    bool secondaryHirschmann;
    bool secondaryRuggedcom;
    void errorCommunicate(string text);
    QWidget *infoWindow;
    string pathFirstConfig;
    string pathSecondConfig;
    QTextBrowser *textBrowser;
    bool comparationHirschmann();
    QLabel *label;
private slots:
    void closeWindow();
    void close();
};

#endif // VERIFYFILES_H
