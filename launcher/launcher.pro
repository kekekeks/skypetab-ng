#-------------------------------------------------
#
# Project created by QtCreator 2012-11-01T15:54:46
#
#-------------------------------------------------



LIBS+= -lX11

TARGET = skypetab-prelaunch
TEMPLATE = app

CONFIG -= qt
SOURCES += \
    main.cpp

HEADERS  +=

unix:
{
   target.path = /usr/bin
   INSTALLS += target
}
