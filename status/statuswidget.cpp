#include "statuswidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QDebug>
#include <QDir>
StatusWidget::StatusWidget(QWidget *parent) : QWidget(parent)
{

    mHorizontalLayout = new QHBoxLayout(this);
    mHorizontalLayout->setSpacing(0);
    mHorizontalLayout->setContentsMargins(0, 0, 0, 0);
//    setContentsMargins(0, 0, 0, 0);
    QFont font=parent->font();
   font.setPointSize(parent->font().pointSize());
   setFont(font);
    loadSettings();
   // adjustSize();

}

void StatusWidget::loadSettings()
{
    //clear all StatusLabel;
    foreach (StatusLabel *s, listStatus) {
        delete s;
    }
    listStatus.clear();


    qDebug()<<"StatusWidget::loadSettings()===============";
    //TODO fix this to fileName
    QSettings setting;
    setting.beginGroup("Main");
    QString mparentColor=setting.value("BgColor","#404244").toString();
    QString mparentFColor=setting.value("FgColor","#FFFFFF").toString();
    setting.endGroup();

    setting.beginGroup("Status");
    int STyle=setting.value("STyle").toInt();
    setting.endGroup();

    QStringList list=DesktopFile::groupList(setting.fileName());

    //if list is empty adding timer and rturn
    if(list.isEmpty()){
        StatusLabel *sLabel=new StatusLabel(QColor(mparentColor),QColor(mparentFColor),this,QString());
        sLabel->setName("Time");
        sLabel->setCommand("date +%H:%M:%S",1000);
        mHorizontalLayout->addWidget(sLabel);
        adjustSize();
        return;
    }


    //begin color for first statu
    QString bgPrevColor=mparentColor;

    //default status separator
    bool sep=false;

    foreach (QString sName,list /*setting.childGroups()*/) {

        qDebug()<<"  "<<sName;

        //remplace the default setting for encoding char
        DesktopFile xdg(setting.fileName(),sName);

        if(xdg.isEmpty())
            continue;

        //read statu values
        QString Label=xdg.value("Label").toString();
        QString BgColor=xdg.value("BgColor",mparentColor).toString();
        QString FgColor=xdg.value("FgColor",mparentFColor).toString();
        bool Separator=xdg.value("Separator",false).toBool();
        QString Command=xdg.value("Command").toString();
        int Interval=xdg.value("Interval",0).toInt()*1000;


        //adding separator if prev statu separator is truee
        if(sep){
            StatusLabel *sepLab=new StatusLabel(QColor(BgColor),QColor(bgPrevColor),this,STyle);
            mHorizontalLayout->addWidget(sepLab);
             listStatus.append(sepLab);
        }

        sep=Separator;
        bgPrevColor=BgColor;

        if(Interval==0)Interval=(60000*60)*24;//one day

        if(Command.startsWith("$HOME") || Command.startsWith("~/")){
            Command.remove("$HOME");
            Command.remove("~");
            Command.insert(0,QDir::homePath());

        }

        if(STyle<1||STyle>5)
            BgColor=mparentColor;

        //add the new statu
        StatusLabel *sLabel=new StatusLabel(QColor(BgColor),QColor(FgColor),this,Label);
        sLabel->setName(sName);
        sLabel->setCommand(Command,Interval);
        mHorizontalLayout->addWidget(sLabel);
        listStatus.append(sLabel);
    }

    adjustSize();
}

