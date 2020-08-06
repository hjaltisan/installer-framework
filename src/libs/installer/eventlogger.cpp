// #include "eventlogger.h"

// #include "settings.h"

// #include "grpcpp/create_channel.h"
// #include "grpcpp/channel.h"
// #include "grpc/status.h"
// #include "grpc/grpc_security.h"

// #include <QEventLoop>
// #include <QJsonDocument>
// #include <QJsonObject>
// #include <QNetworkInterface>
// #include <QNetworkReply>
// #include <QNetworkRequest>
// #include <QRandomGenerator>
// #include <QSettings>
// #include <QSslConfiguration>
// #include <QString>
// #include <QTimer>
// #include <QFuture>
// #include <QThread>
// #include <qtconcurrentrun.h>
// #include <QUuid>

// Q_LOGGING_CATEGORY(lcEventLogger, "events")

// class MyCustomAuthenticator : public grpc::MetadataCredentialsPlugin {
//  public:
//   MyCustomAuthenticator(const grpc::string& ticket) : ticket_(ticket) {}

//   grpc::Status GetMetadata(
//       grpc::string_ref service_url, grpc::string_ref method_name,
//       const grpc::AuthContext& channel_auth_context,
//       std::multimap<grpc::string, grpc::string>* metadata) override {
//     metadata->insert(std::make_pair("x-custom-auth-ticket", ticket_));
//     return grpc::Status::OK;
//   }

//  private:
//   grpc::string ticket_;
// };


// bool EventLogger::setupGrpc()
// {

//     QTime startingSetup;
//     startingSetup.start();
//     qDebug(lcEventLogger()) << "Channel setup";
//     if(!m_channel)
//     {
//         // Without this, we were seeing handshake errors when connecting to the service.
//         qputenv("GRPC_SSL_CIPHER_SUITES", "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384");

//         grpc::SslCredentialsOptions opts;
//         opts.pem_root_certs = m_rootCertificates.toStdString();

//         auto ssl_creds = grpc::SslCredentials(opts);

//         auto call_creds = grpc::MetadataCredentialsFromPlugin(
//             std::unique_ptr<grpc::MetadataCredentialsPlugin>(
//                 new MyCustomAuthenticator(m_authenticationToken.toStdString())));

//         auto channel_creds = grpc::CompositeChannelCredentials(ssl_creds, call_creds);

//         QString host = QLatin1String("elg.evetech.net"); //TODO Set to elg.evepc.163.com if CHINA

//         m_channel = grpc::CreateChannel(host.toStdString(), channel_creds);

//         qDebug(lcEventLogger()) << "Channel created";
//     }

//     while(m_channel->GetState(true) != GRPC_CHANNEL_READY)
//     {
//         if(m_channel->GetState(false) == GRPC_CHANNEL_SHUTDOWN)
//         {
//             qDebug(lcEventLogger()) << "Channel shutdown";
//             m_channel = nullptr;
//             return false;
//         }

//         if(m_shutdownEnded)
//         {
//             qDebug(lcEventLogger()) << "Aborting channel state check";
//             return false;
//         }

//         auto state = m_channel->GetState(false);

//         switch(state)
//         {
//         case GRPC_CHANNEL_IDLE:
//             qDebug(lcEventLogger()) << "Channel state: Idle";
//             break;
//         case GRPC_CHANNEL_CONNECTING:
//             qDebug(lcEventLogger()) << "Channel state: Connecting";
//             break;
//         default:
//             qDebug(lcEventLogger()) << "Channel state:" << state;
//             break;
//         }

//         QThread::msleep(1000);
//     }

//     qDebug(lcEventLogger()) << "Channel ready. Setup duration:" << startingSetup.elapsed() / 1000.0f << "seconds";
//     m_grpcStub = eve_launcher::gateway::GatewayService::NewStub(m_channel);
//     m_grpcContext.reset(new grpc::ClientContext);
//     std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() + std::chrono::seconds(10);
//     m_grpcContext.get()->set_deadline(deadline);
//     m_grpcResponse.reset(new eve_launcher::gateway::DefaultResponse);
//     m_eventWriter = m_grpcStub->PublishEvent(m_grpcContext.get(), m_grpcResponse.get());

//     return true;
// }

// void EventLogger::cleanupGrpc()
// {
//     if(m_eventWriter)
//     {
//         m_eventWriter->Finish();
//         m_eventWriter = nullptr;
//         qDebug(lcEventLogger()) << "Stub orphaned";
//     }

//     m_grpcResponse.reset();
//     m_grpcContext.reset();
//     m_channel = nullptr;
//     qDebug(lcEventLogger()) << "Channel orphaned";
// }

// EventLogger::EventLogger()
//     : m_manager(nullptr),
//       m_shutdownStarted(false),
//       m_shutdownEnded(false)
// {
//     QCryptographicHash hasher(QCryptographicHash::Md5);

//     auto interfaces = QNetworkInterface::allInterfaces();
//     if(!interfaces.isEmpty())
//     {
//         auto macAddress = interfaces.first().hardwareAddress();
//         hasher.addData(macAddress.toLocal8Bit());
//     }
//     //TODO:
//     //QString timestamp = QLatin1String("%1").arg(QDateTime::currentMSecsSinceEpoch());
//     //hasher.addData(timestamp.toLocal8Bit());
//     //QString randomNumber = QLatin1String("%1").arg(QRandomGenerator::securelySeeded().generate());
//     //hasher.addData(randomNumber.toLocal8Bit());

//     m_sessionId = hasher.result();

//     //TODO uncomment and use in next line
//     //QSettings::Format format = (windowsBitness() == Bitness::BITNESS_64) ? QSettings::Registry64Format : QSettings::Registry32Format;
//     QSettings winVersion(QLatin1String("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography"), QSettings::Registry64Format);
//     if(winVersion.contains(QLatin1String("MachineGuid")))
//     {
//         QUuid osUuid = QUuid::fromString(winVersion.value(QLatin1String("MachineGuid")).toString());
//         m_operatingSystemUuid = osUuid.toRfc4122();
//     }

//     //TODO:
//     /*QString osUuidString = OperatingSystemInformation::Instance()->GetOperatingSystemUuid();
//     if(osUuidString==OS_UUID_UNKNOWN)
//     {
//         m_operatingSystemUuid = nullptr;
//     }
//     else
//     {
//         QUuid osUuid = QUuid::fromString(osUuidString);
//         m_operatingSystemUuid = osUuid.toRfc4122();
//     }*/

//     //TODO:
//     //m_session = QLatin1String("ls") + hasher.result().toHex();

//     auto sslConfig = QSslConfiguration::defaultConfiguration();
//     auto caCertChain = sslConfig.caCertificates();


//     // Upgrade gRPC >= 1.19 to properly unfuck this
//     // https://github.com/grpc/grpc/pull/17664
//     QString pem;
//     //TOOO fixit:
//     /*for(auto cert: caCertChain)
//     {
//         auto newPem = cert.toPem();

//         if(!pem.contains(newPem))
//         {
//             pem += cert.toPem();
//         }
//     }*/

//     m_rootCertificates = pem;
// }

// EventLogger *EventLogger::instance()
// {
//     static EventLogger s_instance;
//     return &s_instance;
// }

// QByteArray EventLogger::prepareMessage(QJsonObject logMessage)
// {
//     logMessage[QLatin1String("sessionid")] = m_session;
//     //logMessage[QLatin1String("machine_hash")] = QString(Settings::instance()->getMachineHash().toHex()); //TODO MACHINE HASH

//     if(!logMessage.contains(QLatin1String("timestamp")))
//     {
//         logMessage[QLatin1String("timestamp")] = QDateTime::currentDateTimeUtc().toString(QLatin1String("yyyy-MM-ddTHH:mm:ss.zzzZ"));
//     }
//     QJsonDocument doc(logMessage);
//     QByteArray finalMsg = doc.toJson(QJsonDocument::Compact);

//     return finalMsg;
// }

// QString EventLogger::getAuthenticationToken() const
// {
//     return m_authenticationToken;
// }

// void EventLogger::sendQueuedEvents()
// {
//     QTime shutdown;
//     while(true)
//     {
//         if(m_shutdownStarted && shutdown.isNull())
//         {
//             qDebug(lcEventLogger()) << "Shutdown timer started";
//             shutdown.start();
//         }

//         if(m_shutdownStarted && shutdown.isValid())
//         {
//             if(shutdown.elapsed() > 30000) // wait 30 seconds for transmission
//             {
//                 qDebug(lcEventLogger()) << "Shutdown timed out";
//                 m_shutdownEnded = true;
//                 return;
//             }
//         }

//         if(m_eventQueue.isEmpty())
//         {
//             if(m_shutdownStarted)
//             {
//                 qDebug(lcEventLogger()) << "Event Queue empty, shutting down";
//                 m_shutdownEnded = true;
//                 return;
//             }

//             QThread::msleep(1000);
//             continue;
//         }

//         if (!setupGrpc())
//         {
//             qDebug(lcEventLogger()) << "Setup failed";
//             return;
//         }

//         // Double buffer so we don't have network involved in the lock
//         m_eventQueue_mutex.lock();
//         QQueue<EventQueueEntry> eventsToTransmit;
//         while(!m_eventQueue.isEmpty())
//         {
//             auto entry = m_eventQueue.dequeue();
//             eventsToTransmit.enqueue(entry);
//         }
//         m_eventQueue_mutex.unlock();

//         QQueue<EventQueueEntry> failedEvents;
//         while(!eventsToTransmit.isEmpty())
//         {
//             EventQueueEntry entry = eventsToTransmit.dequeue();
//             if(sendAllocatedEvent(entry))
//             {
//                 delete entry.allocatedPayload;
//             }
//             else
//             {
//                 failedEvents.enqueue(entry);
//             }
//         }

//         m_eventQueue_mutex.lock();
//         while(!failedEvents.isEmpty())
//         {
//             auto failedEvent = failedEvents.dequeue();
//             m_eventQueue.enqueue(failedEvent);
//         }
//         m_eventQueue_mutex.unlock();
//     }
// }

// void EventLogger::setAuthenticationToken(const QString &authenticationToken)
// {
//     if(!m_authenticationToken.isEmpty() && m_authenticationToken != authenticationToken)
//     {
//         qDebug(lcEventLogger()) << "Authentication token is changing";
//         // TODO: Reset grpc channel
//     }

//     m_authenticationToken = authenticationToken;
//     if(m_authenticationToken.isEmpty())
//     {
//         return;
//     }

// }

// bool EventLogger::sendAllocatedEvent(const EventQueueEntry& entry)
// {
//     google::protobuf::Message* payload = entry.allocatedPayload;
//     if(!m_eventWriter)
//     {
//         return false;
//     }

//     qDebug(lcEventLogger()) << "Sending" << payload->GetTypeName().c_str() << ":" << payload->ShortDebugString().c_str();

//     auto event = eve_launcher::Event();
//     event.set_allocated_timestamp(getTimestamp(entry.millisecondsSinceEpoch));
//     auto any = new google::protobuf::Any;
//     any->PackFrom(*payload, "type.evetech.net");
//     event.set_allocated_payload(any);


//     event.set_allocated_md5_session(new std::string(m_sessionId.data(), size_t(m_sessionId.size())));
//     if(m_operatingSystemUuid != nullptr)
//     {
//         event.set_allocated_operatingsystemuuid(new std::string(m_operatingSystemUuid.data(), size_t(m_operatingSystemUuid.size())));
//     }

//     event.set_allocated_application(getApplication());

//     bool result = m_eventWriter->Write(event);
//     if(result)
//     {
//         qDebug(lcEventLogger()) << "Publishing succeeded" << payload->GetTypeName().c_str() << ":" << payload->ShortDebugString().c_str();
//     }
//     else
//     {
//         qDebug(lcEventLogger()) << "Publishing failed" << payload->GetTypeName().c_str() << ":" << payload->ShortDebugString().c_str();
//         cleanupGrpc();
//     }

//     return result;
// }

// void EventLogger::queueAllocatedEvent(google::protobuf::Message* payload)
// {
//     qDebug(lcEventLogger()) << "Queueing" << payload->GetTypeName().c_str() << ":" << payload->ShortDebugString().c_str();

//     EventQueueEntry entry;
//     entry.millisecondsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
//     entry.allocatedPayload = payload;

//     m_eventQueue_mutex.lock();
//     m_eventQueue.enqueue(entry);
//     m_eventQueue_mutex.unlock();

//     if(m_eventQueueThread.isCanceled())
//     {
//         m_eventQueueThread = QtConcurrent::run(this, &EventLogger::sendQueuedEvents);
//     }
// }

// QString EventLogger::getSession()
// {
//     return m_session;
// }

// void EventLogger::waitForFinished()
// {
//     if(m_shutdownStarted)
//     {
//         if(m_eventQueueThread.isRunning())
//         {
//             QTime stopWatch;
//             stopWatch.start();
//             qDebug() << "Waiting for event logger to finish";
//             m_eventQueueThread.waitForFinished();
//             qDebug() << "EventLogger no longer running. Waited" << stopWatch.elapsed() / 1000.0f << "seconds";
//             return;
//         }

//         qDebug() << "EventLogger no longer running. No waiting required";
//         return;
//     }
//     qDebug() << "EventLogger has not started shutdown, therefore not waiting for it.";
// }

// //-----------------------------------------MESSAGE CONSTRUCTION:------------------------------------------------
// void EventLogger::loginCancelled()
// {
//     queueAllocatedEvent(new eve_launcher::LoginCancelled);
// }

// google::protobuf::Timestamp* EventLogger::getTimestamp(qint64 millisecondsSinceEpoch)
// {
//     if(millisecondsSinceEpoch == 0)
//     {
//         millisecondsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
//     }

//     auto now = new google::protobuf::Timestamp;
//     now->set_seconds(millisecondsSinceEpoch / 1000);
//     now->set_nanos(static_cast<google::protobuf::int32>(millisecondsSinceEpoch % 1000) * 1000000);
//     return now;
// }

// eve_launcher::Application* EventLogger::getApplication() {
//     auto app = new eve_launcher::Application;
//     //app->set_version(STRINGIFY(VERSION)); //TODO set the version

//     auto buildTag = eve_launcher::Application_BuildTag::Application_BuildTag_RELEASE;
//     app->set_build_tag(buildTag);

//     #ifdef CCP_BRANCH_CHINA
//         app->set_locale(eve_launcher::Application_Locale_CHINA);
//     #else
//         app->set_locale(eve_launcher::Application_Locale_WORLD);
//     #endif
//     return app;
// }

// eve_launcher::OS* EventLogger::getOsInfo(){
//     auto os = new eve_launcher::OS;
//     os->set_kind(eve_launcher::OS_Kind::OS_Kind_WINDOWS);
//     os->set_version(QSysInfo::productVersion().toStdString());
//     os->set_buildversion(QSysInfo::kernelVersion().toStdString());
//     os->set_prettyproductname(QSysInfo::prettyProductName().toStdString());
//     os->set_processorarchitecture(QSysInfo::currentCpuArchitecture().toStdString());
//     //os->set_bitness(static_cast<eve_launcher::Bitness>(OperatingSystemInformation::Instance()->GetBitness())); //TODO determine OS bitness (steal from the app)

//     return os;
// }

// eve_launcher::IPAddress* EventLogger::toIPAddress(QHostAddress address)
// {
//     eve_launcher::IPAddress* ipAddress = new eve_launcher::IPAddress;
//     if (address.protocol() == QAbstractSocket::IPv4Protocol)
//     {
//         ipAddress->set_v4(static_cast<google::protobuf::uint32>(address.toIPv4Address()));
//     }
//     else if (address.protocol() == QAbstractSocket::IPv6Protocol)
//     {
//         QString ipv6 = address.toString();
//         ipAddress->set_v6(ipv6.toStdString());
//     }

//     return ipAddress;
// }


// unsigned char hexDigitValue(QChar input)
// {
//     unsigned char result;
//     if(input.isDigit())
//     {
//         result = static_cast<unsigned char>(input.digitValue());
//     }
//     else
//     {
//         result = static_cast<unsigned char>(input.toLatin1()) - static_cast<unsigned char>('a') + 10;
//     }
//     return result;
// }

// QByteArray ParseHardwareAddress(QString address)
// {
//     QByteArray result;

//     auto stripped = address.remove(QLatin1String(":")).remove(QLatin1String("-")).remove(QLatin1String(".")).toLower();

//     if(stripped.size() % 2 != 0)
//     {
//         return result;
//     }

//     for(int i = 0; i < stripped.size(); i += 2)
//     {
//         unsigned char high = hexDigitValue(stripped[i]);
//         unsigned char low = hexDigitValue(stripped[i+1]);
//         unsigned char byte = high << 4 | low;
//         result.push_back(static_cast<char>(byte));
//     }

//     return result;
// }

