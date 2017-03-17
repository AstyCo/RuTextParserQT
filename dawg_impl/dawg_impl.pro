#-------------------------------------------------
#
# Project created by QtCreator 2017-03-12T12:38:43
#
#-------------------------------------------------

QT       -= gui

TARGET = dawg_impl
TEMPLATE = lib

DEFINES += DAWG_IMPL_LIBRARY

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

SOURCES +=

HEADERS +=\
        dawg_impl_global.h \
    dawgimpl.h \
    nodedawg.h \
    dawgdic/base-types.h \
    dawgdic/base-unit.h \
    dawgdic/bit-pool.h \
    dawgdic/completer.h \
    dawgdic/dawg-builder.h \
    dawgdic/dawg-unit.h \
    dawgdic/dawg.h \
    dawgdic/dictionary-builder.h \
    dawgdic/dictionary-extra-unit.h \
    dawgdic/dictionary-unit.h \
    dawgdic/dictionary.h \
    dawgdic/guide-builder.h \
    dawgdic/guide-unit.h \
    dawgdic/guide.h \
    dawgdic/link-table.h \
    dawgdic/object-pool.h \
    dawgdic/ranked-completer-candidate.h \
    dawgdic/ranked-completer-node.h \
    dawgdic/ranked-completer.h \
    dawgdic/ranked-guide-builder.h \
    dawgdic/ranked-guide-link.h \
    dawgdic/ranked-guide-unit.h \
    dawgdic/ranked-guide.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
