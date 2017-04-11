#ifndef PANELWIDGET_H
#define PANELWIDGET_H
#include "status/statuswidget.h"
#include <QWindow>
#include "dmenubar/menuapplications.h"
#include <QWidget>
#include <QX11Info>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QDebug>
#include <QProcess>
#include <QLayout>
//#include <QAbstractNativeEventFilter>
//#include "xcb/xcb.h"
namespace Ui {
class PanelWidget;
}


#include "etaskbar/dtaskbarwidget.h"
#include "dsystray/systray.h"
#include "epager/pager.h"
class PanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PanelWidget(QWidget *parent = 0);
    ~PanelWidget();
public slots:
    void reconfigure();
    void exit(){qApp->quit();}
    void showMenu();
void setDock ();
//unsigned int getWindowPID(Window winID) ;


private:
    Ui::PanelWidget *ui;

    //plugins
    StatusWidget *mStatusWidget;
    DtaskbarWidget *mDtaskbarWidget;
    MenuApplications *mMenuApplications;
    Pager *mPager;
    SysTray *mSysTray;

    int m_Position;
QWindow *tlwWindow ;
private slots:

    void resizePanel();
    //X11
    void moveToAllDesktop();
    void setStrut(int top,  int bottom, int topStartX,int topEndX,int bottomStartX, int bottomEndX );
    void loadSettings();
    void loadIconThems();

};

#endif // PANELWIDGET_H
