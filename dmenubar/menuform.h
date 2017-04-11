#ifndef MENUFORM_H
#define MENUFORM_H
#include "menuprogrammes.h"
#include "menufolders.h"
#include "menusystem.h"
#include "menurecent.h"
#include <QWidget>
#include <QListWidgetItem>
namespace Ui {
class MenuForm;
}

class MenuForm : public QWidget
{
    Q_OBJECT

public:
    explicit MenuForm(QWidget *parent = 0);
    ~MenuForm();
 void open(QPoint pos, int s);
signals:
 void actionExecuted(QAction*);
private:
    Ui::MenuForm *ui;

    MenuProgrammes *menuProgrammes;

    MenuFolders *menuFolders;

    MenuRecent *mMenuRecent;

    MenuSystem *menuPower;

     QListWidget *listWidgetRecent;
      QListWidget *listWidgetAll;

    QPoint m_pos;
    int pMap;
private slots:
    void chargeMenuAll();
    void rechargeRecent();
    void chargeMenuRecent();
    void chargeMenuProgrammes();
    void chargeMenufolder();
    void chargeMenuSystem();
    void on_listWidgetMenu_itemPressed(QListWidgetItem *item);
    void on_listWidgetMenu_currentItemChanged(QListWidgetItem *current, QListWidgetItem *);
    void execItem(QListWidgetItem* item);
    void execFolder(QListWidgetItem* item);
//#############################################//

    void resizeMe();
    void realign();
void changeStyle();

void on_lineEdit_textChanged(const QString &arg1);
};

#endif // MENUFORM_H
