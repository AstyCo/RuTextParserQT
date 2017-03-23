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
include(../rutextparser_extensions_include.pri)


SOURCES += \
    treeparser.cpp \
    corpora_parsing/recordincorpora.cpp \
    corpora_parsing/recordnode.cpp \
    corpora_parsing/sentenceincorpora.cpp \
    grammar/cnfgrammar.cpp \
    corpora_parsing/syntagrusparser.cpp \
    corpora_parsing/wordincorpora.cpp \
    corpora_parsing/sentenceinfo.cpp \
    corpora_parsing/treecorpora.cpp \
    corpora_parsing/optimizedtreecorpora.cpp \
    corpora_parsing/optimizedsentence.cpp \
    corpora_parsing/optimizedword.cpp \
    grammar/scoredrules.cpp \
    grammar/simpleruletree.cpp

HEADERS +=\
        syntagrus_parser_global.h \
    treeparser.h \
    corpora_parsing/recordincorpora.h \
    corpora_parsing/recordnode.h \
    corpora_parsing/sentenceincorpora.h \
    corpora_parsing/treecorpora.h \
    grammar/cnfgrammar.h \
    corpora_parsing/syntagrusparser.h \
    corpora_parsing/wordincorpora.h \
    corpora_parsing/sentenceinfo.h \
    corpora_parsing/optimizedtreecorpora.h \
    corpora_parsing/optimizedsentence.h \
    corpora_parsing/optimizedword.h \
    internal/base-types.h \
    grammar/chomskyrulerecord.h \
    grammar/rulerecord.h \
    grammar/scoredrules.h \
    corpora_parsing/uniquevector.h \
    grammar/simpleruletree.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
