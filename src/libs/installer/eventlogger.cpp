#include "eventlogger.h"

// #ifdef CCP_LOGGING_ENABLED

// #include "logging.h"
#include "settings.h"
// #include "telemetrywrapper.h"
// #include "parsehardwareaddress.h"
#include <pdm.h>

#include <google/protobuf/util/json_util.h>

#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkInterface>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRandomGenerator>
#include <QSslConfiguration>
#include <QTimer>
#include <QFuture>
#include <QThread>
#include <qtconcurrentrun.h>
#include <QUuid>

Q_LOGGING_CATEGORY(lcEventLogger, "events")

EventLogger::EventLogger()
    : m_shutdownStarted(false),
      m_shutdownEnded(false)
{
    QCryptographicHash hasher(QCryptographicHash::Md5);

    auto interfaces = QNetworkInterface::allInterfaces();
    if(!interfaces.isEmpty())
    {
        auto macAddress = interfaces.first().hardwareAddress();
        hasher.addData(macAddress.toLocal8Bit());
    }
    QString timestamp = QString(QLatin1String("%1")).arg(QDateTime::currentMSecsSinceEpoch());
    hasher.addData(timestamp.toLocal8Bit());
    QString randomNumber = QString(QLatin1String("%1")).arg(QRandomGenerator::securelySeeded().generate());
    hasher.addData(randomNumber.toLocal8Bit());

    m_sessionId = hasher.result();

    std::string osUuidString = PDM::GetMachineUuid();
    QUuid osUuid = QUuid::fromString(QString::fromStdString(osUuidString));
    m_operatingSystemUuid = osUuid.toRfc4122();

    // m_session = "ls" + hasher.result().toHex();
    m_session = QString(QLatin1String("ls")) + QString(QLatin1String(hasher.result().toHex()));
}

EventLogger *EventLogger::instance()
{
    static EventLogger s_instance;
    return &s_instance;
}

// QByteArray EventLogger::prepareMessage(QJsonObject logMessage)
// {
//     TM_ZONE();

//     logMessage["sessionid"] = m_session;
//     // todo: Do we need this?
//     // logMessage["machine_hash"] = QString(Settings::instance()->getMachineHash().toHex());

//     if(!logMessage.contains("timestamp"))
//     {
//         logMessage["timestamp"] = QDateTime::currentDateTimeUtc().toString(TIMESTAMP_FORMAT);
//     }
//     QJsonDocument doc(logMessage);
//     QByteArray finalMsg = doc.toJson(QJsonDocument::Compact);

//     return finalMsg;
// }

void EventLogger::sendQueuedEvents()
{
    QTime shutdown;
    while(true)
    {
        if(m_shutdownStarted && shutdown.isNull())
        {
            qDebug(lcEventLogger()) << "Shutdown timer started";
            shutdown.start();
        }

        if(m_shutdownStarted && shutdown.isValid())
        {
            if(shutdown.elapsed() > 30000) // wait 30 seconds for transmission
            {
                qDebug(lcEventLogger()) << "Shutdown timed out";
                m_shutdownEnded = true;
                return;
            }
        }

        if(m_eventQueue.isEmpty())
        {
            if(m_shutdownStarted)
            {
                qDebug(lcEventLogger()) << "Event Queue empty, shutting down";
                m_shutdownEnded = true;
                return;
            }

            QThread::msleep(1000);
            continue;
        }

        // if (!setupGrpc())
        // {
        //     qDebug(lcEventLogger()) << "Setup failed";
        //     return;
        // }

        // Double buffer so we don't have network involved in the lock
        m_eventQueue_mutex.lock();
        QQueue<EventQueueEntry> eventsToTransmit;
        while(!m_eventQueue.isEmpty())
        {
            auto entry = m_eventQueue.dequeue();
            eventsToTransmit.enqueue(entry);
        }
        m_eventQueue_mutex.unlock();

        QQueue<EventQueueEntry> failedEvents;
        while(!eventsToTransmit.isEmpty())
        {
            EventQueueEntry entry = eventsToTransmit.dequeue();
            if(sendAllocatedEvent(entry))
            {
                delete entry.allocatedPayload;
            }
            else
            {
                failedEvents.enqueue(entry);
            }
        }

        m_eventQueue_mutex.lock();
        while(!failedEvents.isEmpty())
        {
            auto failedEvent = failedEvents.dequeue();
            m_eventQueue.enqueue(failedEvent);
        }
        m_eventQueue_mutex.unlock();
    }
}

bool EventLogger::sendAllocatedEvent(const EventQueueEntry& entry)
{
    google::protobuf::Message* payload = entry.allocatedPayload;

    qDebug(lcEventLogger()) << "Sending" << payload->GetTypeName().c_str() << ":" << payload->ShortDebugString().c_str();

    // Create the event
    auto event = eve_launcher::installer::Event();

    // Set the timestamp
    event.set_allocated_created(getTimestamp(entry.millisecondsSinceEpoch));

    // todo: Create a uuid
    // bytes uuid = 2; 

    // Populated by the gateway so we skip it
    // IPAddress ip_address = 3; 

    // todo: Figure out what this is supposed to be
    // string tenant = 4;

    // Set the payload
    auto any = new google::protobuf::Any;
    any->PackFrom(*payload, "type.evetech.net");
    event.set_allocated_payload(any);

    // todo: Set the Journey ID https://wiki.ccpgames.com/x/cRwuC
    // bytes journey = 6;

    // Populated by the gateway so we skip it
    // google.protobuf.Timestamp received = 7;

    bool result = true;
    std::string jsonEvent = toJSON(&event);
    // todo: actually send the jsonEvent via http post
    if(result)
    {
        qDebug(lcEventLogger()) << "Publishing succeeded" << payload->GetTypeName().c_str() << ":" << payload->ShortDebugString().c_str();
    }
    else
    {
        qDebug(lcEventLogger()) << "Publishing failed" << payload->GetTypeName().c_str() << ":" << payload->ShortDebugString().c_str();
    }

    return result;
}

std::string EventLogger::toJSON(google::protobuf::Message* message)
{
    std::string jsonString;
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;
    options.preserve_proto_field_names = true;
    MessageToJsonString(*message, &jsonString, options);
    return jsonString;
}

void EventLogger::queueAllocatedEvent(google::protobuf::Message* payload)
{
    qDebug(lcEventLogger()) << "Queueing" << payload->GetTypeName().c_str() << ":" << payload->ShortDebugString().c_str();

    EventQueueEntry entry;
    entry.millisecondsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
    entry.allocatedPayload = payload;

    m_eventQueue_mutex.lock();
    m_eventQueue.enqueue(entry);
    m_eventQueue_mutex.unlock();

    if(m_eventQueueThread.isCanceled())
    {
        m_eventQueueThread = QtConcurrent::run(this, &EventLogger::sendQueuedEvents);
    }
}

QString EventLogger::getSession()
{
    return m_session;
}

void EventLogger::waitForFinished()
{
    if(m_shutdownStarted)
    {
        if(m_eventQueueThread.isRunning())
        {
            QTime stopWatch;
            stopWatch.start();
            qDebug() << "Waiting for event logger to finish";
            m_eventQueueThread.waitForFinished();
            qDebug() << "EventLogger no longer running. Waited" << stopWatch.elapsed() / 1000.0f << "seconds";
            return;
        }

        qDebug() << "EventLogger no longer running. No waiting required";
        return;
    }
    qDebug() << "EventLogger has not started shutdown, therefore not waiting for it.";
}

eve_launcher::installer::Application* EventLogger::getApplication()
{
    auto app = new eve_launcher::installer::Application;

    // todo: set the actual installer version
    std::string version = "1.0.x";
    app->set_version(version);
    // app->set_version(STRINGIFY(VERSION));

    // todo: get the actual build type
    // auto buildType = eve_launcher::installer::Application_BuildType(value);
    // Application_BuildType_BUILD_TYPE_UNKNOWN
    // Application_BuildType_BUILD_TYPE_RELEASE
    // Application_BuildType_BUILD_TYPE_BETA
    // Application_BuildType_BUILD_TYPE_DEV
    app->set_build_type(eve_launcher::installer::Application_BuildType_BUILD_TYPE_UNKNOWN);
    // app->set_build_tag(buildType);
    
    // todo: get the actual locale
    // auto locale = eve_launcher::installer::Application_Locale(value);
    // Application_Locale_LOCALE_UNKNOWN
    // Application_Locale_LOCALE_WORLD
    // Application_Locale_LOCALE_CHINA
    app->set_locale(eve_launcher::installer::Application_Locale_LOCALE_UNKNOWN);
    // app->set_locale(locale);

    // todo: get the actual installer type
    // auto applicationType = eve_launcher::installer::Application_ApplicationType(value);
    // Application_ApplicationType_APPLICATION_TYPE_UNKNOWN
    // Application_ApplicationType_APPLICATION_TYPE_NOT_FOUND
    // Application_ApplicationType_APPLICATION_TYPE_INSTALLER
    // Application_ApplicationType_APPLICATION_TYPE_LAUNCHER
    // Application_ApplicationType_APPLICATION_TYPE_UNINSTALLER
    // Application_ApplicationType_APPLICATION_TYPE_UNINSTALLER_UPDATER
    // Application_ApplicationType_APPLICATION_TYPE_UNINSTALLER_PACKAGE_MANAGER
    app->set_application_type(eve_launcher::installer::Application_ApplicationType_APPLICATION_TYPE_UNKNOWN);
    // app->set_application_type(applicationType);

    // todo: get the actual value here
    // if server is tq
    app->set_no_installer_provider(true);
    // if server is sr
    //   if installerProvider is empty
    //     app->set_no_installer_provider(true);
    //   else
    //     app->set_installer_provider(someQString.toStdString())

    return app;
}

eve_launcher::installer::EventMetadata* EventLogger::getEventMetadata()
{
    auto data = new eve_launcher::installer::EventMetadata;

    // Set the md5_session
    data->set_allocated_md5_session(new std::string(m_sessionId.data(), size_t(m_sessionId.size())));

    // Set the application
    data->set_allocated_application(getApplication());

    // Set the operating_system_uuid if any
    if(m_operatingSystemUuid != nullptr)
    {
        data->set_allocated_operating_system_uuid(new std::string(m_operatingSystemUuid.data(), size_t(m_operatingSystemUuid.size())));
    }

    return data;
}

google::protobuf::Timestamp* EventLogger::getTimestamp(qint64 millisecondsSinceEpoch)
{
    if(millisecondsSinceEpoch == 0)
    {
        millisecondsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
    }

    auto now = new google::protobuf::Timestamp;
    now->set_seconds(millisecondsSinceEpoch / 1000);
    now->set_nanos(static_cast<google::protobuf::int32>(millisecondsSinceEpoch % 1000) * 1000000);
    return now;
}

//-----------------------------------------MESSAGE CONSTRUCTION:------------------------------------------------
void EventLogger::started(int duration)
{
    auto evt = new eve_launcher::installer::Started;
    evt->set_allocated_event_metadata(getEventMetadata());
    evt->set_duration(duration);
    queueAllocatedEvent(evt);
}

void EventLogger::pageDisplayed(eve_launcher::installer::Page previousPage, eve_launcher::installer::Page currentPage, eve_launcher::installer::PageDisplayed_FlowDirection flow)
{
    auto evt = new eve_launcher::installer::PageDisplayed;
    evt->set_allocated_event_metadata(getEventMetadata());
    evt->set_previous_page(static_cast<eve_launcher::installer::Page>(previousPage));
    evt->set_current_page(static_cast<eve_launcher::installer::Page>(currentPage));
    evt->set_flow(static_cast<eve_launcher::installer::PageDisplayed_FlowDirection>(flow));
    queueAllocatedEvent(evt);
}

void EventLogger::userCancelled(eve_launcher::installer::Page page, eve_launcher::installer::UserCancelled_Progress progress)
{
    auto evt = new eve_launcher::installer::UserCancelled;
    evt->set_allocated_event_metadata(getEventMetadata());
    evt->set_page(static_cast<eve_launcher::installer::Page>(page));
    evt->set_progress(static_cast<eve_launcher::installer::UserCancelled_Progress>(progress));
    queueAllocatedEvent(evt);
}

void EventLogger::shutDown(eve_launcher::installer::Page page, eve_launcher::installer::ShutDown_State state, bool finishButton)
{
    auto evt = new eve_launcher::installer::ShutDown;
    evt->set_allocated_event_metadata(getEventMetadata());
    evt->set_page(static_cast<eve_launcher::installer::Page>(page));
    evt->set_state(static_cast<eve_launcher::installer::ShutDown_State>(state));
    evt->set_finish_button(finishButton);
    queueAllocatedEvent(evt);
}

void EventLogger::preparationStarted()
{
    auto evt = new eve_launcher::installer::PreparationStarted;
    evt->set_allocated_event_metadata(getEventMetadata());
    queueAllocatedEvent(evt);
}

void EventLogger::preparationFinished(int duration)
{
    auto evt = new eve_launcher::installer::PreparationFinished;
    evt->set_allocated_event_metadata(getEventMetadata());
    evt->set_duration(duration);
    queueAllocatedEvent(evt);
}

void EventLogger::locationChanged(eve_launcher::installer::LocationChanged_Source source, eve_launcher::installer::LocationChanged_Provider provider, const QString& path)
{
    auto evt = new eve_launcher::installer::LocationChanged;
    evt->set_allocated_event_metadata(getEventMetadata());
    evt->set_source(static_cast<eve_launcher::installer::LocationChanged_Source>(source));
    evt->set_provider(static_cast<eve_launcher::installer::LocationChanged_Provider>(provider));
    evt->set_path(path.toStdString());
    queueAllocatedEvent(evt);
}

void EventLogger::detailsVisibilityChanged(bool visible)
{
    auto evt = new eve_launcher::installer::DetailsVisibilityChanged;
    evt->set_allocated_event_metadata(getEventMetadata());
    evt->set_visible(visible);
    queueAllocatedEvent(evt);
}

void EventLogger::autoRunChanged(bool run)
{
    auto evt = new eve_launcher::installer::AutoRunChanged;
    evt->set_allocated_event_metadata(getEventMetadata());
    evt->set_run(run);
    queueAllocatedEvent(evt);
}

void EventLogger::eulaAcceptanceChanged(bool accept)
{
    auto evt = new eve_launcher::installer::EulaAcceptanceChanged;
    evt->set_allocated_event_metadata(getEventMetadata());
    evt->set_accept(accept);
    queueAllocatedEvent(evt);
}

void EventLogger::redistSearchConcluded(eve_launcher::installer::RedistVersion version, eve_launcher::installer::RedistSearchConcluded_RedistReason reason)
{
    auto evt = new eve_launcher::installer::RedistSearchConcluded;
    evt->set_allocated_event_metadata(getEventMetadata());
    evt->set_version(static_cast<eve_launcher::installer::RedistVersion>(version));
    evt->set_reason(static_cast<eve_launcher::installer::RedistSearchConcluded_RedistReason>(reason));
    queueAllocatedEvent(evt);
}

void EventLogger::providedClientFound()
{
    auto evt = new eve_launcher::installer::ProvidedClientFound;
    evt->set_allocated_event_metadata(getEventMetadata());
    queueAllocatedEvent(evt);
}

void EventLogger::messageBoxShown(eve_launcher::installer::MessageBox messageBox)
{
    auto evt = new eve_launcher::installer::MessageBoxShown;
    evt->set_allocated_event_metadata(getEventMetadata());
    evt->set_message_box(static_cast<eve_launcher::installer::MessageBox>(messageBox));
    queueAllocatedEvent(evt);
}

void EventLogger::messageBoxClosed(eve_launcher::installer::MessageBox messageBox, eve_launcher::installer::MessageBoxClosed_MessageBoxButton messageBoxButton, int timeDisplayed)
{
    auto evt = new eve_launcher::installer::MessageBoxClosed;
    evt->set_allocated_event_metadata(getEventMetadata());
    evt->set_message_box(static_cast<eve_launcher::installer::MessageBox>(messageBox));
    evt->set_message_box_button(static_cast<eve_launcher::installer::MessageBoxClosed_MessageBoxButton>(messageBoxButton));
    evt->set_time_displayed(timeDisplayed);
    queueAllocatedEvent(evt);
}

void EventLogger::stepExecuted(eve_launcher::installer::StepExecuted_Step step, eve_launcher::installer::Component component, eve_launcher::installer::RedistVersion redistVersion, int duration)
{
    auto evt = new eve_launcher::installer::StepExecuted;
    evt->set_allocated_event_metadata(getEventMetadata());
    evt->set_step(static_cast<eve_launcher::installer::StepExecuted_Step>(step));
    evt->set_component(static_cast<eve_launcher::installer::Component>(component));
    evt->set_redist_version(static_cast<eve_launcher::installer::RedistVersion>(redistVersion));
    evt->set_duration(duration);
    queueAllocatedEvent(evt);
}

void EventLogger::errorEncountered(eve_launcher::installer::ErrorEncountered_ErrorCode code, eve_launcher::installer::Page page, eve_launcher::installer::Component component, eve_launcher::installer::RedistVersion redistVersion)
{
    auto evt = new eve_launcher::installer::ErrorEncountered;
    evt->set_allocated_event_metadata(getEventMetadata());
    evt->set_code(static_cast<eve_launcher::installer::ErrorEncountered_ErrorCode>(code));
    evt->set_page(static_cast<eve_launcher::installer::Page>(page));
    evt->set_component(static_cast<eve_launcher::installer::Component>(component));
    evt->set_redist_version(static_cast<eve_launcher::installer::RedistVersion>(redistVersion));
    queueAllocatedEvent(evt);
}

// #endif
