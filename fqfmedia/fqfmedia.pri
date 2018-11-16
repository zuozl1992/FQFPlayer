INCLUDEPATH += $$PWD

INCLUDEPATH += ./../include
LIBS += -L./../lib/win32 -lavformat -lavcodec -lavutil -lswresample

HEADERS += \
    $$PWD/fqfaudiothread.h \
    $$PWD/fqfdecode.h \
    $$PWD/fqfdecodethread.h \
    $$PWD/fqfdemux.h \
    $$PWD/fqfdemuxthread.h \
    $$PWD/fqfresample.h \
    $$PWD/fqfaudiodevice.h \
    $$PWD/qtfqfaudiodevice.h

SOURCES += \
    $$PWD/fqfaudiothread.cpp \
    $$PWD/fqfdecode.cpp \
    $$PWD/fqfdecodethread.cpp \
    $$PWD/fqfdemux.cpp \
    $$PWD/fqfdemuxthread.cpp \
    $$PWD/fqfresample.cpp \
    $$PWD/fqfaudiodevice.cpp \
    $$PWD/qtfqfaudiodevice.cpp
