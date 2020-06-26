win32: {
    GRPC_PATH = $$PWD/../../../../grpc/x86-windows

    DEFINES +=_WIN32_WINNT=0X601

    INCLUDEPATH += $$GRPC_PATH/include

    QMAKE_CXXFLAGS_WARN_ON += /wd4251 /wd4100
}
