QT += quick multimedia
CONFIG += c++11

DESTDIR = ../bin/win64
INCLUDEPATH += ./../include
LIBS += -L./../lib/win64
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

