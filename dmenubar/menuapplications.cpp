#include "menuapplications.h"
#include "utils/mystyle.h"
#include "utils/desktopfile.h"
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
//#include <X11/Xlib.h>


MenuApplications::MenuApplications(QWidget *parent) :
    QToolButton(parent)

{
    QFont font=parent->font();
   font.setPointSize(parent->font().pointSize());
   setFont(font);

    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    //this->setTitle(tQTextStreamrUtf8("Applications"));
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus ,false);
    this->setObjectName("MenuBar");     //اسم الاضافة
    this->setWindowTitle(tr("classic Menu Bar"));   //النص الذي سيتم عرضه لتعريف الاضافة
    this->setWhatsThis(tr("classic menu lancher by AbouZakaria"));//شرح الاضافة
    this->setWindowIcon(QIcon::fromTheme("start-here",QIcon::fromTheme("start-here")));//ايقونة الاضافة

    setContentsMargins(0,0,0,0);
    setPopupMode(QToolButton::InstantPopup);
    setCheckable(true);
setChecked(true);
    setupMenu();
    // keySequence();

}



void MenuApplications::setupMenu()
{

    menuProgrammes=new MenuProgrammes(this);
    connect(menuProgrammes,SIGNAL(menuRecharged()),this,SLOT(rechargeMenu()));

   menuPower=new MenuSystem;

    mnuFile=new QMenu(this);

    menuFolders=new MenuFolders(this);


    setMenu(mnuFile);
    rechargeMenu();
    loadSettings();

}
void MenuApplications::rechargeMenu()
{
    //this->clear();

    mnuFile->clear();

    mnuFile->addActions(m_actList);
    mnuFile->addSeparator();
    foreach (QMenu *m, menuProgrammes->menus) {
        mnuFile->addMenu(m);

    }

    mnuFile->addSeparator();
    mnuFile->addMenu(menuFolders);
    mnuFile->addSeparator();

    mnuFile->addMenu(menuPower);

  //  setMenu(mnuFile);


//    adjustSize();
}



void MenuApplications::loadSettings()
{


    QSettings setting;
    //load setting
    DesktopFile xdg(setting.fileName(),"Menu");
    int bstyle=(xdg.value("Style",0).toInt());
    QString ButtonBgColor=xdg.value("ButtonBgColor","#1E90FF").toString();
    QString ButtonFgColor=xdg.value("ButtonFgColor","#FFFFFF").toString();
    QString MenuBgColor=xdg.value("MenuBgColor","#404244").toString();
    QString MenuFgColor=xdg.value("MenuFgColor","#FFFFFF").toString();
    setText(xdg.value("Text",trUtf8("Start")).toString());
    int radius=xdg.value("BoderRadius",0).toInt();



    mnuFile->setContentsMargins(radius,radius,radius,radius);
    QString sty=MyStyle::taskbarStyle(bstyle).arg("transparent").arg(ButtonBgColor).arg(ButtonFgColor);
    QString st=QString(

                "QMenu {"
                " border-radius: 5px;\n"
                " background-color: %1; "
                " border: 1px solid black;"
                "color: #ffffff;\n"
                " }"

                " QMenu::item:selected {"
                " background-color: %2;"
                "color: %1;\n"
                " }"
                ).arg(MenuBgColor).arg(MenuFgColor);
    setStyleSheet(sty+st);
    mnuFile-> setPalette(Qt::transparent);
    mnuFile->setAttribute(Qt::WA_TranslucentBackground,true);
    foreach (QMenu *m,menuProgrammes->menus) {
        m->setStyleSheet(st);
    }

}

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

void MenuApplications::refreshIcons()
{

    menuProgrammes->refreshIcons();
   // toolStartHere->setIcon(QIcon::fromTheme("start-here"));
   // mnuFile->setIcon(QIcon::fromTheme("start-here").pixmap(QSize(128, 128)));
    menuFolders->refreshIcons();

    menuPower->refreshIcons();

}

//void  MenuApplications::setKeySequence(QKeySequence sequence)
//{
//    //  qDebug()<<"plugins-----key---:b "<<sequence.toString();
//    QKeySequence ks(m_ks);
//    if(ks==sequence){
//        mnuFile->show();
//       activateWindow();

//        QTimer::singleShot(10, this, SLOT(afterMenuActivated()));
//    }
//}

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

    if (isLeftToRight())
    {
        mpos=mapToGlobal(QPoint(toolStartHere->rect().left(),posy));
    }else{
        mpos=mapToGlobal(QPoint(this->rect().right()-mnuFile->sizeHint().width(),posy));
    }

    mnuFile->activateWindow();
    mnuFile->setFocus();
    //    mnuFile->hide();
    mnuFile->exec(mpos);


}
void MenuApplications::showHideMenu()
{
    if (mnuFile && mnuFile->isVisible())
        mnuFile->hide();
    else
        showMenu();
}
void MenuApplications::showMenu()
{

    mnuFile->activateWindow();
    QTimer::singleShot(10, this, SLOT(afterMenuActivated()));
}

//QString   MenuApplications:: keySequence()
//{

//    QSettings settings("elokab","shortcut");
//    settings.beginGroup("Panel-Plugins");
//    settings.beginGroup("MenuBar");
//    if(!settings.contains("Destination")){
//        settings.setValue("Key",QKeySequence("Alt+F1").toString());
//        settings.setValue("Destination","panel");
//         settings.setValue("Comment","show the Main Menu panal");

//    }


//    m_ks= settings.value("Key","Alt+F1").toString();
//    settings.endGroup();//MenuBar
//    settings.endGroup();//Panel-Plugins
//    return m_ks;
//}
//void  MenuApplications::setActions(QList<QAction *>list)
//{
//    m_actList=list;
//    rechargeMenu();
//}

void MenuApplications::setSize(QSize size)
{
   // toolStartHere->setIconSize(size);
    //  toolStartHere->setMaximumHeight(size.height()+4);
  //  rechargeMenu();
}


//void MenuApplications::setLayout(QHBoxLayout *p)
//{
//    p->addWidget(this);
//}



