TEMPLATE = vcapp

HEADERS += SkewtQtTest.h

SOURCES += test.cc
SOURCES += SkewtQtTest.cc

DEFINES +=

CONFIG += debug
CONFIG += qt 
CONFIG += warn_on 
CONFIG += staticlib
config += exceptions

INCLUDEPATH += ../
INCLUDEPATH += ../..
INCLUDEPATH += ../../../
INCLUDEPATH += c:/Projects/log4cpp-0.3.4b/include

LIBS += ../Debug/skewtqt.lib
