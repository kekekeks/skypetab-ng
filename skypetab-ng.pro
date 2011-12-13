#-------------------------------------------------
#
# Project created by QtCreator 2011-12-07T20:20:31
#
#-------------------------------------------------

TARGET = skypetab-ng
TEMPLATE = lib
VERSION = 0.4.1

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
    aboutdialog.cpp

HEADERS +=\
    mainwindow.h \
    stwindowcontainer.h \
    x11.h \
    skypetab.h \
    stwatcher.h \
    contactlistcontainer.h \
    focusguard.h \
    aboutdialog.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE93225CF
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = skypetab-ng.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

FORMS += \
    aboutdialog.ui
