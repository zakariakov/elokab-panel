#include "panel_adaptor.h"
#include "ui_panelwidget.h"
#include "panelwidget.h"
#include <QSettings>
  #include <QtPlatformHeaders/QXcbWindowFunctions>
//#include "etaskbar/dtaskbarwidget.h"
//#include <X11/Xlib-xcb.h>
//#include <xcb/xcb_ewmh.h>
//#include "xcb/xproto.h"
//#include<xcb/xcb.h>

PanelWidget::PanelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanelWidget)
{


    QByteArray sS=qgetenv("DESKTOP_SESSION");
    qDebug()<<"Envirenment :"<< sS;
    if(sS=="i3")
        setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    else
        setWindowFlags(/*Qt::BypassWindowManagerHint | */Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    //TODO FIX This
    // setAttribute(Qt::WA_X11NetWmWindowTypeDock);

    this->winId(); //have to create the QWindow
    tlwWindow = this->windowHandle();
    QXcbWindowFunctions::setWmWindowType(tlwWindow, QXcbWindowFunctions::Dock);

    setDock ();
    setAttribute(Qt::WA_AlwaysShowToolTips);
    setAttribute(Qt::WA_X11DoNotAcceptFocus);
    setWindowTitle(tr("Panel"));
    setObjectName("PanelWidget");
    //      setPalette(Qt::transparent);
    //  setAttribute(Qt::WA_TranslucentBackground,true);

//setLayoutDirection(qApp->layoutDirection());

    ui->setupUi(this);
    loadIconThems();



    mSysTray=new SysTray;
    mMenuApplications =new MenuApplications(this);
    mStatusWidget=new StatusWidget(this);
    mDtaskbarWidget=new DtaskbarWidget(this);

    mPager=new Pager(this);

    ui->horizontalLayout->setSpacing(0);
    ui->horizontalLayout->addWidget(mMenuApplications);
    ui->horizontalLayout->addWidget(mPager);
    ui->horizontalLayout->addWidget(mDtaskbarWidget);
    ui->horizontalLayout->addWidget(mStatusWidget);
    ui->horizontalLayout->addWidget(mSysTray);

 loadSettings();
    //-----------------------------------------------------------------
    connect(QApplication::desktop(),SIGNAL(workAreaResized(int)),this,SLOT(resizePanel()));
    moveToAllDesktop();

    //testingh
    //adjustSize();

    //connect( mInterface,SIGNAL(themChanged()),this,SLOT(reconfigureThemes()));
 resizePanel();

}



PanelWidget::~PanelWidget()
{
    delete ui;
}



void PanelWidget::setDock ()
{
    if (winId() == 0)
        return ;

    if (!QX11Info::display())
        return ;
    //unsigned int wid=getWindowPID(winId());

    Atom a[2] = { None, None };
    a[0] = XInternAtom (QX11Info::display(), "_NET_WM_WINDOW_TYPE_DOCK", false);
    Atom a2 = XInternAtom (QX11Info::display(), "_NET_WM_WINDOW_TYPE", false);

    XChangeProperty (QX11Info::display(),
                     tlwWindow->winId(),
                     a2, XA_ATOM, 32, PropModeReplace, (unsigned char *) a, 1);


    //---------------------------------------------------------------------------TODO
    //return;

    //    xcb_connection_t *c = XGetXCBConnection( QX11Info::display());
    //    xcb_ewmh_connection_t EWMH;
    //    xcb_intern_atom_cookie_t *EWMHCookie = xcb_ewmh_init_atoms(c, &EWMH);
    //    if (! xcb_ewmh_init_atoms_replies(&EWMH, EWMHCookie, NULL)) {

    //        return ;
    //    }


    //    xcb_change_property(c,       /* Connection to the X server */
    //                        XCB_PROP_MODE_REPLACE,     /* Property mode */
    //                        wid,   /* Window */
    //                        EWMH._NET_WM_WINDOW_TYPE, /* Property to change */
    //                        XCB_ATOM_ATOM,     /* Type of the property */
    //                        32,   /* Format of the property (8, 16, 32) */
    //                        1, /* Length of the data parameter */
    //                        &(EWMH._NET_WM_WINDOW_TYPE_DOCK));    /* Data */
    //----------------------------------------------------------------------------
}


void PanelWidget::reconfigure()
{

    loadSettings();
    mMenuApplications->loadSettings();
    mDtaskbarWidget->loadSettings();
    mStatusWidget->loadSettings();
    mPager->loadSettings();
     resizePanel();
}
void PanelWidget::showMenu()
{
    if(mMenuApplications)
        mMenuApplications->showMenu();
}

void PanelWidget::loadSettings()
{
    //    BgColor=#244E36
    //    FgColor=#FFFFFF
    //    Boder=false
    //    BoderColor=#FF5500

    QSettings setting;
    setting.beginGroup("Main");
    QString fontName=setting.value("FontName").toString();
    int fontSize=setting.value("FontSize").toInt();

    QFont font;
    font.setFamily(fontName);
    font.setPointSize(fontSize);
    setFont(font);

    QFontMetrics fm(font);
    int size=fm.height();
    setMaximumHeight(size+5);
if(mSysTray)
    mSysTray->setIconSize(QSize(size+5,size+5));

    QString bgColor=setting.value("BgColor","#404244").toString();
    QString FgColor=setting.value("FgColor","#FFFFFF" ).toString();

    m_Position=setting.value("Position",1).toInt();



    //TODO FIX ALPHA IN mystyle.h and all moduls
    /*
    int Alpha=setting.value("Alpha",255).toInt();
    QColor bg=QColor(bgString);
    bg.setAlpha(Alpha);

    QString bgColor=QString("rgba(%1,%2,%3,%4)")
            .arg(bg.red())
            .arg(bg.green())
            .arg(bg.blue())
            .arg(bg.alpha());
    */


    this->setStyleSheet(QString("background-color: %1;color: %2;") .arg(bgColor) .arg(FgColor));





}

void PanelWidget::resizePanel()
{
    qDebug()<<"PanelWidget::resizePanel()===============================================";


    QFontMetrics fm(font());
    int size=fm.height();
    int m_panelHeight=size+5;

    QRect screen(QApplication::desktop()->screen(0)->geometry());
    qDebug()<<"   screen  :"<<screen;
    QRect rect;



    rect.moveLeft(screen.left());
    //  rect.moveTop(screen.top());
    if (m_Position == 0){
        this->setGeometry(screen.left(),screen.top(),screen.width(),m_panelHeight);
        rect = QRect(screen.left(),screen.top(),screen.width(),m_panelHeight);
        rect.moveTop(screen.top());
    } else {

        this->setGeometry(screen.left(),screen.bottom(),screen.width(),m_panelHeight);
        rect = QRect(screen.left(),screen.bottom(),screen.right(),m_panelHeight);
        rect.moveBottom(screen.bottom());
    }

    this->setGeometry(rect);
    int mScreenNum= QApplication::desktop()->primaryScreen();
    QRect desktop = QApplication::desktop()->screen(mScreenNum)->geometry();

    if (m_Position == 0){
        setStrut(  rect.bottom(), 0,
                   rect.left(), rect.right(),
                   0, 0
                   );
    }else{
        setStrut(   0, desktop.height() - rect.y(),
                    0, 0,
                    rect.left(), rect.right()
                    );

    }

    this->setGeometry(rect);
    this->resize(rect.width(),rect.height());
    qDebug()<<"   widget panel resized: "<<rect;

    setMaximumWidth(rect.width());
    setMinimumHeight(rect.height());
    //  m_rootWindow = QX11Info::appRootWindow();

}


/*-----------------------------------
     Begin       X11 LIB
-----------------------------------*/

Atom XFatom(const char* atomName)
{
    static QHash<QString, Atom> hash;

    if (hash.contains(atomName))
        return hash.value(atomName);

    Atom atom = XInternAtom(QX11Info::display(), atomName, false);
    hash[atomName] = atom;
    return atom;
}

void PanelWidget::setStrut(int top,  int bottom,
                           int topStartX,    int topEndX,
                           int bottomStartX, int bottomEndX )

{

    unsigned long desstrut[12];
    memset(desstrut,0,sizeof(desstrut));

    desstrut[0] = 0;
    desstrut[1] = 0;
    desstrut[2] = top;
    desstrut[3] = bottom;

    desstrut[4] = 0;
    desstrut[5] = 0;
    desstrut[6] = 0;
    desstrut[7] = 0;
    desstrut[8] = topStartX;
    desstrut[9] = topEndX;
    desstrut[10] = bottomStartX;
    desstrut[11] = bottomEndX;


    XChangeProperty(QX11Info::display(), this->winId() , XFatom("_NET_WM_STRUT_PARTIAL"),
                    XA_CARDINAL, 32, PropModeReplace,  (unsigned char *) desstrut, 12  );


    XChangeProperty(QX11Info::display(), this->winId(), XFatom("_NET_WM_STRUT"),
                    XA_CARDINAL, 32, PropModeReplace, (unsigned char*) desstrut, 4);
}



void PanelWidget::moveToAllDesktop()
{
    unsigned long desktop = true ? 0xFFFFFFFF : 0;

    XChangeProperty(QX11Info::display(), tlwWindow->winId(), XFatom("_NET_WM_DESKTOP"), XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(&desktop), 1);


    //TODO FIX THIS
    XClientMessageEvent msg;
    msg.window = tlwWindow->winId();
    msg.type = ClientMessage;
    msg.message_type = XFatom(/*"_NET_WM_WINDOW_TYPE_DOCK" */"_NET_WM_DESKTOP");
    msg.send_event = true;
    msg.display = QX11Info::display();
    msg.format = 32;
    msg.data.l[0] = -1;
    //TODO FIX This
    XSendEvent(QX11Info::display(), QX11Info::appRootWindow(0), 0, (SubstructureRedirectMask | SubstructureNotifyMask) , (XEvent *) &msg);


}

//! -x11-اشارات مكتبة  -------------------------------------------------- */

/*-----------------------------------
      End      X11 LIB
-----------------------------------*/

/*****************************************************************************************************
 *
 *****************************************************************************************************/
void PanelWidget::loadIconThems()
{

    QString icnThem=QIcon::themeName();

    QByteArray sS=qgetenv("DESKTOP_SESSION");

    if(sS=="elokab-session"){
        QSettings setting("elokab","elokabsettings");
        setting.beginGroup("Themes");
        icnThem=  setting.value("iconsTheme",QIcon::themeName()).toString();
        setting.endGroup();

    }

    if(icnThem=="hicolor"||icnThem.isEmpty()){

        QStringList failback;
        failback << "oxygen"<< "Mint-X"
                 << "Humanity"
                 << "Prudence-icon"<< "elementary"
                 <<"Adwaita"<<"breeze"<< "gnome";

        QDir dir("/usr/share/icons/");
        foreach (QString s, failback)
        {
            if (dir.exists(s))
            {
                icnThem=s;

                break;
            }
        }
    }

    if(icnThem.isEmpty())
        icnThem="hicolor";

    qDebug()<<"icon theme :"<< icnThem;
    QIcon::setThemeName(icnThem);



}
