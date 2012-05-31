#-------------------------------------------------
#
# Project created by QtCreator 2011-12-07T20:20:31
#
#-------------------------------------------------

TARGET = skypetab-ng
TEMPLATE = lib
VERSION = 0.4.11

DEFINES += SKYPETABNG_LIBRARY
QMAKE_CXXFLAGS += -std=c++0x
SOURCES += \
    mainwindow.cpp \
    x11int.cpp \
    stwindowcontainer.cpp \
    x11.cpp \
    skypetab.cpp \
    stwatcher.cpp \
    contactlistcontainer.cpp \
    focusguard.cpp \
    aboutdialog.cpp \
    qtintercept.cpp \
    settingsdialog.cpp

HEADERS +=\
    mainwindow.h \
    stwindowcontainer.h \
    x11.h \
    skypetab.h \
    stwatcher.h \
    contactlistcontainer.h \
    focusguard.h \
    aboutdialog.h \
    qintercept.h \
    settingsdialog.h

system("chmod +x skypetab-ng")

unix:
{
   target.path = /usr/lib

   desktop.path = /usr/share/applications
   desktop.files = skypetab-ng.desktop
   launcher.path = /usr/bin
   launcher.files = skypetab-ng


   INSTALLS += target
   INSTALLS += desktop
   INSTALLS += launcher
}
FORMS += \
    aboutdialog.ui \
    settingsdialog.ui
