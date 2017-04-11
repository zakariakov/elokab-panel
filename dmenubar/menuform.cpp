#include "menuform.h"
#include "openexec.h"
#include "ui_menuform.h"
#include <QListWidget>
#include <QStackedWidget>
#include <QToolButton>
#include <QLineEdit>
#include <QX11Info>
#include <QDesktopWidget>
#include <QDebug>
#include <QGraphicsDropShadowEffect>

MenuForm::MenuForm(QWidget *parent) :
    QWidget(parent,  Qt::Popup | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint /*| Qt::X11BypassWindowManagerHint*/),
    m_pos(0,0),
    ui(new Ui::MenuForm)
{
    this->setAttribute(Qt::WA_TranslucentBackground,true);

    ui->setupUi(this);

    menuProgrammes=new MenuProgrammes(this);
    connect(menuProgrammes,SIGNAL(menuRecharged()),this,SLOT(rechargeMenu()));

    mMenuRecent=new MenuRecent(this);
    connect(this,SIGNAL(actionExecuted(QAction*)),mMenuRecent,SIGNAL(actionAdded(QAction*)));

    menuPower=new MenuSystem();

    menuFolders=new MenuFolders(this);
    chargeMenuAll();
    chargeMenuRecent();
    chargeMenuProgrammes();
    chargeMenufolder();
    chargeMenuSystem();
    changeStyle();

    QGraphicsDropShadowEffect *fx = new QGraphicsDropShadowEffect();

    fx->setOffset(0);
    fx->setBlurRadius(14);
    fx->setColor(QColor(0, 0, 0, 200));

    setGraphicsEffect(fx);

}

MenuForm::~MenuForm()
{
    delete ui;
}
//________________________________________________________________
void MenuForm::chargeMenuAll()
{
    QListWidgetItem *item=new QListWidgetItem;
    item->setText(tr("All progrrames"));
    item->setIcon(QIcon::fromTheme("application-x-executable"));
    item->setData(Qt::UserRole,ui->stackedWidget->count());
    ui->listWidgetMenu->addItem(item);

    listWidgetAll=new QListWidget;
    listWidgetAll->setFrameShape(QFrame::NoFrame);
    connect(listWidgetAll,SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(execItem(QListWidgetItem*)));
    connect(listWidgetAll,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(execItem(QListWidgetItem*)));

    ui->stackedWidget->addWidget(listWidgetAll);


}
//________________________________________________________________
void MenuForm::chargeMenuRecent()
{
    QListWidgetItem *item=new QListWidgetItem;
    item->setText(mMenuRecent->title());
    item->setIcon(mMenuRecent->icon());
    item->setData(Qt::UserRole,ui->stackedWidget->count());
    ui->listWidgetMenu->addItem(item);

   listWidgetRecent=new QListWidget;
   listWidgetRecent->setFrameShape(QFrame::NoFrame);
    connect(listWidgetRecent,SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(execItem(QListWidgetItem*)));
    connect(listWidgetRecent,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(execItem(QListWidgetItem*)));

    ui->stackedWidget->addWidget(listWidgetRecent);

    foreach (QAction *act, mMenuRecent->actions()) {
        QListWidgetItem *item=new QListWidgetItem;

        item->setText(act->text());
        item->setIcon(act->icon());
        item->setData(Qt::UserRole,act->data());

        listWidgetRecent->addItem(item);
    }
}

//________________________________________________________________
void MenuForm::rechargeRecent()
{

int count=listWidgetRecent->count();

    for (int i = count; i >0; --i) {
        delete listWidgetRecent->item(i-1);
    }

    foreach (QAction *act, mMenuRecent->actions()) {
        QListWidgetItem *item=new QListWidgetItem;

        item->setText(act->text());
        item->setIcon(act->icon());
        item->setData(Qt::UserRole,act->data());

        listWidgetRecent->addItem(item);
    }
}

//________________________________________________________________
void MenuForm::chargeMenuProgrammes()
{

    foreach (QMenu *m, menuProgrammes->menus) {

        QListWidgetItem *item=new QListWidgetItem;
        item->setText(m->title());
        item->setIcon(m->icon());
        item->setData(Qt::UserRole,ui->stackedWidget->count());

        ui->listWidgetMenu->addItem(item);

        QListWidget *listWidget=new QListWidget;
        listWidget->setFrameShape(QFrame::NoFrame);
        connect(listWidget,SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(execItem(QListWidgetItem*)));
        connect(listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(execItem(QListWidgetItem*)));

        ui->stackedWidget->addWidget(listWidget);

        foreach (QAction *act, m->actions()) {
            QListWidgetItem *item=new QListWidgetItem;

            item->setText(act->text());
            item->setIcon(act->icon());
            item->setData(Qt::UserRole,act->data());
           item->setToolTip(act->toolTip());
            listWidget->addItem(item);
            listWidgetAll->addItem(item->clone());
    }

    }
}

//________________________________________________________________
void MenuForm::chargeMenufolder()
{
    QListWidgetItem *item=new QListWidgetItem;
    item->setText(menuFolders->title());
    item->setIcon(menuFolders->icon());
    item->setData(Qt::UserRole,ui->stackedWidget->count());

    ui->listWidgetMenu->addItem(item);
    QListWidget *listWidget=new QListWidget;
     listWidget->setFrameShape(QFrame::NoFrame);
    connect(listWidget,SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(execFolder(QListWidgetItem*)));
    connect(listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(execFolder(QListWidgetItem*)));

    ui->stackedWidget->addWidget(listWidget);

    foreach (QAction *act, menuFolders->actions()) {
        QListWidgetItem *item=new QListWidgetItem;

        item->setText(act->text());
        item->setIcon(act->icon());
        item->setData(Qt::UserRole,act->data());

        listWidget->addItem(item);
    }

}

//________________________________________________________________
void MenuForm::chargeMenuSystem()
{
    foreach (QAction *act, menuPower->actions()) {
        if(act->isSeparator())
            continue;

        QToolButton *btn=new QToolButton;
        btn->setDefaultAction(act);
        ui->horizontalLayoutSys->addWidget(btn);
    }
}

//________________________________________________________________
void MenuForm::on_listWidgetMenu_itemPressed(QListWidgetItem *item)
{
    int ind=item->data(Qt::UserRole).toInt();
    ui->stackedWidget->setCurrentIndex(ind);
    ui->lineEdit->clear();

}

//________________________________________________________________
void MenuForm::on_listWidgetMenu_currentItemChanged(QListWidgetItem *current, QListWidgetItem */*previous*/)
{
    int ind=current->data(Qt::UserRole).toInt();
    ui->stackedWidget->setCurrentIndex(ind);
    ui->lineEdit->clear();
}

//________________________________________________________________
void MenuForm::execItem(QListWidgetItem* item)
{


    QStringList datac=item->data(Qt::UserRole).toStringList();
    QString exec=datac.at(2).trimmed();
    //    qDebug()<<exec;
    OpenExec::execFile(exec);
    this->hide();

    QAction *act=new QAction;
    act->setIcon(item->icon());
    act->setText(item->text());
    act->setData(datac);
    emit actionExecuted(act);
    delete act;
   QTimer::singleShot(10,this,SLOT(rechargeRecent()));
}

//________________________________________________________________
void MenuForm::execFolder(QListWidgetItem* item)
{

    QString localDir=item->data(Qt::UserRole).toString();

    OpenExec::execFolder(localDir);
    this->hide();
}


//##############################################################//

//________________________________________________________________
void MenuForm::open(QPoint pos, int s)
{
    qDebug()<<m_pos<<s<<QApplication::desktop()->availableGeometry(pos);

    m_pos = pos;

    pMap=s;

    realign();
    show();
    ui->lineEdit->setFocus();
    QTimer::singleShot(5,this,SLOT(resizeMe()));

}

//________________________________________________________________
void MenuForm::resizeMe()
{
    //    resize(75,50);
    //    adjustSize();
    //    realign();
    int p1=m_pos.x()-this->pos().x()/*+pMap*/;
    if(p1>this->width()-30)
        p1=this->width()-30;
    if(p1<10)
        p1=10;

    ui->widgetTop->move(p1,0);
    ui->widgetBot->move(p1,0);
}

//________________________________________________________________
void MenuForm::realign()
{
    int mMargin;

    if(QX11Info::isCompositingManagerRunning()){
        ui->widgetTopContener->setVisible(true);
        ui->widgetBotContener->setVisible(true);
        mMargin=7;
    }else{
        ui->widgetTopContener->setVisible(false);
        ui->widgetBotContener->setVisible(false);
        mMargin=1;
    }

    ui->verticalLayoutContent->setContentsMargins(mMargin,mMargin,mMargin,mMargin);
    QRect screen = QApplication::desktop()->availableGeometry(m_pos);

    QRect rect;

    rect.setSize(this->size());

    rect.moveTo(m_pos-QPoint((this->width()/2),(this->height()/2)));

    if (rect.right() > screen.right())
        rect.moveRight(screen.right());


    if (rect.bottom() > screen.bottom()){
        rect.moveBottom(screen.bottom());
        ui->widgetTop->setVisible(false);
        ui->widgetBot->setVisible(true);
    }

    if (rect.left() < screen.left())
        rect.moveLeft(screen.left());

    if (rect.top() < screen.top()){
        rect.moveTop(screen.top());
        ui-> widgetTop->setVisible(true);
        ui-> widgetBot->setVisible(false);
    }

    move(rect.topLeft());
}

//________________________________________________________________
void MenuForm::changeStyle()
{
    QColor colB=this->palette().window().color();
    QString colF=this->palette().windowText().color().name();
    QString r=QString::number(colB.red());
    QString g=QString::number(colB.green());
    QString b=QString::number(colB.blue());
    QString a=QString::number(colB.alpha());
    QString stl=    QString(
                " QWidget#widgetBgr{"
                " border-radius:5;"
                " background-color: rgba(%1, %2, %3, %4);"
                " color: %5;"
                " }"
                " QWidget#widgetTop{"
                " background-color: qconicalgradient(cx:0.51, cy:0, angle:87.9, "
                " stop:0.43 rgba(255, 255, 255, 0), "
                "  stop:0.431 rgba(%1, %2, %3, %4), "
                "  stop:0.579 rgba(%1, %2, %3, %4), "
                "  stop:0.58 rgba(255, 255, 255, 0));"
                " }"

                " QWidget#widgetBot{"
                "background-color: qconicalgradient(cx:0.540612, cy:1, angle:270.198, "
                " stop:0.43 rgba(255, 255, 255, 0), "
                " stop:0.431 rgba(%1, %2, %3, %4), "
                " stop:0.579 rgba(%1, %2, %3, %4), "
                " stop:0.58 rgba(255, 255, 255, 0));"
                " }"

                " QListWidget{"
                " background-color: rgba(255, 255, 255, 0);"
                "color :%5"
                " }"
                )
            .arg(r)
            .arg(g)
            .arg(b)
            .arg(a)
            .arg(colF);

    setStyleSheet(stl);

}

void MenuForm::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1.isEmpty()){

        for (int i = 0; i < listWidgetAll->count(); ++i)
            listWidgetAll->item(i)->setHidden(false);

        return;
    }

    ui->stackedWidget->setCurrentWidget(listWidgetAll);

        for (int i = 0; i < listWidgetAll->count(); ++i) {
            listWidgetAll->item(i)->setHidden(true);
       QListWidgetItem *item= listWidgetAll->item(i);
       QStringList datac=item->data(Qt::UserRole).toStringList();
       QString exec=datac.at(2).trimmed();
       if(item->text().toLower().contains(arg1)
//               ||item->toolTip().contains(arg1)
               ||exec.toLower().contains(arg1))

           item->setHidden(false);

        }



}
