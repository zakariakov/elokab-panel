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
//------------------------------------------------------------------
MenuApplications::MenuApplications(QWidget *parent) :
    QToolButton(parent)

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
    setupMenu();
    // keySequence();

}

//------------------------------------------------------------------
void MenuApplications::setupMenu()
{


mFindButton=new FindButton;
 connect(mFindButton,SIGNAL(findTextChanged(QString)),this,SLOT(findText(QString)));
    menuProgrammes=new MenuProgrammes(this);
    connect(menuProgrammes,SIGNAL(menuRecharged()),this,SLOT(rechargeMenu()));
    mMenuRecent=new MenuRecent(this);
     connect(menuProgrammes,SIGNAL(actionExecuted(QAction*)),mMenuRecent,SIGNAL(actionAdded(QAction*)));
   menuPower=new MenuSystem();

    mnuFile=new QMenu(this);

    menuFolders=new MenuFolders(this);
    mnuFind=new QMenu(this);
    mnuFind->setTitle(tr("Find"));
    mnuFind->setIcon(QIcon::fromTheme("edit-find"));
    connect(mnuFind,SIGNAL(aboutToShow()),mFindButton,SLOT(setFocus()));


         setMenu(mnuFile);

    rechargeMenu();
    loadSettings();

}

//------------------------------------------------------------------
void MenuApplications::rechargeMenu()
{

 //clear();
     mnuFile->clear();

     mnuFind->clear();

     mnuFind->addAction(mFindButton->actionfind());

     mnuFind->addSeparator();
 //add menus
     mnuFile->addMenu(mnuFind);

    mnuFile->addSeparator();
    mnuFile->addMenu(mMenuRecent);
    foreach (QMenu *m, menuProgrammes->menus) {
        mnuFile->addMenu(m);

    }

    mnuFile->addSeparator();
    mnuFile->addMenu(menuFolders);
    mnuFile->addSeparator();

    mnuFile->addMenu(menuPower);



}


//------------------------------------------------------------------
void MenuApplications::loadSettings()
{


    QSettings setting;
    setting.beginGroup("Main");
    QString mparentColor=setting.value("BgColor","#404244").toString();
    QString mparentFColor=setting.value("FgColor","#FFFFFF").toString();
    setting.endGroup();

    DesktopFile xdg(setting.fileName(),"Menu");
    int bstyle=(xdg.value("Style",0).toInt());
    QString ButtonBgColor=xdg.value("ButtonBgColor",mparentColor).toString();
    QString ButtonFgColor=xdg.value("ButtonFgColor",mparentFColor).toString();
    QString MenuBgColor=xdg.value("MenuBgColor","#404244").toString();
    QString MenuFgColor=xdg.value("MenuFgColor","#FFFFFF").toString();
    QString MenuBorderColor=xdg.value("MenuBorderColor","#666666").toString();
    setText(xdg.value("Text",tr("Start")).toString());
    int radius=xdg.value("BoderRadius",0).toInt();



    mnuFile->setContentsMargins(radius,radius,radius,radius);
    QString stButton=MyStyle::taskbarStyle(bstyle).arg(mparentColor).arg(ButtonBgColor).arg(ButtonFgColor);
    QString stMenu=MyStyle:: menuColor()
            .arg(MenuBgColor)
            .arg(MenuFgColor)
            .arg(MenuBorderColor)
            .arg(QString::number(radius));


    setStyleSheet(stButton+stMenu);
//    mnuFile-> setPalette(Qt::transparent);
//    mnuFile->setAttribute(Qt::WA_TranslucentBackground,true);
    foreach (QMenu *m,menuProgrammes->menus) {
        m->setStyleSheet(stMenu);
    }

menuPower->setStyleSheet(stMenu);
}

//------------------------------------------------------------------
void MenuApplications::execApplication()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action){
        QString data=action->data().toString();
        QString exec=data.section("|",0,0);
        QProcess process;
        process.setWorkingDirectory(QDir::homePath());

        process.startDetached(exec);
        //   qDebug()<<exec;
    }
}



//------------------------------------------------------------------
void MenuApplications::afterMenuActivated()
{

    QRect screen = QApplication::desktop()->availableGeometry();
    int posy;
    QPoint ps=mapToGlobal(QPoint(this->rect().left(),this->rect().top()));
    if(ps.y()>=screen.height()){
        posy= this->rect().top()- mnuFile->sizeHint().height();;
    }else{
        posy= this->rect().bottom();
    }

    QPoint mpos;

        mpos=mapToGlobal(QPoint(this->rect().right()-mnuFile->sizeHint().width(),posy));


    mnuFile->exec(mpos);
    mnuFile->activateWindow();

     X11UTILLS::raiseWindow(mnuFile->winId());
     mnuFile->setFocus();

}

//------------------------------------------------------------------
void MenuApplications::showHideMenu()
{
    if (mnuFile && mnuFile->isVisible())
        mnuFile->hide();
    else
        showMenu();
}

//------------------------------------------------------------------
void MenuApplications::showMenu()
{

    mnuFile->activateWindow();
    mnuFile->setFocus();

    QTimer::singleShot(10, this, SLOT(afterMenuActivated()));
}

//------------------------------------------------------------------Find
void MenuApplications::findText(QString text)
{

    foreach (QAction *a, mnuFind->actions()) {

        if(a->objectName()=="ActionWidget")
            continue;

        mnuFind->removeAction(a);
    }

    if (text.isEmpty()) return;

    foreach (QMenu *m, menuProgrammes->menus) {
        foreach (QAction *a,   m->actions()) {
            QStringList data=a->data().toStringList();
            QString exec=data.at(0);
            if(exec.toLower().contains(text)
                    ||a->text().toLower().contains(text))
            {
                mnuFind->addAction(a);
            }
        }
    }

}




