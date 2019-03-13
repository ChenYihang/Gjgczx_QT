#-------------------------------------------------
#
# Project created by QtCreator 2018-05-27T15:41:51
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MultiChannelCollectionOnline
TEMPLATE = app

RC_ICONS =icon/title.ico

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    SubInterface/colorcard.cpp \
    SubInterface/comboboxt.cpp \
    SubInterface/namelineeditt.cpp \
    SubInterface/offlineinterface.cpp \
    SubInterface/onlineinterface.cpp \
    SubInterface/rmsgroup.cpp \
    multichannelcollectiononline.cpp \
    ../Common/common.cpp

HEADERS += \
    FFTW/fftw3.h \
    SubInterface/colorcard.h \
    SubInterface/comboboxt.h \
    SubInterface/interfacecommon.h \
    SubInterface/namelineeditt.h \
    SubInterface/offlineinterface.h \
    SubInterface/onlineinterface.h \
    SubInterface/rmsgroup.h \
    multichannelcollectiononline.h \
    ../Common/common.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/FFTW/lib/ -llibfftw3-3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/FFTW/lib/ -llibfftw3-3

INCLUDEPATH += $$PWD/FFTW
DEPENDPATH += $$PWD/FFTW
