win32: {
    PROTOBUF_PATH = $$PWD/../../../../install

    # Protobuf
    INCLUDEPATH += $$PROTOBUF_PATH/include
    CONFIG(release, debug|release):  LIBS += -L$$PROTOBUF_PATH/lib/ -llibprotobuf
    else:CONFIG(debug, debug|release): LIBS += -L$$PROTOBUF_PATH/lib/ -llibprotobuf
}
