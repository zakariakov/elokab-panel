#ifndef MENUAPPLICATIONS_H
#define MENUAPPLICATIONS_H


#include "findbutton.h"
#include "menuprogrammes.h"
#include "menufolders.h"
#include "menusystem.h"
#include "menurecent.h"
#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QToolButton>
#include <QFileSystemWatcher>
#include <QLayout>
#include  <QWidgetAction>
#include  <QLineEdit>

class MenuApplications : public QToolButton
{
    Q_OBJECT

public:
    explicit MenuApplications(QWidget *parent = 0);


public slots:

    void showMenu();

    void loadSettings();

signals:

private slots:

    void afterMenuActivated();

    void showHideMenu();

    void setupMenu();

    void rechargeMenu();

    void execApplication();

    void findText(QString text);

private:

    QMenu *mnuFile;

    MenuProgrammes *menuProgrammes;

    MenuFolders *menuFolders;

    MenuRecent *mMenuRecent;

    MenuSystem *menuPower;


    QMenu *mnuFind;


    FindButton *mFindButton;
   // QLineEdit *findLineEdite;



};



#endif // MENUAPPLICATIONS_H
