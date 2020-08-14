TEMPLATE = subdirs
SUBDIRS += 7zip installer
installer.depends = 7zip

# Platform Detection Module
    win32: LIBS += -L$$PWD/pdm/lib/ -lpdm
    INCLUDEPATH += $$PWD/pdm/include
    DEPENDPATH += $$PWD/pdm/include
    win32:!win32-g++: PRE_TARGETDEPS += $$PWD/pdm/lib/pdm.lib
    else:win32-g++: PRE_TARGETDEPS += $$PWD/pdm/lib/libpdm.a
