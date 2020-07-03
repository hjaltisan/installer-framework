win32: {
    
    equals(QMAKE_HOST.arch, x86_64) {
        GRPC_PATH = $$PWD/../../../../grpc/x64-windows
    } else {
        GRPC_PATH = $$PWD/../../../../grpc/x86-windows
    }

    DEFINES +=_WIN32_WINNT=0X601

    INCLUDEPATH += $$GRPC_PATH/include
    INCLUDEPATH += $$GRPC_PATH/lib

    LIBS += -L$$GRPC_PATH/lib/ -llibprotobuf -lgrpc -lgrpc++ -lgpr -lzlibstatic -llibcrypto -llibssl -lws32 -lcares -lssl -lcrypto
    
    DEPENDPATH += $$GRPC_PATH/lib

    PRE_TARGETDEPS += $$GRPC_PATH/lib/libprotobuf.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/grpc.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/grpc++.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/gpr.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/zlibstatic.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/libcrypto.lib
    PRE_TARGETDEPS += $$GRPC_PATH/lib/libssl.lib

    QMAKE_CXXFLAGS_WARN_ON += /wd4251 /wd4100
}
