TEMPLATE = vclib

HEADERS += SkewtQtPlot.h
HEADERS += SkewTAdapterQt.h

SOURCES += SkewtQtPlot.cc
SOURCES += SkewTAdapterQt.cc

DEFINES +=

CONFIG += debug
CONFIG += qt 
CONFIG += warn_on 
CONFIG += staticlib
config += exceptions

DESTDIR = ./Debug

INCLUDEPATH += ../
INCLUDEPATH += ../.. 
INCLUDEPATH += c:/Projects/include 
INCLUDEPATH += c:/Projects
INCLUDEPATH += c:/Projects/AvapsEd
