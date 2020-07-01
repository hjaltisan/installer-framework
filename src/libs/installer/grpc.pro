win32: {
    
    equals(QMAKE_HOST.arch, x86_64) {
        GRPC_PATH = $$PWD/../../../../grpc/x64-windows
    } else {
        GRPC_PATH = $$PWD/../../../../grpc/x86-windows
    }

    DEFINES +=_WIN32_WINNT=0X601

    INCLUDEPATH += $$GRPC_PATH/include
    INCLUDEPATH += $$GRPC_PATH/lib

    LIBS += -L$$GRPC_PATH/lib/ -llibprotobuf -lgrpc -lgrpc++ -lgpr -lzlibstatic -llibeay32 -lssleay32 -lws32
    
    DEPENDPATH += $$GRPC_PATH/lib

    PRE_TARGETDEPS += $$GRPC_PATH/lib/libprotobuf.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/grpc.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/grpc++.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/gpr.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/zlibstatic.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/libeay32.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/ssleay32.lib

    QMAKE_CXXFLAGS_WARN_ON += /wd4251 /wd4100
}
