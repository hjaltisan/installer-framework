win32: {
    PROTOBUF_PATH = $$PWD/../../../protobuf-3.6.0/x86-windows-static-md

    # Protobuf
    INCLUDEPATH += $$PROTOBUF_PATH/include
    CONFIG(release, debug|release):  LIBS += -L$$PROTOBUF_PATH/lib/ -llibprotobuf
    else:CONFIG(debug, debug|release): LIBS += -L$$PROTOBUF_PATH/debug/lib/ -llibprotobufd
}
