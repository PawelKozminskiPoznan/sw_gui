#ifndef EXISTPROJGENOREDIT_H
#define EXISTPROJGENOREDIT_H

#include "GenerateProjectFile.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QWizardPage>

class ExistProjGenOrEdit : public QWizardPage
{
    Q_OBJECT
public:
    ExistProjGenOrEdit(QWidget *parent = nullptr, GenerateProjectFile *gen = nullptr, QWizard *w = nullptr);
private:
    QWizard *wizard;
    QLabel *label;
    QVBoxLayout *layout;
    GenerateProjectFile *generate;
    int nextPage;
    int nextId() const;
    bool validatePage();
    void initializePage();
    bool isComplete() const;
    void cleanupPage();
private slots:
    void genButtonPress();
    void editButtonPress();
};

#endif // EXISTPROJGENOREDIT_H
