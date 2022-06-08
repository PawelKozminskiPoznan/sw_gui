#ifndef EXISTPROJEDITPAGE_H
#define EXISTPROJEDITPAGE_H

#include "GenerateProjectFile.h"

#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QWizardPage>
#include <map>

using namespace std;

class ExistProjEditPage : public QWizardPage
{
    Q_OBJECT
public:
    ExistProjEditPage(QWidget *parent = nullptr, GenerateProjectFile *gen = nullptr, QWizard *w = nullptr);
private:
    QWizard *wizard;
    GenerateProjectFile *generate;
    QString projFile;
    QGridLayout *layout;
    map<string,multimap<string,pair<string,string>>> projectFile;
    QComboBox * combo;
    QComboBox *comboDel;
    QWidget *infoWindow;
    vector<string> actualDevList;
    bool addClicked;
    int nextPage;
    int nextId() const;
    void initializePage();
    bool validatePage();
    string proj;
    QLabel* orNewDev;
    bool addNext;
    bool editNext;
    QLineEdit* projNameLineEdit;
    void errorReadError();
    string getErrorLog();
    void cleanupPage();
private slots:
    void addDevButtonPress();
    void deleteDevButtonPress();
    void closeWindow();
    void closeWindowLuckDev();
    void edit();
    void editProjVar();
    void saveNewProjName();
    void okPress();
    void close();
    void okP();
};

#endif // EXISTPROJEDITPAGE_H
