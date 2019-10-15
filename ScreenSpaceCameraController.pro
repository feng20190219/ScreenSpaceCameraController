#-------------------------------------------------
#
# Project created by QtCreator 2018-12-21T14:55:28
#
#-------------------------------------------------

QT += widgets quickwidgets

TEMPLATE = lib
TARGET = ScreenSpaceCameraController

win32:CONFIG(release, debug|release): {
       DESTDIR = $$PWD/../x64/release
} else:win32:CONFIG(debug, debug|release): {
       DESTDIR = $$PWD/../x64/debug
}

DEFINES += SCREENSPACECAMERACONTROLLER_LIBRARY

win32:CONFIG(release, debug|release) : {
    LIBS += -L$$PWD/../x64/release/ -llicore
} else : win32:CONFIG(debug, debug|release) : {
    LIBS += -L$$PWD/../x64/debug/ -llicored
}

DEFINES += QT_DEPRECATED_WARNINGS


INCLUDEPATH += $$PWD/../licore/include

SOURCES += \
        screenspacecameracontroller.cpp \
        cameraeventaggregator.cpp \
        screenspaceeventhandler.cpp \
        intersectiontests.cpp \
        cameracontroller.cpp \
        quadraticrealpolynomial.cpp \
        cubicrealpolynomial.cpp \
        quarticrealpolynomial.cpp \
        tweencollection.cpp \
        tweenjs.cpp \
        cameraflightpath.cpp \
        cesiummath.cpp \
        cesiumcartesian3.cpp \
        ellipsoidgeodesic.cpp

HEADERS += \
        screenspacecameracontroller.h \
        sscc_global.h \
        cameraeventaggregator.h \
        screenspaceeventhandler.h \
        intersectiontests.h \
        cameracontroller.h \
        quadraticrealpolynomial.h \
        cubicrealpolynomial.h \
        quarticrealpolynomial.h \
        screenspaceeventutils.h \
        tweencollection.h \
        tweenjs.h \
        cameraflightpath.h \
        cesiummath.h \
        cesiumcartesian3.h \
        ellipsoidgeodesic.h
