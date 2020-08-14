TEMPLATE = subdirs
SUBDIRS += libs sdk sdk/translations
sdk.depends = libs

# Platform Detection Module
    win32: LIBS += -L$$PWD/libs/pdm/lib/ -lpdm
    INCLUDEPATH += $$PWD/libs/pdm/include
    DEPENDPATH += $$PWD/libs/pdm/include
    win32:!win32-g++: PRE_TARGETDEPS += $$PWD/libs/pdm/lib/pdm.lib
    else:win32-g++: PRE_TARGETDEPS += $$PWD/libs/pdm/lib/libpdm.a