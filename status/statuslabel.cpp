#include "statuslabel.h"
#include "utils/mystyle.h"
#include <QDebug>
#include <QApplication>
#include <QtConcurrent>
StatusLabel::StatusLabel(QColor bgColor,
                         QColor fgColor,
                         QWidget *parent,
                         const QString text):QLabel(parent)
{
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    //   setParent(parent);
    setContentsMargins(0,0,0,0);
    QFont font=parent->font();

    font.setPointSize(parent->font().pointSize());
    setFont(font);

    //   setLayoutDirection( parent->layoutDirection());
    mLabel=text;
    setText(mLabel);

    //__________________________________________________________
    m_render = new QFutureWatcher< void >(this);

    connect(m_render, SIGNAL(finished()),
            SLOT(on_render_finished()));
    connect(this, SIGNAL(textReady(QString))
            , SLOT(on_textReady(QString)));


    setStyleSheet(QString("background-color:%1; color:%2 ;").arg(bgColor.name()).arg(fgColor.name()));


}

//separator
StatusLabel::StatusLabel(QColor nextColor, QColor prevColor, QWidget *parent, int style):QLabel(parent)
{
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    QFont font=parent->font();
    font.setPointSize(parent->font().pointSize());
    setFont(font);

    //    setLayoutDirection( parent->layoutDirection());


    setMinimumWidth(7);
    QString styleSt;

    if(parent->isRightToLeft())
        styleSt= MyStyle::statusStyle(style).arg(prevColor.name()).arg(nextColor.name());
    else
        styleSt=  MyStyle::statusStyle(style).arg(nextColor.name()).arg(prevColor.name());

    setStyleSheet(styleSt);


}
//__________________________________________________________________________
void StatusLabel::on_render_finished()
{
    setText(QString(" %1 %2 ").arg(mLabel).arg(m_string.trimmed()));
}

void StatusLabel::on_textReady(QString str)
{
    if(!m_render->isCanceled()) m_string = str;
}

void StatusLabel::startRender()
{
    if(!m_render->isRunning())
        m_render->setFuture(QtConcurrent::run(this, &StatusLabel::render));
}

void StatusLabel::cancelRender()
{
    m_render->cancel();
    m_string = QString();
}

void StatusLabel::render()
{

    if(m_render->isCanceled()) return;

    QString  str = updateCommand();

    if(m_render->isCanceled())return;

    emit textReady(str);

}
QString StatusLabel::updateCommand()
{
    QProcess pr;
    pr.start(mCommand);
    if (!pr.waitForStarted())
        return QString();

    if (!pr.waitForFinished())
        return QString();

    QString result=pr.readAllStandardOutput();
    QString err=pr.readAllStandardError();

    QStringList list=result.split("\n",QString::SkipEmptyParts);
    QString s;
    if(list.count() >0){
        s=list.last();
        //        if(s.isEmpty())
        //            s=list.first();
    }
    if(!err.isEmpty())
        qDebug()<<"Error command"<<mName<<err;

    setText(QString(" %1 %2 ").arg(mLabel).arg(s.trimmed()));

    return s;

}
//________________________________________________________________________



void StatusLabel::setCommand(QString command, int interval)
{
    mTimer=new QTimer;
    mTimer->setInterval(interval);
    connect(mTimer,SIGNAL(timeout()),this,SLOT(startRender()/*updateCommand()*/));
    mCommand=command;
    mTimer->start();
    startRender(); //updateCommand();
}

