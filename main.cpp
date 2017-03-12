#include "panelwidget.h"

#include <QApplication>
#include "panel_adaptor.h"
#include <QAbstractNativeEventFilter>
#include <QDBusConnection>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("elokab");
    a.setApplicationName("epanel");
    a.setApplicationDisplayName("Elokab-Panel");


    QDBusConnection connection = QDBusConnection::sessionBus();
    if (! QDBusConnection::sessionBus().registerService("org.elokab.panel"))
    {
        qWarning() << "Unable to register 'org.elokab.panel' service - is another instance of elokab-panel running?";

        QDBusInterface dbus("org.elokab.panel",
                            "/org/elokab/panel",
                            "org.elokab.panel.Interface");

        if (!dbus.isValid()) {
            qDebug() << "QDBusInterface is not valid!";
            return 1;
        }

        //--------------------------------------------------------------------------
        QStringList args = a.arguments();
          if(args.count()>1)
          {

              QString help=QObject::tr
                      ("Usage: epanel [OPTION]\n"
                       " \n"
                       "OPTION:\n"
                       " -r  reconfigure       reconfigure settings and layout.\n"
                       " -m  menu              show menu applications\n"
                       );

              QString arg = args.at(1);

              QDBusMessage    msg;

               if (arg == "-r"|| arg == "reconfigure" )    {msg = dbus.call("reconfigure");}
              else if (arg == "-m" ||arg == "menu" ) {msg = dbus.call("showMenu");}
              else { qWarning() << "Unknown option: " << args;  qDebug()<<help; return 0;}

              QString response = msg.arguments().first().toString();
              qDebug() << "dbus:reconfigure"  << "=" << response;
               return 1;
        }
        //--------------------------------------------------------------------------
        //           QList<QVariant> args;
        //          args.append("Elokab Notification");       // Application Name
        //           dbus.callWithArgumentList(QDBus::NoBlock, "reconfigure", args);


        return 1;

    }


    PanelWidget w;

    new panel_adaptor(&w);

    connection.registerObject("/org/elokab/panel", &w);

    w.show();

    return a.exec();

}
