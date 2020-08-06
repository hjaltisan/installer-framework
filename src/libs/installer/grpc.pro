CONFIG += staticlib

win32: {
    
    LIBS_PATH = C:/libs/x86-windows

    DEFINES +=_WIN32_WINNT=0X601

    INCLUDEPATH += $$LIBS_PATH/include
    INCLUDEPATH += $$LIBS_PATH/lib

    LIBS += -L$$LIBS_PATH/lib/ -llibprotobuf -lgrpc -lgrpc++ -lgpr -lzlibstatic -llibcrypto -llibssl -lws32 -lcares -lssl -lcrypto

    DEPENDPATH += $$LIBS_PATH/lib
    
    QMAKE_CXXFLAGS_WARN_ON += /wd4251 /wd4100
}
