#include "GenerateProjectFile.h"
#include "ExistProjDonePage.h"

#include <QPushButton>
#include <QTimer>


ExistProjDonePage::ExistProjDonePage(QWidget *parent, QWizard *w, GenerateProjectFile *gen, string path, vector<string>* usedProjVar) : QWizardPage(parent)
{
    generate = gen;
    wizard = w;
    nextPage = -1;
    layout = nullptr;

    pathProgram = path;
    uint pos = pathProgram.find_last_of("/\\");
    pathProgram = pathProgram.substr(0, pos);
    usedProjVarPage = usedProjVar;
}

bool ExistProjDonePage::validatePage()
{
    generate->editedProj = false;
    return true;
}

void ExistProjDonePage::initializePage()
{
    QString text = QString(QString::fromUtf8("Tworzenie plików konfiguracyjnych"));
    string textS = text.toStdString();
    const char * c = textS.c_str();
    setTitle(tr(c));

    communicateLabel = new QLabel();
    pathToConfigFilesLabel = new QLabel();
    QLabel* pathToProjFileLabel = new QLabel();


    if(layout != nullptr)//usuwanie poprzedniego layout'u
    {
        QLayoutItem* child;
            while(layout->count()!=0)
            {
                child = layout->takeAt(0);
                if(child->layout() != nullptr)
                {
                    delete (child->layout());
                }
                else if(child->widget() != nullptr)
                {
                    delete child->widget();
                }
                delete child;
            }
    }
    else
        layout = new QVBoxLayout;
    QPushButton *menu = new QPushButton(QString::fromUtf8("Strona główna"));
    connect(menu, SIGNAL(released()), this, SLOT(menuButtonPress()));
    QLabel *pathProj = new QLabel(QString(QString::fromUtf8("Ścieżka do pliku projektu: ")));
    QLabel *pathDev = new QLabel(QString(QString::fromUtf8("Ścieżka do plików konfiguracyjnych: ")));
    layout->addWidget(communicateLabel);
    layout->addWidget(new QLabel());
    layout->addWidget(pathProj);
    layout->addWidget(pathToConfigFilesLabel);
    pathToConfigFilesLabel->setWordWrap(true);
    pathToConfigFilesLabel->setTextInteractionFlags(Qt::TextInteractionFlag(1));
    layout->addWidget(new QLabel());
    layout->addWidget(pathDev);
    layout->addWidget(pathToProjFileLabel);
    pathToProjFileLabel->setWordWrap(true);
    pathToProjFileLabel->setTextInteractionFlags(Qt::TextInteractionFlag(1));
    layout->addWidget(new QLabel());
    layout->addWidget(menu);
    setLayout(layout);

    usedProjVarPage->clear();

    QList<QWizard::WizardButton> l;
    l << QWizard::BackButton <<  QWizard::NextButton << QWizard::FinishButton;
    wizard->setButtonLayout(l);

    string projName = "";
    projName = field("editedProjName").toString().toStdString();
    if(!generate->editedProj)//projName.empty() &&
    {
        projFile = field("path").toString();
        string pathProj = projFile.toStdString();
        if(!pathProj.empty())
        {
            projName = "";
            replace(pathProj.begin(), pathProj.end(), '/', '\\');
            uint posDot = pathProj.find_last_of(".");
            uint posSlash = pathProj.find_last_of("\\");
            projName.append(pathProj.begin()+int(posSlash)+1, pathProj.begin()+int(posDot));
        }
    }
    QTimer::singleShot(0, wizard->button(QWizard::FinishButton), SLOT(setFocus()));

    string pathToProjFile =  pathProgram + "\\output\\" + projName + "\\" + projName + ".prj";
    string pathToConfigFiles = pathProgram + "\\output\\" + projName + "\\device\\";
    c = pathToConfigFiles.c_str();
    text = QString(QString::fromUtf8(c));
    c = pathToProjFile.c_str();
    QString text2 = QString(QString::fromUtf8(c));
    try {
        Project* proj;
        if(generate->editedProj)
            proj = new Project("output/"+projName+'/'+projName+".prj",true);
        else
        {
            string path = field("path").toString().toStdString();
            proj = new Project(path,true);
        }
        configsGenerate = proj->getParsedProject();
        generate->loadConfigsProj(configsGenerate);
        generate->outputsFolders(projName);
        generate->writeToProjectFile();
        map<string,stringstream> configs = proj->createConfigFiles(generate->portLimitations);
        generate->portLimitations.clear();
        proj->saveOutputFile(configs);
        communicateLabel->setText(QString(QString::fromUtf8("Pliki konfiguracyjne zostały stworzone!")));
        pathToConfigFilesLabel->setText(text2);
        pathToProjFileLabel->setText(text);
        while(!generate->devicesNames.empty()) generate->devicesNames.pop();
    } catch (...) {
        communicateLabel->setText(QString(QString::fromUtf8("Niepowodzenie...     Pliki konfiguracyjne nie zostały stworzone."//n
                                                "Problem z prawidłowym zczytaniem pliku projektu; może zawiera nie pełną lub niepoprawną konfigurację.")));
        communicateLabel->setWordWrap(true);
        QString t = QString::fromUtf8("Komunikat błedu z pliku logs.txt odnośnie ładowania pliku projektu:");
        pathToConfigFilesLabel->setText(t);
        pathToConfigFilesLabel->setWordWrap(true);
        string text = getErrorLog();
        const char * c = text.c_str();
        t = QString(QString::fromUtf8(c));
        pathToProjFileLabel->setText(t);
        pathProj->setText("");
        pathDev->setText("");
    }
}

int ExistProjDonePage::nextId() const
{
    return nextPage;
}

void ExistProjDonePage::menuButtonPress()
{
    QString buttonText = QString::fromUtf8("Dalej");
    wizard->setButtonText(wizard->NextButton, buttonText);
    QList<QWizard::WizardButton> l;
    l << QWizard::BackButton <<  QWizard::NextButton << QWizard::FinishButton;
    wizard->setButtonLayout(l);
    generate->clearConfigsProj();
    generate->editedProj = false;
    wizard->setStartId(MyWizard::Page_Intro);
    wizard->restart();
}

void ExistProjDonePage::cleanupPage()
{
    QString buttonText = QString::fromUtf8("Generuj");
    wizard->setButtonText(wizard->NextButton, buttonText);
    generate->loadConfigsProj(configsGenerate);
}

string ExistProjDonePage::getErrorLog()
{
    string path;
    size_t posSlash = generate->pathProj.find_last_of("/\\");
    path.append(generate->pathProj.begin()+int(posSlash)+1,generate->pathProj.end()-4);//-4 jesli rozszerzenie .prj
    if(path[0] == '\\')
        path = "output" + path + "\\logs";
    else
        path = "output\\" + path + "\\logs";
    string extension = ".txt";
    path.append(extension);
    string errorLog;
    ifstream f(path);
    if (f.is_open())
    {
        string tempError;
        while (getline(f,tempError))
        {
            if(!tempError.empty())
                errorLog = tempError;
        }
        return errorLog;
    }
    return "Problem z plikiem: " + path + " uniemożliwiający wyświetlenie komunikatu błedu";
}
