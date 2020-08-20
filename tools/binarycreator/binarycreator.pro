TEMPLATE = app
TARGET = binarycreator
INCLUDEPATH += . .. rcc ../common

CONFIG += c++1z

include(../../installerfw.pri)

QT -= gui
QT += qml xml

CONFIG += console
DESTDIR = $$IFW_APP_PATH
SOURCES = binarycreator.cpp \
          rcc/rcc.cpp \
          rcc/rccmain.cpp \
          ../common/repositorygen.cpp
HEADERS = rcc/rcc.h
RESOURCES += binarycreator.qrc

macx:include(../../no_app_bundle.pri)

target.path = $$[QT_INSTALL_BINS]
INSTALLS += target

# Platform Detection Module
    #win32: LIBS += -L$$PWD/../../src/libs/pdm/lib/ -lpdm
    #INCLUDEPATH += $$PWD/../../src/libs/pdm/include
    #DEPENDPATH += $$PWD/../../src/libs/pdm/include
    win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../src/libs/pdm/lib/pdm.lib
    else:win32-g++: PRE_TARGETDEPS += $$PWD/../../src/libs/pdm/lib/libpdm.a