TEMPLATE = vcapp

HEADERS += SkewtQtTest.h

SOURCES += test.cc
SOURCES += SkewtQtTest.cc

DEFINES +=

CONFIG += build_all
CONFIG += qt 
CONFIG += warn_on 
CONFIG += staticlib
CONFIG += exceptions

INCLUDEPATH += ../
INCLUDEPATH += ../..
INCLUDEPATH += ../../../
INCLUDEPATH += c:/Projects/log4cpp-0.3.4b/include
