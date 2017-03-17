#include "openexec.h"
#include "utils/desktopfile.h"
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QSettings>
OpenExec::OpenExec(QObject *parent) : QObject(parent)
{

}
void OpenExec::execFile(const QString &fileName)
{

    QProcess process;
    process.setWorkingDirectory(QDir::homePath());
    QDir::setCurrent(QDir::homePath());
//dir.setCurrent(QDir::homePath());
    //------------------------------------------------------------non desktop
    if(!fileName.endsWith(".desktop")){
        qDebug()<<fileName;
        process.startDetached(fileName.trimmed()/*,QStringList(),QDir::homePath()*/);
        return;
    }

    //--------------------------------------------------------------desktop
    DesktopFile ds(fileName,"Desktop Entry");

    QString exec=ds.value("Exec").toString();

    QRegExp ex("\\%[A-Z]");
    QRegExp ex2("\\%[a-z]");
    exec.remove(ex);
    exec.remove(ex2);
    exec.remove("-caption");

    bool terminal=ds.value("Terminal",false).toBool();

    QString term=defaultTerminal();
    if(terminal)
        process.startDetached(QString("%1 -e %2").arg(term).arg(exec.trimmed()));//Todo remplace xfce4-terminal
    else
        process.startDetached(exec.trimmed());


}

//############################# get prefired terminam ###########################
QString OpenExec::defaultTerminal()
{
    QByteArray sS=qgetenv("DESKTOP_SESSION");
    qDebug()<<"envirenment"<<sS;
    QString terminal;
    if(sS=="elokab-session"){
        QSettings setting("elokab","elokabsettings");
        setting.beginGroup("Terminal");
        terminal=  setting.value("Default","xterm").toString();
        setting.endGroup();
        return terminal;
    }

    //search in enverenment
    if(sS=="xfce")
        terminal="xfce4-terminal";
    else if(sS=="Enlightenment"||sS.contains("enlightenment"))
        terminal="terminology";
    else if(sS==" plasma-wayland-session"||sS==" plasma-session"||sS.contains("plasma"))
        terminal="konsole";
    else if(sS=="gnome-session"||sS.contains("gnome"))
        terminal="gnome-terminal";
    else if(sS=="cinnamon-session")
        terminal="gnome-terminal";
    else if(sS.contains("deepin"))
        terminal="gnome-terminal";
    else if(sS==("lxsession"))
        terminal="lxterminal";
    //search list of terminal

    if(terminal.isEmpty()){
        //TODO remplace this list
        QStringList list;
        list<<"elokab-terminal"<<"gnome-terminal"<<"konsole"<<"deepin-terminal"<<"terminology"<<"xfce4-terminal"
           << "lxterminal"<<"qterminal"<<"mate-terminal"<<"pantheon-terminal"<<"terminator"<<"theterminal"<<"aterm"
           <<"eterm"<<"mlterm"<<" tilda";

        QStringList dirs = QString(getenv("PATH")).split(":");

        foreach(QString dir,dirs)
        {
            foreach (QString term, list) {
                if(QFile::exists(dir+"/"+term)){
                    terminal=term;
                    qDebug()<<"terminal exist"<<dir+"/"+term;
                    break;
                }
            }
            if(!terminal.isEmpty())break;
        }
    }
    if(terminal.isEmpty())
        terminal="xtem";

    qDebug()<<"defaultTerminal"<<terminal;
    return terminal;

}

