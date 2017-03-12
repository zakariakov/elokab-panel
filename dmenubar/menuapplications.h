#ifndef MENUAPPLICATIONS_H
#define MENUAPPLICATIONS_H

//#include <MenuSystem>
#include "menuprogrammes.h"
#include "menufolders.h"
#include "menusystem.h"
#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QToolButton>
#include <QFileSystemWatcher>
#include <QLayout>
class MenuApplications : public QToolButton
{
    Q_OBJECT

public:
    explicit MenuApplications(QWidget *parent = 0);
    QMenu *mnuFile;



public slots:
    /*virtual*/

  //  /*virtual*/ void setLayout(QHBoxLayout *p);
    /*virtual*/ void setSize(QSize size);

  //  /*virtual*/ void setKeySequence(QKeySequence sequence);
  //  /*virtual*/ QString   keySequence();
  //  /*virtual*/  void setActions(QList<QAction *>list);

    /*virtual*/  void themeChanged()
    {
        refreshIcons();
    }
    void showMenu();
 void loadSettings();

    //    void setListAction( QList<QAction *>actList);
signals:

private slots:
    void afterMenuActivated();
    void showHideMenu();

    void setupMenu();



    void rechargeMenu();

    void refreshIcons();

    void execApplication();




private:
    MenuProgrammes *menuProgrammes;
    MenuFolders *menuFolders;
    MenuSystem *menuPower;

    QList<QAction *>m_actList;


    QToolButton *toolStartHere;
    bool m_multi;
    int m_sizeHint;
    QPoint menuPos;

    QString m_ks;
};

#endif // MENUAPPLICATIONS_H
