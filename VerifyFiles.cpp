#include "VerifyFiles.h"
#include <regex>

#include <QPushButton>
#include <iostream>
VerifyFiles::VerifyFiles(QWidget *parent, QWizard *w) : QWizardPage(parent)
{
    layout = nullptr;
    setTitle(tr("Weryfikacja konfiguracji przełącznika"));
    setSubTitle(tr("Wyniki weryfikacji"));
    wizard = w;
}

int VerifyFiles::nextId() const
{
    return nextPage;
}

bool VerifyFiles::validatePage()
{

    return true;
}

void VerifyFiles::initializePage()
{
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

    label = new QLabel();
    textBrowser = new QTextBrowser();

    primaryHirschmann = false;
    primaryRuggedcom = false;
    secondaryHirschmann = false;
    secondaryRuggedcom = false;

    pathFirstConfig = field("pathFirstConfig").toString().toStdString();
    pathSecondConfig = field("pathSecondConfig").toString().toStdString();

    primaryFile.open(pathFirstConfig, ios::in);
    secondaryFile.open(pathSecondConfig, ios::in);

    checkDeviceOfFile(primaryFile, secondaryFile);
    bool resultsOfComparation = false;
    if(primaryHirschmann && secondaryHirschmann)
    {
        parseHirschmannPrimaryConfigFile(primaryFile);
        parseHirschmannSecondaryConfigFile(secondaryFile);
        resultsOfComparation = comparationHirschmann();
        nextPage = -1;
    }
    else if(primaryRuggedcom && secondaryRuggedcom)
    {
        //parseRuggedcomConfigFile
        nextPage = -1;
    }
    else
    {
        string text;
        if(primaryHirschmann)
        {
            if(secondaryRuggedcom)
                text = "Próbujesz porównać plik konfiguracji urządzenia Hirschmann z plikiem urządzenia RuggedCom! \nOperacja niemożliwa.";
            else
                text = "Plik konfiguracji podstawowej: urządzenie Hirschmann.\nPlik konfiguracji weryfikowanej: błąd \nZaładuj odpowiedni plik.";
        }
        else if(primaryRuggedcom)
        {
            if(secondaryHirschmann)
                text = "Próbujesz porównać plik konfiguracji urządzenia Ruggedcom z plikiem urządzenia Hirschmann! \nOperacja niemożliwa.";
            else
                text = "Plik konfiguracji podstawowej: urządzenie Ruggedcom.\nPlik konfiguracji weryfikowanej: błąd \n Załaduj odpowiedni plik.";
        }
        else if(secondaryHirschmann)
            text = "Plik konfiguracji podstawowej: błąd.\nPlik konfiguracji weryfikowanej: urządzenie Hirschmann \nZaładuj odpowiedni plik.";
        else if(secondaryRuggedcom)
            text = "Plik konfiguracji podstawowej: błąd.\nPlik konfiguracji weryfikowanej: urządzenie RuggedCom \nZaładuj odpowiedni plik.";
        else
            text = "Oba załadowane pliki nie odpowiadają konfiguracji urządzenia Hirschmann oraz RuggedCom";
        errorCommunicate(text);
        nextPage = MyWizard::Page_VerifyProj;
    }
    primaryFile.close();
    secondaryFile.close();

    if(resultsOfComparation)
    {
        label->setText(QString(QString::fromUtf8("Weryfikacja przebiegła prawidłowo.")));
        textBrowser->setText(QString(QString::fromUtf8("Konfiguracja z pierwszego pliku zawarta jest w drugim pliku")));
    }
    else
        label->setText(QString(QString::fromUtf8("Błąd weryfikacji!\nBrak następujących poleceń:")));

    label->setWordWrap(true);


    layout->addWidget(label);
    layout->addWidget(textBrowser);
    setLayout(layout);


    //registerField("pathFirstConfig*", pathToFirstConfigFileLineEdit);
    //registerField("pathSecondConfig*", pathToSecondConfigFileLineEdit);
}

void VerifyFiles::parseHirschmannPrimaryConfigFile(ifstream &configFile)
{
    primaryFile.close();
    primaryFile.open(pathFirstConfig, ios::in);
    string line;
    bool insideInterface = false;
    pair<string,vector<string>> interface;
    vector<string> interfaceContent;
    string interfaceName;
    while (getline(configFile,line))
    {
        unsigned int len=line.length();
        if (len > 0 && len<256)
        {
            if (!regex_match(line,regex("!.*")))
            {
                if (regex_match(line,regex("interface.*")))
                {
                    insideInterface = true;
                    interfaceName = line;
                }
                else if(insideInterface)
                {
                    if (regex_match(line,regex("exit")))
                    {
                        interfaceContent.push_back(line);
                        insideInterface = false;
                        interface.first = interfaceName;
                        interface.second = interfaceContent;
                        primaryPortsSettings.insert(interface);
                        interfaceContent.clear();
                    }
                    else
                    {
                        interfaceContent.push_back(line);
                    }
                }
                else
                {
                    primarySettings.push_back(line);
                }
            }
        }
    }
    for(map<string,vector<string>>::iterator it=primaryPortsSettings.begin(); it!=primaryPortsSettings.end(); ++it)
    {
        cout << it->first << " => " << '\n';
        for(int i=0; i < it->second.size(); i++)
            cout<<"     "<<it->second[i]<<endl;
    }
}

void VerifyFiles::parseHirschmannSecondaryConfigFile(ifstream &configFile)
{
    secondaryFile.close();
    secondaryFile.open(pathSecondConfig, ios::in);
    string line;
    bool insideInterface = false;
    pair<string,vector<string>> interface;
    vector<string> interfaceContent;
    string interfaceName;
    while (getline(configFile,line))
    {
        unsigned int len=line.length();
        if (len > 0 && len<256)
        {
            if (!regex_match(line,regex("!.*")))
            {
                if (regex_match(line,regex("interface.*")))
                {
                    insideInterface = true;
                    interfaceName = line;
                }
                else if(insideInterface)
                {
                    if (regex_match(line,regex("exit")))
                    {
                        interfaceContent.push_back(line);
                        insideInterface = false;
                        interface.first = interfaceName;
                        interface.second = interfaceContent;
                        secondaryPortsSettings.insert(interface);
                        interfaceContent.clear();
                    }
                    else
                    {
                        interfaceContent.push_back(line);
                    }
                }
                else
                {
                    secondarySettings.push_back(line);
                }

            }
        }
    }
    for(map<string,vector<string>>::iterator it=secondaryPortsSettings.begin(); it!=secondaryPortsSettings.end(); ++it)
    {
        cout << it->first << " => " << '\n';
        for(int i=0; i < it->second.size(); i++)
            cout<<"     "<<it->second[i]<<endl;
    }
}

bool VerifyFiles::comparationHirschmann()
{
    bool comparation = true;
    string noFounded;
    for(vector<string>::iterator primary = primarySettings.begin() ; primary != primarySettings.end(); ++primary)
    {
        vector<string>::iterator secondary = std::find(secondarySettings.begin(), secondarySettings.end(), *primary);
        if (secondary == secondarySettings.end())
        {
            comparation = false;
            noFounded += *primary+"\n";
        }
    }
//    map<string,vector<string>>::iterator it;
//    for (it = primaryPortsSettings.begin(); it != primaryPortsSettings.end(); it++)
//    {
//        map<string,vector<string>>::iterator it2;
//        it2 = secondaryPortsSettings.find(it->first);
//        if (it2 == secondaryPortsSettings.end())
//        {
//            comparation = false;
//            noFounded += it->first+"\n";
//            for(vector<string>::iterator interfaceContent = it->second.begin() ; interfaceContent != it->second.end(); ++interfaceContent)
//            {
//                noFounded += *interfaceContent+"\n";
//            }
//        }
//        else
//        {
//            for(vector<string>::iterator interfaceContent = it->second.begin() ; interfaceContent != it->second.end(); ++interfaceContent)
//            {
//                vector<string>::iterator secondaryContent = std::find(it2->second.begin(), it2->second.end(), *interfaceContent);
//                if(secondaryContent == it2->second.end())
//                {
//                    noFounded += *interfaceContent+"\n";
//                    comparation = false;
//                }
//            }
//        }
//    }
    if(!comparation)
    {
        QString noFoundedQ = QString::fromStdString(noFounded);
        textBrowser->setText(noFoundedQ);
    }
    return comparation;
}

void VerifyFiles::checkDeviceOfFile(ifstream &configPrimaryFile, ifstream &configSecondaryFile)
{
    string line;
    while (getline(configPrimaryFile,line))
    {
        unsigned int len=line.length();
        if (len > 0 && len<256)
        {
            if (regex_match(line,regex("!.*")))//spr czy hirschmann
            {
                primaryHirschmann = true;
                break;
            }
            else if (regex_match(line,regex("#.*")))//spr czy hirschmann
            {
                primaryRuggedcom = true;
                break;
            }
        }
    }
    while (getline(configSecondaryFile,line))
    {
        unsigned int len=line.length();
        if (len > 0 && len<256)
        {
            if (regex_match(line,regex("!.*")))//spr czy hirschmann
            {
                secondaryHirschmann = true;
                break;
            }
            else if (regex_match(line,regex("#.*")))//spr czy hirschmann
            {
                secondaryRuggedcom = true;
                break;
            }
        }
    }
}


void VerifyFiles::errorCommunicate(string text)
{
    infoWindow = new QWidget;
    infoWindow->resize(240,200);

    QVBoxLayout* lDelete = new QVBoxLayout;
    QLabel* info = new QLabel();
    QPushButton *ok = new QPushButton("Ok");
    const char * c = text.c_str();
    QString t = QString(QString::fromUtf8(c));
    info->setText(t);
    connect(ok, SIGNAL(released()), this, SLOT(closeWindow()));
    infoWindow->setAttribute(Qt::WA_DeleteOnClose,true);
    connect(infoWindow , SIGNAL(destroyed()), this,SLOT(close()));
    lDelete->addWidget(info);
    ok->setDefault(true);
    lDelete->addWidget(ok);
    infoWindow->setLayout(lDelete);
    wizard->button(QWizard::NextButton)->setDisabled(true);
    wizard->button(QWizard::BackButton)->setDisabled(true);
    infoWindow->show();
}

void VerifyFiles::closeWindow()
{
    wizard->button(QWizard::NextButton)->setDisabled(false);
    wizard->button(QWizard::BackButton)->setDisabled(false);
    infoWindow->hide();
    wizard->setStartId(MyWizard::Page_VerifyProj);
    wizard->restart();
}

void VerifyFiles::close()
{
    wizard->button(QWizard::NextButton)->setDisabled(false);
    wizard->button(QWizard::BackButton)->setDisabled(false);
    wizard->setStartId(MyWizard::Page_VerifyProj);
    wizard->restart();
}
