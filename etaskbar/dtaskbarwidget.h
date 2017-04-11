#ifndef DTASKBARWIDGET_H
#define DTASKBARWIDGET_H

#include "dactiontaskbar.h"
#include <QWidget>

#include <QLayout>
#include <QSpacerItem>
//#include <KF5/KWindowSystem/KWindowSystem>
//#include <KF5/KWindowSystem/KWindowInfo>
//#include <KF5/KWindowSystem/NETWM>

#include "X11/Xlib.h"
#include <X11/Xatom.h>
#include <xcb/xcb_event.h>

class DtaskbarWidget : public QWidget,public QAbstractNativeEventFilter
{

          Q_OBJECT

public:
          // DtaskbarWidget();
          explicit DtaskbarWidget(QWidget *parent = 0);

      bool nativeEventFilter(const QByteArray &eventType, void *message, long *);
      void windowPropertyChanged(unsigned long window, unsigned long atom);

protected:

signals:
      void wmChanged();
public slots:

void loadSettings();


private slots:

void init();
          void setIconStyle();


          void refreshTaskList();
          void activeWindowChanged();

          void wheelEvent(QWheelEvent* event);


private:
         // QMap<unsigned long, Client*> m_clients;
          QHash<unsigned long, DActionTaskbar*> mButtonsHash;
          int m_iconStyle;
          QSize    m_size;

          Qt::ToolButtonStyle mButtonStyle;
          unsigned long m_rootWindow;
           unsigned long m_activeWindow;
          QHBoxLayout * m_horizontalLayout ;
          QHBoxLayout *horizontalLayoutSpace;
          QSpacerItem *horizontalSpacer;

int mBtnWidth;
           int wmRef;
QWidget *m_parent;

};

#endif // DTASKBARWIDGET_H
