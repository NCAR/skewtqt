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
INCLUDEPATH += c:/Projects/log4cpp-0.3.4b/include
INCLUDEPATH += c:/Projects
INCLUDEPATH += c:/Projects/boost_1_34_0

