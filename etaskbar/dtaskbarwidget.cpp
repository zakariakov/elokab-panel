/********************************************************************
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
    m_horizontalLayout->addSpacing(10);

    horizontalSpacer = new QSpacerItem(5, 5, QSizePolicy::Expanding, QSizePolicy::Minimum);



    //     connect(KWindowSystem::self(), SIGNAL(stackingOrderChanged()), SLOT(refreshTaskList()));
    //        connect(KWindowSystem::self(), static_cast<void (KWindowSystem::*)(WId, NET::Properties, NET::Properties2)>(&KWindowSystem::windowChanged)
    //                , this, &LXQtTaskBar::onWindowChanged);
    //        connect(KWindowSystem::self(), &KWindowSystem::windowAdded, this, &LXQtTaskBar::onWindowAdded);
    //        connect(KWindowSystem::self(), &KWindowSystem::windowRemoved, this, &LXQtTaskBar::onWindowRemoved);



    loadSettings();
    qApp->installNativeEventFilter(this);

    refreshTaskList();
    //   this->setStyleSheet("QWidget{background-color: red  ;}");

}


bool DtaskbarWidget::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{


    if (eventType == "xcb_generic_event_t") {
        xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);


//        if(event->response_type!=wmRef){
//            wmRef=event->response_type;
//            if(wmRef==28)
//                refreshTaskList();
//        }
        switch (event->response_type & ~0x80) {
        // FIXME: there is no XDamageNotify in xcb?
        // case XCB_DAMAGE_NOTIFY: break;
        case XCB_KEYMAP_NOTIFY: {
           // qDebug() << "keymap";
            break;
        }
        case XCB_DESTROY_NOTIFY: {
            xcb_destroy_notify_event_t *destroy = reinterpret_cast<xcb_destroy_notify_event_t *>(event);
           // emit windowClosed(destroy->window);
            // qDebug() << "windowClosed";
            break;
        }
        case XCB_CONFIGURE_NOTIFY: {
            xcb_configure_notify_event_t *configure = reinterpret_cast<xcb_configure_notify_event_t *>(event);
//            emit windowReconfigured(configure->window, configure->x, configure->y,
//                configure->width, configure->height);
             //qDebug() << "windowReconfigured";
            break;
        }
        case XCB_PROPERTY_NOTIFY: {
            xcb_property_notify_event_t *property = reinterpret_cast<xcb_property_notify_event_t*>(event);
            windowPropertyChanged(property->window, property->atom);
            // qDebug() << "windowPropertyChanged";
            break;
        }
        case XCB_CLIENT_MESSAGE: {
            xcb_client_message_event_t *client = reinterpret_cast<xcb_client_message_event_t *>(event);
           // emit clientMessageReceived(client->window, client->type, client->data.data32);
            // qDebug() << "clientMessageReceived";
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
             qDebug()<<"DtaskbarWidget::windowPropertyChanged   _NET_CLIENT_LIST";
             refreshTaskList();
        }

        if(atom == X11UTILLS::atom("_NET_ACTIVE_WINDOW")){
//             qDebug()<<"DtaskbarWidget::windowPropertyChanged   _NET_ACTIVE_WINDOW";
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
    setting.endGroup();

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

    QString  mystyle=  MyStyle::taskbarStyle(Style).arg(mparentColor).arg(ActiveBgColor).arg(ActiveFgColor);

    setStyleSheet(mystyle);

    refreshTaskList();
}


//*******************taskbar**************************//

void DtaskbarWidget::refreshTaskList()
{

    //blockSignals(true);

    //              قائمة مؤقة بجميع معرفات النوافذ الحاضرة
   QList<unsigned long> listWindow = X11UTILLS::getClientList();
    if(listWindow.count()==mButtonsHash.count())
    {

        activeWindowChanged();
        //   blockSignals(false);
        return;
    }


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

        //        اضافة الزر للبنال
        m_horizontalLayout->addWidget(btn);
       // btn->setIconSize(m_size);

    }

    m_horizontalLayout->addItem(  horizontalSpacer);




    foreach (DActionTaskbar *btn,mButtonsHash){

        if(m_iconStyle==TEXTBICON){
            btn->setMaximumWidth(mBtnWidth);
           //  btn->setMaximumHeight(m_size.height());

            btn->adjustSize();

        }else{

            btn->setMaximumWidth(btn->sizeHint().width()+10);
            btn->setMinimumWidth(btn->sizeHint().width()+10);
 btn->adjustSize();
        }
    }

    activeWindowChanged();
adjustSize();
    //blockSignals(false);

}

// النافذة المفعلة
void DtaskbarWidget::activeWindowChanged()
{

    //        البحث عن النافذة المفعلة
    unsigned long window =X11UTILLS::getActiveAppWindow();


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


void DtaskbarWidget::setSize(QSize size)
{
    m_size=size;

    this->setMinimumHeight(m_size.height());
    refreshTaskList();
}

//======================== STYLE  ========================
QString DtaskbarWidget::getStyle(int Style)
{

    switch (Style) {
    case 1:
        //  
        return   (QLatin1String(
                               "QToolButton{\n"
                               "background-color: transparent;\n"
                               "border-left:  10px solid transparent;\n"
                               " border-radius: 0px;\n"
                               "border-right:  10px solid transparent;\n"
                               "}"
                                "QToolButton:checked{\n"
                                  "background-color: %2;\n"
                                  "color: %3;\n"
                                "border-left:  10px solid ;\n"
                                " border-radius: 0px;\n"
                                "border-right:  10px solid ;\n"
                                "border-right-color: qconicalgradient(cx:1, cy:0.506, angle:358.2, stop:0.429787 %1, stop:0.431 %2, stop:0.579 %2, stop:0.58 %1);\n"
                                "\n"
                                "	border-left-color: qconicalgradient(cx:1, cy:0.506, angle:358.2, stop:0.429787 transparent, stop:0.431 %1, stop:0.579 %1, stop:0.58 transparent);\n"

                                "}"));
        break;
    case 2:
        // 
        return   (QLatin1String(
                      "QToolButton{\n"
                      "background-color: transparent;\n"
                      "border-left:  10px solid transparent;\n"
                      " border-radius: 0px;\n"
                      "border-right:  10px solid transparent;\n"
                      "} \n"
                      "QToolButton:checked{"
                      " color: %3;\n"
                      " border-right:10 solid; \n"
                      " border-left:10 solid; \n"
                       " background-color: %2; \n"
                    "  border-left-color: qconicalgradient(cx:0, cy:0.533955, angle:180, stop:0.429787 %1, stop:0.431 %2, stop:0.579 %2, stop:0.58 %1); \n"
                       " border-right-color: qconicalgradient(cx:0, cy:0.533955, angle:180, stop:0.429787 transparent, stop:0.431 %1, stop:0.579 %1, stop:0.58 transparent);"

                      "}"));
        break;
    case 3:
        return QString(
                    "QToolButton{\n"
                    "background-color: %1;\n"
                    "border-left:  10px solid %1;\n"
                    " border-radius: 0px;\n"
                    "border-right:  10px solid %1;\n"
                    "} \n"
                    "QToolButton:checked{"
                    " color: %3;\n"
                     "background-color: %2;\n"
                    " border-left-color: qconicalgradient(cx:1, cy:1, angle:315, stop:0.5 %2, stop:0.501 %1);"

                    " border-right-color: qconicalgradient(cx:1, cy:1, angle:315, stop:0.5 %1, stop:0.501 %2);"
                   "}"
                    );
        break;
    case 4:
        return QString(
                    "QToolButton{\n"
                    "background-color: %1;\n"
                    "border-left:  10px solid %1;\n"
                    " border-radius: 0px;\n"
                    "border-right:  10px solid %1;\n"
                    "} \n"
                    "QToolButton:checked{"
                    " color: %3;\n"
                     "background-color: %2;\n"
                  "  border-left-color: qconicalgradient(cx:0, cy:1, angle:225, stop:0.5 %2, stop:0.501 %1);"
                   " border-right-color: qconicalgradient(cx:0, cy:1, angle:225, stop:0.5 %1, stop:0.501 %2); "
                "}"

                    );
        break;
    case 5:
        return QString("QToolButton{ background-color: %1 ;  border:  0px solid; border-radius: 0px;} \n"
                      " QToolButton:checked{"
                       " color: %3 ;"
                       " border:  0px solid; "
                       " border-radius: 0px; "
                       " background-color: %2 ;} "

                       );


        break;
    default:
        return QString("QToolButton{ background-color: palette(base) ; color: palette(windowText) ; border:  0px solid; border-radius: 0px;}"
                      " QToolButton:checked{ background-color: palette(highlight) ;color: palette(highlightedtext); border:  0px solid ; border-radius: 0px;}"
                       );

        break;
    }
    return QString("QToolButton{ background-color: palette(base) ; color: palette(windowtext) ; border:  0px solid; border-radius: 0px;}"
                  " QToolButton:checked{ background-color: palette(highlight) ;color: palette(highlightedtext); border:  0px solid ; border-radius: 0px;}"
                   );



}
