﻿/********************************************************************
 *(c)GPL3+

original code  http://razor-qt.org
 modified by abou zakaria
*********************************************************************/
#include "dtaskbarwidget.h"
#include "dactiontaskbar.h"
#include "taskbarsettingdialog.h"
#include "utils/mystyle.h"
#include <QtGui>
#include <QX11Info>
#include <QDesktopWidget>
#include <QMenu>
#include <QDebug>
#include "utils/x11utills.h"
#include "xcb/xcb.h"

#define TEXTBICON 0
#define ICONONLY 1

DtaskbarWidget::DtaskbarWidget(QWidget *parent):
    QWidget(parent)
{

    QFont font=parent->font();
    font.setPointSize(parent->font().pointSize());
    setFont(font);
    this->setObjectName("dtaskbar");
    this->setWindowTitle("Taskbar");
    this->setWhatsThis(tr("by AbouZakaria"));//شرح الاضافة
    this->setWindowIcon(QIcon::fromTheme("gnome-panel-window-list",QIcon(":/icons/panel-window-list")));

    m_rootWindow = QX11Info::appRootWindow();
    m_horizontalLayout = new QHBoxLayout(this);
    m_horizontalLayout->setSpacing(0);
    m_horizontalLayout->setContentsMargins(0,0,0,0);
    m_horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    m_horizontalLayout->addSpacing(3);

    horizontalSpacer = new QSpacerItem(5, 5, QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_horizontalLayout->addItem(horizontalSpacer);

    //     connect(KWindowSystem::self(), SIGNAL(stackingOrderChanged()), SLOT(refreshTaskList()));
    //        connect(KWindowSystem::self(), static_cast<void (KWindowSystem::*)(WId, NET::Properties, NET::Properties2)>(&KWindowSystem::windowChanged)
    //                , this, &LXQtTaskBar::onWindowChanged);
    //        connect(KWindowSystem::self(), &KWindowSystem::windowAdded, this, &LXQtTaskBar::onWindowAdded);
    //        connect(KWindowSystem::self(), &KWindowSystem::windowRemoved, this, &LXQtTaskBar::onWindowRemoved);



    loadSettings();
    qApp->installNativeEventFilter(this);

    QTimer::singleShot(100,this,SLOT(init()));

    //   this->setStyleSheet("QWidget{background-color: red  ;}");

}

void DtaskbarWidget::init()
{

    // qApp->installNativeEventFilter(this);

}

bool DtaskbarWidget::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{


    if (eventType == "xcb_generic_event_t") {
        xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);



        switch (event->response_type & ~0x80) {

        case XCB_PROPERTY_NOTIFY: {
            xcb_property_notify_event_t *property = reinterpret_cast<xcb_property_notify_event_t*>(event);


            windowPropertyChanged(property->window, property->atom);

            break;
        }

        default: break;
        }


    }

    return false;
}

void DtaskbarWidget::windowPropertyChanged(unsigned long window, unsigned long atom)
{
    if (window ==m_rootWindow) {

        if (atom == X11UTILLS::atom("_NET_CLIENT_LIST")){
            //  qDebug()<<"DtaskbarWidget::windowPropertyChanged   _NET_CLIENT_LIST";
            refreshTaskList();
        }

        if(atom == X11UTILLS::atom("_NET_ACTIVE_WINDOW")){
            // qDebug()<<"DtaskbarWidget::windowPropertyChanged   _NET_ACTIVE_WINDOW";

            activeWindowChanged();
        }

        return;
    }


    if(mButtonsHash.contains(window)){

        mButtonsHash[window]->windowPropertyChanged(atom);
    }

}

void DtaskbarWidget::setIconStyle()
{
    loadSettings();

    QList<DActionTaskbar *> allButtons = this->findChildren<DActionTaskbar *>();
    foreach (DActionTaskbar *btn, allButtons)
    {
        btn->setToolButtonStyle(mButtonStyle);
        //   btn->setIconSize(m_size);
        if(m_iconStyle==TEXTBICON){

            btn->setMaximumWidth(QWIDGETSIZE_MAX);
        }else{

            btn->setMaximumWidth(m_size.width());
        }
    }
}

void DtaskbarWidget::loadSettings()
{
    //get parent colors
    QSettings setting;
    setting.beginGroup("Main");
    QString mparentColor=setting.value("BgColor","#404244").toString();
    QString fontName=setting.value("FontName").toString();
    int fontSize=setting.value("FontSize").toInt();
    setting.endGroup();

    QFont font;
    font.setFamily(fontName);
    font.setPointSize(fontSize);
    setFont(font);
    QFontMetrics fm(font);
    int size=fm.height();
    foreach (DActionTaskbar *btn, mButtonsHash) {
        btn->setFont(font);
        btn->setIconSize(QSize(size,size));
    }
    //get setting for the taskbar
    setting.beginGroup("Taskbar");
    m_iconStyle=setting.value("IconStyle",0).toInt();
    QString  ActiveBgColor=setting.value("ActiveBgColor","#1E90FF").toString();
    QString  ActiveFgColor=setting.value("ActiveFgColor","#FFFFFF").toString();
    int Style=setting.value("Style",0).toInt();
    int IconSize=setting.value("IconSize",0).toInt();
    mBtnWidth=setting.value("ButtonWidth",100).toInt();
    setting.endGroup();

    //
    if(m_iconStyle==TEXTBICON){
        mButtonStyle=(Qt::ToolButtonTextBesideIcon);
    }else{
        mButtonStyle=(Qt::ToolButtonIconOnly);
    }

    m_size=(QSize(IconSize,IconSize));
    QString  mystyle;
    if(Style<1||Style>5)
        mystyle=  MyStyle::taskbarStyle(Style).arg(mparentColor);
    else
        mystyle=  MyStyle::taskbarStyle(Style).arg(mparentColor).arg(ActiveBgColor).arg(ActiveFgColor);

    setStyleSheet(mystyle);


    refreshTaskList();
}


//*******************taskbar**************************//

void DtaskbarWidget::refreshTaskList()
{


    //              قائمة مؤقة بجميع معرفات النوافذ الحاضرة
    QList<unsigned long> listWindow = X11UTILLS::getClientList();


    //                  حذف جميع الازرار ومعرفاتها السابقة
    if(mButtonsHash.count()>0){
        //              قائمة بجميع معرفات النولفذ والازرار
        QMutableHashIterator<unsigned long, DActionTaskbar*> i(mButtonsHash);
        while (i.hasNext())
        {

            i.next();

            int     r = listWindow.removeAll(i.key());

            if (!r)
            {
                delete i.value();
                i.remove();
            }

        }
    }

    m_horizontalLayout->removeItem(horizontalSpacer);

    foreach (unsigned long wnd, listWindow)
    {


        //            انشاء زر جديد للنوافذ الحاضرة
        DActionTaskbar *btn=new DActionTaskbar(wnd,this);
        //           نوع الزر هل هو ارمز والتص ام الرمز فقط
        btn->setToolButtonStyle(mButtonStyle);

        //            اضافة ازر للقائمة
        mButtonsHash.insert(wnd, btn);

        qDebug()<<mButtonsHash.count() <<this->width();

        //TODO fix This
        //     if(mButtonsHash.count() *mBtnWidth <this->width()-20)

        //        اضافة الزر للبنال
        m_horizontalLayout->addWidget(btn);


    }

    m_horizontalLayout->addItem(  horizontalSpacer);





    foreach (DActionTaskbar *btn,mButtonsHash){

        if(m_iconStyle==TEXTBICON){
            btn->setMaximumWidth(mBtnWidth);

        }else{

            btn->setMaximumWidth(btn->sizeHint().width()+7);
            btn->setMinimumWidth(btn->sizeHint().width()+7);

        }
        btn->adjustSize();
    }

    activeWindowChanged();
    //adjustSize();
    //blockSignals(false);

}

// النافذة المفعلة
void DtaskbarWidget::activeWindowChanged()
{

    //        البحث عن النافذة المفعلة
    unsigned long window =X11UTILLS::getActiveAppWindow();

    if( ! window)
        return;


    foreach (DActionTaskbar *btn,mButtonsHash){

        btn->setText(X11UTILLS::getWindowTitle(mButtonsHash.key(btn)));
        btn->setChecked(false);

        btn->setActiveWin(false);
    }


    //         التاكد من وجودها مع الازرا
    DActionTaskbar* toolbtn =0;
    if (mButtonsHash.contains(window)){
        toolbtn=mButtonsHash.value(window);
        m_activeWindow=window;
        toolbtn->setActiveWin(true);
        // toolbtn->setChecked(true);

    }



}


//***********************mose wheel*************
void DtaskbarWidget::wheelEvent(QWheelEvent* event)
{

    QList<unsigned long> winList = X11UTILLS::getClientList();
    int current = winList.indexOf(X11UTILLS::getActiveAppWindow());
    int delta = event->delta() < 0 ? 1 : -1;

    for (int ix = current + delta; 0 <= ix && ix < winList.size(); ix += delta)
    {
        unsigned long window = winList.at(ix);

        X11UTILLS::raiseWindow(window);
        break;

    }

}


