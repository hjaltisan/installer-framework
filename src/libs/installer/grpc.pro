CONFIG += staticlib

win32: {
    
    LIBS_PATH = C:/libs

    DEFINES +=_WIN32_WINNT=0X601

    INCLUDEPATH += $$LIBS_PATH/include
    DEPENDPATH += $$LIBS_PATH/include

    LIBS += -L$$LIBS_PATH/lib/address_sortinge.lib
    
    QMAKE_CXXFLAGS_WARN_ON += /wd4251 /wd4100
}
