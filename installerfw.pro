TEMPLATE = subdirs
SUBDIRS += src tools
tools.depends = src

requires(!cross_compile)

include (installerfw.pri)
include (doc/doc.pri)

BUILD_TESTS = $$(BUILDTESTS)
isEmpty(BUILD_TESTS):BUILD_TESTS=$${BUILDTESTS}
!isEmpty(BUILD_TESTS) {
    SUBDIRS += tests
    tests.depends = src
}

BUILD_EXAMPLES = $$(BUILDEXAMPLES)
isEmpty(BUILD_EXAMPLES):BUILD_EXAMPLES=$${BUILDEXAMPLES}
!isEmpty(BUILD_EXAMPLES) {
    SUBDIRS += examples
    examples.depends = src
}

!minQtVersion(5, 9, 5) {
    message("Cannot build Qt Installer Framework with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.9.5.")
}

# Platform Detection Module
    win32: LIBS += -L$$PWD/src/libs/pdm/lib/ -lpdm
    INCLUDEPATH += $$PWD/src/libs/pdm/include
    DEPENDPATH += $$PWD/src/libs/pdm/include
    win32:!win32-g++: PRE_TARGETDEPS += $$PWD/src/libs/pdm/lib/pdm.lib
    else:win32-g++: PRE_TARGETDEPS += $$PWD/src/libs/pdm/lib/libpdm.a