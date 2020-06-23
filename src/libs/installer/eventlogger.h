#ifndef EVENTLOGGER_H
#define EVENTLOGGER_H

#include "grpcpp/channel.h"
#include "grpcpp/security/credentials.h"
#include "eve_launcher/gateway.grpc.pb.h"
#include "eve_launcher/launcher.pb.h"

#include <QFuture>
#include <QLoggingCategory>
#include <QMutex>
#include <QNetworkAccessManager>
#include <QObject>
#include <QQueue>

#include <memory>

#define EVENTLOGGER_LOGINCANCELLED() EventLogger::instance()->loginCancelled()

Q_DECLARE_LOGGING_CATEGORY(lcEventLogger)

struct EventQueueEntry
{
    qint64 millisecondsSinceEpoch;
    google::protobuf::Message* allocatedPayload;
};

class EventLogger : QObject
{
    Q_OBJECT

public:
    static EventLogger* instance();


    void loginCancelled();

    QString getSession();
    QString getAuthenticationToken() const;
    void setAuthenticationToken(const QString &authenticationToken);
    void waitForFinished();

protected:
    void queueAllocatedEvent(::google::protobuf::Message* payload);
    eve_launcher::OS* getOsInfo();

    QByteArray prepareMessage(QJsonObject logMessage);

    QNetworkAccessManager* m_manager;
    QString m_session;
    QByteArray m_sessionId;
    QByteArray m_operatingSystemUuid;

    QString m_rootCertificates;

    bool m_shutdownStarted;
    bool m_shutdownEnded;

    std::unique_ptr<eve_launcher::gateway::GatewayService::Stub> m_grpcStub;
    std::unique_ptr< ::grpc::ClientWriterInterface< ::eve_launcher::Event>> m_eventWriter;
    std::shared_ptr<::grpc::Channel> m_channel;
    std::unique_ptr<grpc::ClientContext> m_grpcContext;
    std::unique_ptr<eve_launcher::gateway::DefaultResponse> m_grpcResponse;

    QString m_authenticationToken;
    QMutex m_eventQueue_mutex;
    QQueue<EventQueueEntry> m_eventQueue;
    QFuture<void> m_eventQueueThread;

    explicit EventLogger();
    google::protobuf::Timestamp* getTimestamp(qint64 millisecondsSinceEpoch = 0);
    eve_launcher::Application* getApplication();
    bool setupGrpc();
    void cleanupGrpc();
    void sendQueuedEvents();
    bool sendAllocatedEvent(const EventQueueEntry& payload);
    eve_launcher::user::Identifier* createUserIdentifier(int userId);
    eve_launcher::IPAddress* toIPAddress(QHostAddress address);
    QByteArray ParseHardwareAddress(QString address);
};

#endif // EVENTLOGGER_H
