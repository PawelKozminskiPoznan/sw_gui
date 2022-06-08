#ifndef NEWOREXSISTPROJ_H
#define NEWOREXSISTPROJ_H

#include <QWizardPage>
#include <MyWizard.h>


class NewOrExistProjPage : public QWizardPage
{
    Q_OBJECT
public:
    NewOrExistProjPage(QWidget *parent = nullptr);

private:
    int nextId() const;
    int nextPage;
    QPushButton *newProj;
    QPushButton *existProj;
    QPushButton *verifyProj;

private slots:
    void newProjButtonPress();
    void existProjButtonPress();
    void verifyProjButtonPress();
};

#endif // NEWOREXSISTPROJ_H
