#include "menuapplications.h"
#include "utils/mystyle.h"
#include "utils/desktopfile.h"
#include "utils/x11utills.h"

//#include "menuconfigdialog.h"
//#include <EMimIcon>

#include <QDir>
#include <QDebug>
#include<QSettings>
#include <QTextCodec>
#include  <QProcess>
#include  <QDesktopWidget>
#include  <QUrl>
#include  <QApplication>
#include  <QDebug>
#include  <QCompleter>

#define  Classic_Menu 0
#define  Form_Menu 1
//------------------------------------------------------------------
MenuApplications::MenuApplications(QWidget *parent) :
    QToolButton(parent),menuStyle(0),mMenuForm(0),mMenuClassic(0)

{
    QFont font=parent->font();
    font.setPointSize(parent->font().pointSize());
    setFont(font);
    setContentsMargins(0,0,0,0);
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    //this->setTitle(tQTextStreamrUtf8("Applications"));
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus ,false);
    this->setObjectName("MenuBar");     //اسم الاضافة
    this->setWindowTitle(tr("classic Menu Bar"));   //النص الذي سيتم عرضه لتعريف الاضافة
    this->setWhatsThis(tr("classic menu lancher by AbouZakaria"));//شرح الاضافة
    this->setWindowIcon(QIcon::fromTheme("start-here",QIcon::fromTheme("start-here")));//ايقونة الاضافة


    setPopupMode(QToolButton::InstantPopup);
    setCheckable(true);
    setChecked(true);

    loadSettings();
    if(menuStyle==Classic_Menu){
        mMenuClassic=new MenuClassic;
        setMenu(mMenuClassic);
    } else{
        mMenuForm=new MenuForm;
        connect(this,SIGNAL(pressed()),this,SLOT(showForm()));

    }

}

void MenuApplications::showForm()
{
    int x=this->geometry().x();
    int y=this->geometry().y();
    int map=this->geometry().width()/2;
    if(mMenuForm)
        mMenuForm->open(QPoint(x,y),map);

}

//------------------------------------------------------------------
void MenuApplications::loadSettings()
{


    QSettings setting;
    setting.beginGroup("Main");
    menuStyle=setting.value("MenuStyle",1).toInt();
    QString mparentColor=setting.value("BgColor","#404244").toString();
    QString mparentFColor=setting.value("FgColor","#FFFFFF").toString();
    QString fontName=setting.value("FontName").toString();
    int fontSize=setting.value("FontSize").toInt();
    setting.endGroup();

    QFont font;
    font.setFamily(fontName);
    font.setPointSize(fontSize);
    setFont(font);

    DesktopFile xdg(setting.fileName(),"Menu");
    int bstyle=(xdg.value("Style",0).toInt());
    QString ButtonBgColor=xdg.value("ButtonBgColor",mparentColor).toString();
    QString ButtonFgColor=xdg.value("ButtonFgColor",mparentFColor).toString();

    setText(xdg.value("Text",tr("Start")).toString());
    int radius=xdg.value("BoderRadius",0).toInt();



    this->setContentsMargins(radius,radius,radius,radius);
    QString stButton=MyStyle::taskbarStyle(bstyle).arg(mparentColor).arg(ButtonBgColor).arg(ButtonFgColor);



    setStyleSheet(stButton);
//    this-> setPalette(Qt::transparent);
//    this->setAttribute(Qt::WA_TranslucentBackground,true);

}

//------------------------------------------------------------------
void MenuApplications::afterMenuActivated()
{

    QRect screen = QApplication::desktop()->availableGeometry();
    int posy;
    QPoint ps=mapToGlobal(QPoint(this->rect().left(),this->rect().top()));
    if(menuStyle==Classic_Menu){
        if(!mMenuClassic)return;

        if(ps.y()>=screen.height()){
            posy= this->rect().top()- mMenuClassic->sizeHint().height();;
        }else{
            posy= this->rect().bottom();
        }

        QPoint mpos;

        mpos=mapToGlobal(QPoint(this->rect().right()-mMenuClassic->sizeHint().width(),posy));

        mMenuClassic->exec(mpos);
        mMenuClassic->activateWindow();
        X11UTILLS::raiseWindow(mMenuClassic->winId());
        mMenuClassic->setFocus();
    }else{
         if(!mMenuForm)return;
         showForm();
         mMenuForm->setFocus();
    }
}


//------------------------------------------------------------------
void MenuApplications::showMenu()
{
    if(menuStyle==Classic_Menu){
        if(!mMenuClassic)return;
        mMenuClassic->activateWindow();
        mMenuClassic->setFocus();
    }else{
        if(!mMenuForm)return;
        mMenuForm ->activateWindow();
        mMenuForm->setFocus();
    }

    QTimer::singleShot(10, this, SLOT(afterMenuActivated()));
}

