#include "panel_adaptor.h"
#include "ui_panelwidget.h"
#include "panelwidget.h"
#include <QSettings>
//#include "etaskbar/dtaskbarwidget.h"

PanelWidget::PanelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanelWidget)
{


    QByteArray sS=qgetenv("DESKTOP_SESSION");
    qDebug()<<"Envirenment :"<< sS;
    if(sS=="i3")
        setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    else
        setWindowFlags(Qt::BypassWindowManagerHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    setWindowTitle(tr("Panel"));
    setObjectName("PanelWidget");
    //setPalette(Qt::transparent);
    //setAttribute(Qt::WA_TranslucentBackground,true);
    setMaximumHeight(40);
    ui->setupUi(this);
    loadIconThems();
    mMenuApplications =new MenuApplications(this);
    mStatusWidget=new StatusWidget(this);

    mDtaskbarWidget=new DtaskbarWidget(this);
    mSysTray=new SysTray;
ui->horizontalLayout->setSpacing(0);
     ui->horizontalLayout->addWidget(mMenuApplications);
    ui->horizontalLayout->addWidget(mDtaskbarWidget);
    ui->horizontalLayout->addWidget(mStatusWidget);
    ui->horizontalLayout->addWidget(mSysTray);

    QFontMetrics fm(font());
    int size=fm.height();

    setGeometry(0,0,50,size+5);

    moveToAllDesktop();
    loadSettings();
    //testingh
    //adjustSize();

//connect( mInterface,SIGNAL(themChanged()),this,SLOT(reconfigureThemes()));

}

PanelWidget::~PanelWidget()
{
    delete ui;
}
void PanelWidget::reconfigure()
{
    qDebug()<<"called reconfigura";
    loadSettings();
   mMenuApplications->loadSettings();
   mDtaskbarWidget->loadSettings();
   mStatusWidget->loadSettings();
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
    QString BgColor=setting.value("BgColor","#404244").toString();
    QString FgColor=setting.value("FgColor","#FFFFFF"
                                            "").toString();
    m_Position=setting.value("Position",1).toInt();
    this->setStyleSheet(QString("background-color: %1;color: %2;").arg(BgColor).arg(FgColor));
    resizePanel();
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

    XClientMessageEvent msg;
    msg.window = this->winId();
    msg.type = ClientMessage;
    msg.message_type = XFatom("_NET_WM_WINDOW_TYPE_DOCK" /*"_NET_WM_DESKTOP"*/);
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
        failback << "oxygen"<< "Mint-X"<< "Humanity"<< "Tango"<< "Prudence-icon"<< "elementary"<< "gnome"<<"Adwaita";

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
