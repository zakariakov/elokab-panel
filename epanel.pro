#-------------------------------------------------
#
# Project created by QtCreator 2017-03-04T09:15:20
#
#-------------------------------------------------

QT       += core gui  x11extras concurrent  xml dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = epanel
TEMPLATE = app

LIBS += $(SUBLIBS)  -L/usr/lib -lX11 -xrender -lXcomposite -lXdamage

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = epanel
DESTDIR = usr/bin
CONFIG += qt \
          release
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
INCLUDEPATH +=build

SOURCES += main.cpp\
        panelwidget.cpp \
        panel_adaptor.cpp \
        status/statuslabel.cpp \
        status/statuswidget.cpp \
        etaskbar/dtaskbarwidget.cpp \
        etaskbar/dactiontaskbar.cpp \
        utils/x11utills.cpp \
        utils/edir.cpp \
        utils/desktopfile.cpp \
        dsystray/systray.cpp \
        dsystray/trayicon.cpp \
        utils/xdesktoputils.cpp \
        dmenubar/menuapplications.cpp \
        dmenubar/menufolders.cpp \
        dmenubar/menuprogrammes.cpp \
        dmenubar/menugen.cpp \
        dmenubar/menusystem.cpp \
        dmenubar/findbutton.cpp \
        dmenubar/power/powerdlg.cpp \
        dmenubar/power/powermain.cpp \
        dmenubar/menurecent.cpp \
epager/pager.cpp \
    dmenubar/openexec.cpp \
    windowdock.cpp \
    dmenubar/menuform.cpp \
    dmenubar/menuclassic.cpp

HEADERS  += panelwidget.h \
            panel_adaptor.h \
            status/statuslabel.h \
            status/statuswidget.h \
            utils/desktopfile.h \
            utils/x11utills.h \
            utils/edir.h \
            utils/mystyle.h \
            utils/xdesktoputils.h \
            etaskbar/dactiontaskbar.h \
            etaskbar/dtaskbarwidget.h \
            dsystray/systray.h \
            dsystray/trayicon.h \
            dmenubar/menuapplications.h \
            dmenubar/menufolders.h \
            dmenubar/menuprogrammes.h \
            dmenubar/menugen.h \
            dmenubar/menusystem.h \
            dmenubar/power/powerdlg.h \
            dmenubar/power/powermain.h \
            dmenubar/menurecent.h \
            dmenubar/findbutton.h \
            epager/pager.h \
    dmenubar/openexec.h \
    windowdock.h \
    dmenubar/menuform.h \
    dmenubar/menuclassic.h


FORMS    += panelwidget.ui \
    dmenubar/menuform.ui

DISTFILES += \
    etc/xdg/menus/elokab-applications.menu \
    usr/share/desktop-directories/elokab-audio-video.directory \
    usr/share/desktop-directories/elokab-development.directory \
    usr/share/desktop-directories/elokab-education.directory \
    usr/share/desktop-directories/elokab-game.directory \
    usr/share/desktop-directories/elokab-graphics.directory \
    usr/share/desktop-directories/elokab-home.directory \
    usr/share/desktop-directories/elokab-islamic.directory \
    usr/share/desktop-directories/elokab-menu-applications.directory \
    usr/share/desktop-directories/elokab-network.directory \
    usr/share/desktop-directories/elokab-office.directory \
    usr/share/desktop-directories/elokab-other.directory \
    usr/share/desktop-directories/elokab-recent.directory \
    usr/share/desktop-directories/elokab-settings.directory \
    usr/share/desktop-directories/elokab-system-tools.directory \
    usr/share/desktop-directories/elokab-system.directory \
    usr/share/desktop-directories/elokab-utility.directory \
    usr/share/elokab/elokab-syspower/Hibernate.desktop \
    usr/share/elokab/elokab-syspower/Lock.desktop \
    usr/share/elokab/elokab-syspower/Logout.desktop \
    usr/share/elokab/elokab-syspower/Message.desktop \
    usr/share/elokab/elokab-syspower/Reboot.desktop \
    usr/share/elokab/elokab-syspower/Shutdown.desktop \
    usr/share/elokab/elokab-syspower/Suspend.desktop \
    usr/share/elokab/elokab-menu/elokab-recent.desktop

#---------------------------------------------
#                   TRANSLATIONS
#---------------------------------------------
TRANSLATIONS    =usr/share/elokab/translations/ar/epanel.ts\
                 usr/share/elokab/translations/fr/epanel-fm.ts\
                 usr/share/elokab/translations/en/epanel-fm.ts\
#---------------------------------------------
#                   INSTALL
#---------------------------------------------
 MKDIR = mkdir -p /etc/xdg/elokab
 MKDIR = mkdir -p /etc/xdg/menus
 MKDIR = mkdir -p /usr/share/desktop-directories
 MKDIR = mkdir -p /usr/share/elokab

        target.path = /usr/bin

        data.files =usr/share/elokab/*
        data.path=/usr/share/elokab/

        dataDirectories.files =usr/share/desktop-directories/*
        dataDirectories.path =usr/share/desktop-directories/

        xdgMenu.files=etc/xdg/menus/elokab-applications.menu
        xdgMenu.path=etc/xdg/menus/

 INSTALLS +=target \
            data \
            dataDirectories \
            xdgMenu
