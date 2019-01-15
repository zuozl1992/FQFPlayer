QT += quick multimedia
CONFIG += c++11

INCLUDEPATH += ./../include
win32{
    #区分32位和64位windows程序
    opt = $$find(QMAKESPEC,"msvc2017_64")
    isEmpty(opt){
        DESTDIR = ../bin/win32
        LIBS += -L./../lib/win32
    }
    !isEmpty(opt){
        DESTDIR = ../bin/win64
        LIBS += -L./../lib/win64
    }
}
LIBS += -lFQFLib

SOURCES += \
        main.cpp \
	colorquickimageprovider.cpp \
	qmlfqfvideodevice.cpp \
	qtfqfaudiodevice.cpp \
	videocontrol.cpp

HEADERS += \
	colorquickimageprovider.h \
	qmlfqfvideodevice.h \
	qtfqfaudiodevice.h \
	videocontrol.h


RESOURCES += qml.qrc \
	images.qrc

