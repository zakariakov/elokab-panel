#ifndef MENUAPPLICATIONS_H
#define MENUAPPLICATIONS_H


#include "menuclassic.h"

#include "menuform.h"

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
void showForm();
    void afterMenuActivated();




private:



MenuForm *mMenuForm;
MenuClassic *mMenuClassic;
int menuStyle;

   // QLineEdit *findLineEdite;



};



#endif // MENUAPPLICATIONS_H
