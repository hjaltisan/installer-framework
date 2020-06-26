win32: {
    GRPC_PATH = $$PWD/../../../../grpc/x86-windows

    DEFINES +=_WIN32_WINNT=0X601

    LIBS += -L$$GRPC_PATH/lib/ -llibprotobuf
    LIBS += -L$$GRPC_PATH/lib/ -lgrpc
    LIBS += -L$$GRPC_PATH/lib/ -lgrpc++
    LIBS += -L$$GRPC_PATH/lib/ -lgpr
    LIBS += -L$$GRPC_PATH/lib/ -lzlib
    LIBS += -L$$GRPC_PATH/lib/ -lws2_32
    LIBS += -L$$GRPC_PATH/lib/ -llibeay32
    LIBS += -L$$GRPC_PATH/lib/ -lssleay32

    INCLUDEPATH += $$GRPC_PATH/include
    INCLUDEPATH += $$GRPC_PATH/lib
    DEPENDPATH += $$GRPC_PATH/lib

    PRE_TARGETDEPS += $$GRPC_PATH/lib/libprotobuf.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/grpc.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/grpc++.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/gpr.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/zlib.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/ws2_32.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/libeay32.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/ssleay32.lib

    QMAKE_CXXFLAGS_WARN_ON += /wd4251 /wd4100
}
