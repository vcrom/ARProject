#-------------------------------------------------
#
# Project created by QtCreator 2014-06-13T20:00:19
#
#-------------------------------------------------

#QT       += core
#QT       -= gui

CONFIG   += console
CONFIG   -= app_bundle
CONFIG  += qt warn_on #release


TARGET = ARProject

TEMPLATE = app

LIBS += -L/usr/X11R6/lib \
        -L/usr/local/lib \
        -L$$_PRO_FILE_PWD_/lib \
        -lARgsub \
        -lARvideo \
        -lAR \
        -lpthread \
        -lglut \
        -lGLU \
        -lGL \
        -lXi \
        -lX11 \
        -lm \
        -pthread \
        -lgstreamer-0.10 \
        -lgobject-2.0 \
        -lgmodule-2.0 \
        -lgthread-2.0 \
        -lxml2 \
        -lglib-2.0



INCLUDEPATH += "$$_PRO_FILE_PWD_/include"

LIBRARY_PATH += "$$_PRO_FILE_PWD_/lib"

SOURCES += main.cpp

QMAKE_POST_LINK +=  mkdir -p Data &
QMAKE_POST_LINK +=  $$QMAKE_COPY $$_PRO_FILE_PWD_/Data/* Data &
