
# PDM_PATH = C:/pdm
PDM_PATH = $$PWD/../pdm

win32: LIBS += -L$$PDM_PATH/lib/ -lpdm
INCLUDEPATH += $$PDM_PATH/include
DEPENDPATH += $$PDM_PATH/include
win32:!win32-g++: PRE_TARGETDEPS += $$PDM_PATH/lib/pdm.lib
else:win32-g++: PRE_TARGETDEPS += $$PDM_PATH/lib/libpdm.a


win32 {
    
    LIBS_PATH = C:/libs

    DEFINES +=_WIN32_WINNT=0X601

    INCLUDEPATH += $$LIBS_PATH/include
    DEPENDPATH += $$LIBS_PATH/include

    LIBS += $$LIBS_PATH/lib/libprotobufe.lib
    
    QMAKE_CXXFLAGS_WARN_ON += /wd4251 /wd4100
}
