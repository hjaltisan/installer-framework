grpc.pro 1.20

CONFIG += staticlib

win32: {
    
    LIBS_PATH = C:/libs

    DEFINES +=_WIN32_WINNT=0X601

    INCLUDEPATH += $$LIBS_PATH/include
    DEPENDPATH += $$LIBS_PATH/lib

    LIBS += -L$$LIBS_PATH/lib/ -laddress_sorting -lbenchmark -lbenchmark_main -lcares -lcrypto -lgflags_nothreads_static -lgpr -lgrpc -lgrpc_cronet -lgrpc_csharp_ext -lgrpc_plugin_support -lgrpc_unsecure -lgrpc++ -lgrpc++_cronet -lgrpc++_error_details -lgrpc++_reflection -lgrpc++_unsecure -lgrpcpp_channelz -llibprotobuf -llibprotobuf-lite -llibprotoc -lssl -lzlibstatic -llibcrypto -llibssl -lws32

    PRE_TARGETDEPS += $$LIBS_PATH/lib/address_sorting.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/benchmark.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/benchmark_main.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/cares.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/crypto.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/gflags_nothreads_static.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/gpr.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/grpc.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/grpc_cronet.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/grpc_csharp_ext.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/grpc_plugin_support.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/grpc_unsecure.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/grpc++.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/grpc++_cronet.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/grpc++_error_details.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/grpc++_reflection.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/grpc++_unsecure.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/grpcpp_channelz.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/libprotobuf.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/libprotobuf-lite.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/libprotoc.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/ssl.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/zlibstatic.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/libcrypto.lib
    PRE_TARGETDEPS += $$LIBS_PATH/lib/libssl.lib
    
    QMAKE_CXXFLAGS_WARN_ON += /wd4251 /wd4100
}
