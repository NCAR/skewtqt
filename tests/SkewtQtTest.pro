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

DESTDIR = c:/Projects/bin

INCLUDEPATH += ../
INCLUDEPATH += ../..
INCLUDEPATH += ../../../
INCLUDEPATH += c:/Projects/include

LIBS += ../Debug/skewtqt.lib

# Libraries from other packages
LIBS += c:/Binaries/Aspen/lib/Debug/SkewT.lib
 

