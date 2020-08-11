
win32: {
    
    LIBS_PATH = C:/libs

    DEFINES +=_WIN32_WINNT=0X601

    INCLUDEPATH += $$LIBS_PATH/include


    
    QMAKE_CXXFLAGS_WARN_ON += /wd4251 /wd4100
}
