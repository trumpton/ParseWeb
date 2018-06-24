#-------------------------------------------------
#
# Project created by QtCreator 2015-12-13T14:21:04
#
#-------------------------------------------------

QT += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ParseWeb
TEMPLATE = app

win32:RC_FILE=icons.rc

SOURCES += main.cpp\
    mainwindow.cpp \
    decodehtml.cpp \
    fileio.cpp \
    ../Lib/safetextedit.cpp \
    ../Lib/safelineedit.cpp \
    ../Lib/lineeditnavigator.cpp \
    ../Lib/alertsound.cpp \
    ../Lib/iniconfig.cpp \
    ../Lib/supportfunctions.cpp \
    ../Lib/warningform.cpp \
    ../Lib/warningok.cpp \
    ../Lib/warningyesno.cpp

HEADERS += mainwindow.h \
    initypes.h \
    decodehtml.h \
    fileio.h \
    icons.rc \
    icons.h \
    ../Lib/alertsound.h \
    ../Lib/iniconfig.h \
    ../Lib/safetextedit.h \
    ../Lib/safelineedit.h \
    ../Lib/lineeditnavigator.h \
    ../Lib/supportfunctions.h \
    ../Lib/warningform.h \
    ../Lib/warningok.h \
    ../Lib/warningyesno.h \
    buildinfo.h \
    version.h

FORMS += mainwindow.ui \
    ../Lib/warningform.ui \
    ../Lib/warningok.ui \
    ../Lib/warningyesno.ui

RESOURCES += \
    icons.qrc \
    ../Lib/sounds.qrc

OTHER_FILES += \
    icons.rc

