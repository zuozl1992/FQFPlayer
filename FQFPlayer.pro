QT += multimedia core gui widgets sql
CONFIG += c++11
TEMPLATE = app

DESTDIR = ../bin/win32

include(fqfmedia/fqfmedia.pri)
include(qxtglobalshortcut/qxtglobalshortcut.pri)
include(component/component.pri)
include(lrcreader/lrcreader.pri)

FORMS += \
    mainpage.ui \
    optionpage.ui

HEADERS += \
    mainpage.h \
    optionpage.h

SOURCES += \
    main.cpp \
    mainpage.cpp \
    optionpage.cpp

RESOURCES += \
    images.qrc

RC_FILE += \
    version.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
