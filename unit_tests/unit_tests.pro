#-------------------------------------------------
#
# Project created by QtCreator 2017-03-03T11:41:03
#
#-------------------------------------------------

QT       += testlib xml network

QT       -= gui

TARGET = tst_syntagrusparsertest
CONFIG   += console
CONFIG   -= app_bundle

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

include(../link_projects_include.pri)
include(../rutextparser_extensions_include.pri)
include(../syntagrus_parser_include.pri)
include(../cyk_syntactical_analyzer_include.pri)
include(../dawg_impl_include.pri)
include(../morph_client_include.pri)




SOURCES += \
    tst_all.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    tst_all.h
