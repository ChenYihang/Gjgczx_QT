#-------------------------------------------------
#
# Project created by QtCreator 2018-11-01T14:37:03
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += xml
QT       += script
QT       += webchannel
QT       += webenginewidgets


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyMQTT
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mymqtt.cpp \
    navlistview.cpp \
    httpapi.cpp \
    infoupdate.cpp \
    jsonsolve.cpp \
    subscription.cpp \
    jsdocument.cpp \
    datareceive.cpp \
    filedeal.cpp

HEADERS += \
    mymqtt.h \
    navlistview.h \
    httpapi.h \
    infoupdate.h \
    common.h \
    jsonsolve.h \
    subscription.h \
    jsdocument.h \
    multifunc.h \
    datareceive.h \
    filedeal.h

FORMS += \
        mymqtt.ui \
    subscription.ui \
    datareceive.ui

DISTFILES +=

RESOURCES += \
    rc.qrc





win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lQt5Mqtt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lQt5Mqttd

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libQt5Mqtt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libQt5Mqttd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/Qt5Mqtt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/Qt5Mqttd.lib
