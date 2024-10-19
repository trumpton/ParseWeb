#-------------------------------------------------
#
# Project created by QtCreator 2015-12-13T14:21:04
#
#-------------------------------------------------

QT += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET = ParseWeb
TEMPLATE = app


# Start Common Versioning Includes

unix:BUILDDATE = $$system(date +"%Y-%d-%m %H:%M:%S")
win32:BUILDDATE = $$system(echo "%date:~6,4%-%date:~3,2%-%date:~0,2% %time:~0,8%")

GITHASH = $$system(git --git-dir=\"$$PWD/.git\" describe --always --tags)
GITHASH = $$replace(GITHASH,"v","")
GITHASH = $$replace(GITHASH,"-",".")
LIBHASH = $$system(git --git-dir=\"$$PWD/../Lib/.git\" describe --always --tags)
LIBHASH = $$replace(LIBHASH,"v","")
LIBHASH = $$replace(LIBHASH,"-",".")

GITHASHPARTS = $$split(GITHASH, .)
V1 = $$member(GITHASHPARTS,0)
V2 = $$member(GITHASHPARTS,1)
V3 = $$member(GITHASHPARTS,2)
VERSION = "$${V1}.$${V2}.$${V3}"

system("echo VERSION=\"$$VERSION\" > $$OUT_PWD/version.txt")
system("echo GITHASH=\"$$GITHASH\" >> $$OUT_PWD/version.txt")
system("echo LIBHASH=\"$$LIBHASH\" >> $$OUT_PWD/version.txt")
system("echo BUILDDATE=\"$$BUILDDATE\" >> $$OUT_PWD/version.txt")

DEFINES += GITHASH='"\\\"$$GITHASH\\\""'
DEFINES += LIBHASH='"\\\"$$LIBHASH\\\""'
DEFINES += BUILDDATE='"\\\"$$BUILDDATE\\\""'
DEFINES += PWD='"\\\"$$PWD\\\""'

# End Common Versioning Includes


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
    ../Lib/warningyesno.h

FORMS += mainwindow.ui \
    ../Lib/warningform.ui \
    ../Lib/warningok.ui \
    ../Lib/warningyesno.ui

RESOURCES += \
    icons.qrc \
    ../Lib/sounds.qrc

OTHER_FILES += \
    icons.rc

