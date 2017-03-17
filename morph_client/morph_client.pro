#-------------------------------------------------
#
# Project created by QtCreator 2017-03-16T23:50:45
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = morph_client
TEMPLATE = lib

DEFINES += MORPH_CLIENT_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../link_projects_include.pri)
include(../rutextparser_extensions_include.pri)

SOURCES += morphclient.cpp

HEADERS += morphclient.h\
        morph_client_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
