#-------------------------------------------------
#
# Project created by QtCreator 2018-12-27T11:54:56
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += multimedia
QT      += charts


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MultiChannelCollection
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        multichannel.cpp \
    cdataprocess.cpp \
    cfilehandle.cpp \
    csocketclient.cpp \
    csocketprocess.cpp \
    csocketserver.cpp \
    timechartplot.cpp \
    updatecharts.cpp

HEADERS += \
        multichannel.h \
    cdataprocess.h \
    cfilehandle.h \
    csocketclient.h \
    csocketprocess.h \
    csocketserver.h \
    common.h \
    multifunc.h \
    timechartplot.h \
    updatecharts.h

FORMS += \
        multichannel.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/WpdPack/Lib/ -lwpcap -lPacket
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/WpdPack/Lib/ -lwpcap -lPacket

INCLUDEPATH += $$PWD/WpdPack/Include
DEPENDPATH += $$PWD/WpdPack/Include
