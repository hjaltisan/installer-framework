TEMPLATE = subdirs

SUBDIRS += \
    archivegen \
    binarycreator \
    repogen \
    devtool \
    repocompare


# Platform Detection Module
    win32: LIBS += -L$$PWD/../src/libs/pdm/lib/ -lpdm
    INCLUDEPATH += $$PWD/../src/libs/pdm/include
    DEPENDPATH += $$PWD/../src/libs/pdm/include
    win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../src/libs/pdm/lib/pdm.lib
    else:win32-g++: PRE_TARGETDEPS += $$PWD/../src/libs/pdm/lib/libpdm.a