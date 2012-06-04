################################################################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
################################################################

TARGET   = oscilloscope

#xenomai has a variable called signals.
CONFIG += no_keywords


INCLUDEDIR   = $$(HOME)/.xenomailab/include/
DESTDIR      = $${PWD}/../
OBJECTS_DIR = build/.obj
MOC_DIR = build/.moc
RCC_DIR = build/.rcc
UI_DIR = build/.ui
CLEAN_FILES += ${TARGET}

HEADERS = \
    signaldata.h \
    plot.h \
    knob.h \
    wheelbox.h \
    samplingthread.h \
    curvedata.h \
    mainwindow.h \
    opsettings.h \
    $${INCLUDEDIR}mtrx.h\
    blockio.h \
    inputmap.h

SOURCES = \
    signaldata.cpp \
    plot.cpp \
    knob.cpp \
    wheelbox.cpp \
    samplingthread.cpp \
    curvedata.cpp \
    mainwindow.cpp \
    main.cpp \
    opsettings.cpp \
    $${INCLUDEDIR}mtrx.c\
    blockio.cpp \
    inputmap.cpp

#Qwt libs
LIBS += -L/usr/local/qwt-6.0.1/lib -lqwt
INCLUDEPATH += /usr/local/qwt-6.0.1/include

INCLUDEPATH += $$INCLUDEDIR /usr/include/xenomai
LIBS += -L/usr/lib -lxenomai -lpthread  -lnative -Xlinker -rpath -Xlinker

QMAKE_CXXFLAGS += -D_GNU_SOURCE -D_REENTRANT -Wall -pipe -D__XENO__
