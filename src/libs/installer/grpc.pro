win32: {
    GRPC_PATH = $$PWD/../../../../grpc/x86-windows

    DEFINES +=_WIN32_WINNT=0X601

    INCLUDEPATH += $$GRPC_PATH/include
    INCLUDEPATH += $$GRPC_PATH/lib

    LIBS += -L$$GRPC_PATH/lib/ -llibprotobuf
    LIBS += -L$$GRPC_PATH/lib/ -lgrpc
    LIBS += -L$$GRPC_PATH/lib/ -lgrpc++
    LIBS += -L$$GRPC_PATH/lib/ -lgpr
    LIBS += -L$$GRPC_PATH/lib/ -lzlib
    LIBS += -L$$GRPC_PATH/lib/ -lws2_32
    LIBS += -L$$GRPC_PATH/lib/ -llibeay32
    LIBS += -L$$GRPC_PATH/lib/ -lssleay32

    QMAKE_CXXFLAGS_WARN_ON += /wd4251 /wd4100
}
