DESTDIR = $$OUT_PWD/../bin

# set dependency on headers
CONFIG += depend_includepath
unix: CONFIG += debug_and_release debug_and_release_target

# looking for libs in DESTDIR
LIBS += -L$$DESTDIR
