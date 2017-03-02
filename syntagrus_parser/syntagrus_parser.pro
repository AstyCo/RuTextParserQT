#-------------------------------------------------
#
# Project created by QtCreator 2017-03-02T23:37:13
#
#-------------------------------------------------

QT -= gui
QT += xml

TARGET = syntagrus_parser
TEMPLATE = lib

DEFINES += SYNTAGRUS_PARSER_LIBRARY

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
include(../cnfgrammar_include.pri)


SOURCES += syntagrusparser.cpp

HEADERS += syntagrusparser.h\
        syntagrus_parser_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
