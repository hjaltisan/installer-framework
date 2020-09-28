#ifndef EVENTLOGGER_H
#define EVENTLOGGER_H

// #ifdef CCP_LOGGING_ENABLED

#include "eve_launcher/installer.pb.h"

#include <QFuture>
#include <QLoggingCategory>
#include <QMutex>
#include <QObject>
#include <QQueue>

#include <memory>

#define EVENTLOGGER_INSTALLERSTARTED(duration) EventLogger::instance()->started(duration)
#define EVENTLOGGER_INSTALLERPAGEDISPLAYED(previousPage, currentPage, flow) EventLogger::instance()->pageDisplayed(previousPage, currentPage, flow)
#define EVENTLOGGER_INSTALLERUSERCANCELLED(page, progress) EventLogger::instance()->userCancelled(page, progress)
#define EVENTLOGGER_INSTALLERSHUTDOWN(page, state, finishButton) EventLogger::instance()->shutDown(page, state, finishButton)
#define EVENTLOGGER_INSTALLERPREPARATIONSTARTED() EventLogger::instance()->preparationStarted()
#define EVENTLOGGER_INSTALLERPREPARATIONFINISHED(duration) EventLogger::instance()->preparationFinished(duration)
#define EVENTLOGGER_INSTALLERLOCATIONCHANGED(source, provider, path) EventLogger::instance()->locationChanged(source, provider, path)
#define EVENTLOGGER_INSTALLERDETAILSVISIBILITYCHANGED(visible) EventLogger::instance()->detailsVisibilityChanged(visible)
#define EVENTLOGGER_INSTALLERAUTORUNCHANGED(run) EventLogger::instance()->autoRunChanged(run)
#define EVENTLOGGER_INSTALLEREULAACCEPTANCECHANGED(accept) EventLogger::instance()->eulaAcceptanceChanged(accept)
#define EVENTLOGGER_INSTALLERREDISTSEARCHCONCLUDED(version, reason) EventLogger::instance()->redistSearchConcluded(version, reason)
#define EVENTLOGGER_INSTALLERPROVIDEDCLIENTFOUND() EventLogger::instance()->providedClientFound()
#define EVENTLOGGER_INSTALLERMESSAGEBOXSHOWN(messageBox) EventLogger::instance()->messageBoxShown(messageBox)
#define EVENTLOGGER_INSTALLERMESSAGEBOXCLOSED(messageBox, messageBoxButton, timeDisplayed) EventLogger::instance()->messageBoxClosed(messageBox, messageBoxButton, timeDisplayed)
#define EVENTLOGGER_INSTALLERSTEPEXECUTED(step, component, redistVersion, duration) EventLogger::instance()->stepExecuted(step, component, redistVersion, duration)
#define EVENTLOGGER_INSTALLERERRORENCOUNTERED(code, page, component, redistVersion) EventLogger::instance()->errorEncountered(code, page, component, redistVersion)

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

    // what to use for the boolvalue?
    void started(int duration);
    void pageDisplayed(eve_launcher::installer::Page previousPage, eve_launcher::installer::Page currentPage, eve_launcher::installer::PageDisplayed_FlowDirection flow);
    void userCancelled(eve_launcher::installer::Page page, eve_launcher::installer::UserCancelled_Progress progress);
    void shutDown(eve_launcher::installer::Page page, eve_launcher::installer::ShutDown_State state, bool finishButton);
    void preparationStarted();
    void preparationFinished(int duration);
    void locationChanged(eve_launcher::installer::LocationChanged_Source source, eve_launcher::installer::LocationChanged_Provider provider, const QString& path);
    void detailsVisibilityChanged(bool visible);
    void autoRunChanged(bool run);
    void eulaAcceptanceChanged(bool accept);
    void redistSearchConcluded(eve_launcher::installer::RedistVersion version, eve_launcher::installer::RedistSearchConcluded_RedistReason reason);
    void providedClientFound();
    void messageBoxShown(eve_launcher::installer::MessageBox messageBox);
    void messageBoxClosed(eve_launcher::installer::MessageBox messageBox, eve_launcher::installer::MessageBoxClosed_MessageBoxButton messageBoxButton, int timeDisplayed);
    void stepExecuted(eve_launcher::installer::StepExecuted_Step step, eve_launcher::installer::Component component, eve_launcher::installer::RedistVersion redistVersion, int duration);
    void errorEncountered(eve_launcher::installer::ErrorEncountered_ErrorCode code, eve_launcher::installer::Page page, eve_launcher::installer::Component component, eve_launcher::installer::RedistVersion redistVersion);

    QString getSession();
    void waitForFinished();

protected:
    void queueAllocatedEvent(::google::protobuf::Message* payload);

    QByteArray prepareMessage(QJsonObject logMessage);

    QString m_session;
    QByteArray m_sessionId;
    QByteArray m_operatingSystemUuid;

    bool m_shutdownStarted;
    bool m_shutdownEnded;

    QMutex m_eventQueue_mutex;
    QQueue<EventQueueEntry> m_eventQueue;
    QFuture<void> m_eventQueueThread;

    explicit EventLogger();
    google::protobuf::Timestamp* getTimestamp(qint64 millisecondsSinceEpoch = 0);

    eve_launcher::installer::Application* getApplication();
    eve_launcher::installer::EventMetadata* getEventMetadata();

    void sendQueuedEvents();
    bool sendAllocatedEvent(const EventQueueEntry& payload);
    std::string EventLogger::toJSON(::google::protobuf::Message* event);
};

// #else

// #define EVENTLOGGER_INSTALLERSTARTED(duration)
// #define EVENTLOGGER_INSTALLERPAGEDISPLAYED(previousPage, currentPage, flow)
// #define EVENTLOGGER_INSTALLERUSERCANCELLED(page, progress)
// #define EVENTLOGGER_INSTALLERSHUTDOWN(page, state, finishButton)
// #define EVENTLOGGER_INSTALLERPREPARATIONSTARTED()
// #define EVENTLOGGER_INSTALLERPREPARATIONFINISHED(duration)
// #define EVENTLOGGER_INSTALLERLOCATIONCHANGED(source, provider, path)
// #define EVENTLOGGER_INSTALLERDETAILSVISIBILITYCHANGED(visible)
// #define EVENTLOGGER_INSTALLERAUTORUNCHANGED(run)
// #define EVENTLOGGER_INSTALLEREULAACCEPTANCECHANGED(accept)
// #define EVENTLOGGER_INSTALLERREDISTINFORMATION(version, reason)
// #define EVENTLOGGER_INSTALLERCLIENTPROVIDED()
// #define EVENTLOGGER_INSTALLERMESSAGEBOXSHOWN(messageBox)
// #define EVENTLOGGER_INSTALLERMESSAGEBOXCLOSED(messageBox, messageBoxButton, timeDisplayed)
// #define EVENTLOGGER_INSTALLERSTEPEXECUTED(step, component, redistVersion, duration)
// #define EVENTLOGGER_INSTALLERERRORENCOUNTERED(code, page, component, redistVersion)

// #endif // CCP_LOGGING_ENABLED

#endif // EVENTLOGGER_H





