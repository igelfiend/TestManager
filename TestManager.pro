#-------------------------------------------------
#
# Project created by QtCreator 2017-03-13T09:24:25
#
#-------------------------------------------------

QT       += core gui xml
CONFIG += C++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestManager
TEMPLATE = app

target.path = C:/work/VNAPT_EXPERIMENT/TestManager/release
INSTALLS += target

INCLUDEPATH += gui\
               objects
SOURCES += main.cpp\
    utils.cpp \
    Gui/configlist.cpp \
    Gui/editparamform.cpp \
    Gui/mainwindow.cpp \
    Gui/paramlistitem.cpp \
    Objects/config.cpp \
    Objects/group.cpp \
    Objects/manager.cpp \
    Objects/param.cpp \
    Objects/test.cpp \
    Objects/test_info.cpp

HEADERS  += utils.h \
    Gui/configlist.h \
    Gui/editparamform.h \
    Gui/mainwindow.h \
    Gui/paramlistitem.h \
    Objects/config.h \
    Objects/group.h \
    Objects/manager.h \
    Objects/param.h \
    Objects/test.h \
    Objects/test_info.h

FORMS    += Gui/mainwindow.ui \
    Gui/editparamform.ui