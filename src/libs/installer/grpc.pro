win32: {
    
    LIBS_PATH = $$PWD/../../../../../libs/x86-windows

    DEFINES +=_WIN32_WINNT=0X601

    INCLUDEPATH += $$LIBS_PATH/include
    INCLUDEPATH += $$LIBS_PATH/lib

    LIBS += -L$$LIBS_PATH/lib/ -llibprotobuf -lgrpc -lgrpc++ -lgpr -lzlibstatic -llibcrypto -llibssl -lws32 -lcares -lssl -lcrypto

    DEPENDPATH += $$LIBS_PATH/lib

    PRE_TARGETDEPS += $$LIBS_PATH/lib/libprotobuf.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/grpc.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/grpc++.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/gpr.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/zlibstatic.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/libcrypto.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/libssl.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/grpcpp_channelz.lib
    
    QMAKE_CXXFLAGS_WARN_ON += /wd4251 /wd4100
}
