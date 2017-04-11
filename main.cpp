#include "panelwidget.h"

#include "utils/edir.h"
#include <QApplication>
#include "panel_adaptor.h"
//#include <QAbstractNativeEventFilter>
#include <QDBusConnection>


QString help=QObject::tr
        ("Usage: epanel [OPTION]\n"

         "OPTION:\n"
         " -c  reconfigure       reconfigure settings and layout.\n"
         " -m  menu              show menu applications\n"
         " -x  exit              exit if panel is runing \n"
         " -r  restart           restart panel  \n"
         );

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("elokab");
    a.setApplicationName("epanel");
    a.setApplicationDisplayName("Elokab-Panel");


    QStringList args = a.arguments();
    if(args.count()>1 && args.at(1) == "-h"){qWarning() <<help; return 0;}

    QDBusConnection connection = QDBusConnection::sessionBus();
    if (! QDBusConnection::sessionBus().registerService("org.elokab.panel"))
    {
        qWarning() << "Unable to register 'org.elokab.panel' service - is another instance of elokab-panel running?";

        if(args.count()>1)
        {
            QDBusInterface dbus("org.elokab.panel",
                                "/org/elokab/panel",
                                "org.elokab.panel.Interface");

            if (!dbus.isValid()) {
                qDebug() << "QDBusInterface is not valid!";
                return 0;
            }



            QString arg = args.at(1);

            QDBusMessage    msg;

            if (arg == "-c"|| arg == "reconfigure" )
            {msg = dbus.call("reconfigure");return 0;}

            else if (arg == "-m" ||arg == "menu" )
            {msg = dbus.call("showMenu");return 0;}

            else if (arg == "-x" ||arg == "exit" )
                      {msg = dbus.call("exit");return 0;}

            else if (arg == "-r"||arg == "restart")
            {
                msg = dbus.call("exit");
                a.processEvents();
                QDBusConnection::sessionBus().registerService("org.elokab.panel");
            }

            else { qWarning() << "Unknown option: " << args;  qDebug()<<help; return 0;}

            QString response = msg.arguments().first().toString();
            qDebug() << "dbus:reconfigure"  << "=" << response;

//            if (arg != "-r" && arg != "restart" )
//            return 0;
        }else{

           return 0;
        }

    }

    //---------------------------TRANSLATION------------------------------------
    /// مسار مجلد البرنامج
    QString translatPath= Edir::dataDir()+"/translations";
    /// جلب اعدادات اللغة
    QByteArray sS=qgetenv("DESKTOP_SESSION");
    QString locale;
    if(sS=="elokab-session"){
        QSettings globalSetting("elokab","elokabsettings");
        globalSetting.beginGroup("Language");
         locale=globalSetting.value("Name","Default").toString();
        globalSetting.endGroup();
        /// اذاكانت اللغة الافتراضية
        if(locale=="Default")
            locale = QLocale::system().name().section("_",0,0);
    }else{

         locale = QLocale::system().name().section("_",0,0);
    }

    /// اللغة الحالية لجميع البرنامج
    QLocale::setDefault(QLocale(locale));
    /// جلب ترجمات كيوتي
    QString translatorFileName = QLatin1String("qt_");
    translatorFileName += locale;
    QTranslator *translatorsys = new QTranslator;
    if (translatorsys->load(translatorFileName, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        QApplication::installTranslator(translatorsys);
    /// جلب ترجمة البرنامج من مجلد البرنامج
    QTranslator translator;
    translator.load(QString(translatPath+"/"+locale+"/"+a.applicationName()));

    a.installTranslator(&translator);

    QLocale lx=QLocale(locale);

  a.setLayoutDirection(lx.textDirection());

    //-------------------------------------------------------------------------------end translation

//    if(locale=="ar"){
//        qApp->setLayoutDirection(Qt::RightToLeft);
//        w.setLayoutDirection(Qt::RightToLeft);
//qDebug ()<<"local"<<locale;
//    }else
//qApp->setLayoutDirection(Qt::LeftToRight);

  PanelWidget w;
new panel_adaptor(&w);
// w.setLayoutDirection(lx.textDirection());
    connection.registerObject("/org/elokab/panel", &w);

   w.show();

    return a.exec();

}
