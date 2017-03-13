#ifndef PANELWIDGET_H
#define PANELWIDGET_H
#include "status/statuswidget.h"

#include "dmenubar/menuapplications.h"
#include <QWidget>
#include <QX11Info>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QDebug>
#include <QProcess>
#include <QLayout>
#include <QAbstractNativeEventFilter>
#include "xcb/xcb.h"
namespace Ui {
class PanelWidget;
}

//#include "X11/Xlib.h"
//#include <X11/Xatom.h>
#include "etaskbar/dtaskbarwidget.h"
#include "dsystray/systray.h"
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

private:
    Ui::PanelWidget *ui;
StatusWidget *mStatusWidget;
DtaskbarWidget *mDtaskbarWidget;
MenuApplications *mMenuApplications;
SysTray *mSysTray;
int m_Position;

private slots:

    void resizePanel();
    //X11
    void moveToAllDesktop();
    void setStrut(int top,  int bottom, int topStartX,int topEndX,int bottomStartX, int bottomEndX );
void loadSettings();
void loadIconThems();

};

#endif // PANELWIDGET_H
