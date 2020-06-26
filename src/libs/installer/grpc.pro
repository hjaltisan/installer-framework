win32: {
    GRPC_PATH = $$PWD/../../../../grpc/x86-windows

    DEFINES +=_WIN32_WINNT=0X601

    INCLUDEPATH += $$GRPC_PATH/include
    INCLUDEPATH += $$GRPC_PATH/lib/
    LIBS += -L$$GRPC_PATH/lib/ -llibprotobuf -lgrpc -lgrpc++ -lgpr -lzlib -lws2_32 -llibeay32 -lssleay32

    QMAKE_CXXFLAGS_WARN_ON += /wd4251 /wd4100
}
