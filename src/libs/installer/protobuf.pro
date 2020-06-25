win32: {
    PROTOBUF_PATH = $$_PRO_FILE_PWD_/../../../../install

    # Protobuf
    INCLUDEPATH += $$PROTOBUF_PATH/include
    CONFIG(release, debug|release):  LIBS += -L$$PROTOBUF_PATH/lib/ -llibprotobuf
    else:CONFIG(debug, debug|release): LIBS += -L$$PROTOBUF_PATH/lib/ -llibprotobuf
}
