#include "grpcevent.h"
// #include "eventlogger.h"
#include <QMessageBox>

#include <pdm/pdm.h>
#include <pdm/pdm_data.h>
#include <pdm/version.h>


using namespace QInstaller;

GrpcEvent::GrpcEvent(PackageManagerCore *core)
    : UpdateOperation(core)
{
    setName(QLatin1String("GrpcEvent"));
}

void GrpcEvent::backup()
{
}


bool GrpcEvent::performOperation()
{
    // //if (!checkArgumentCount(2, 2, tr(" (Url, Payload)")))
    //   //  return false;

    // //const QStringList args = arguments();
    // //const QString url = args.at(0);
    // //const QString payloadData = args.at(1);
    // QMessageBox::question(
    //             nullptr,
    //             tr("Things"),
    //             tr("Do you want to do stuff?")
    //             );
    // qDebug(lcEventLogger()) << "GrpcEvent::performOperation";
    // EVENTLOGGER_LOGINCANCELLED();
    bool test = PDM::IsWine();
    return true;
}

bool GrpcEvent::undoOperation()
{
    return true;
}

bool GrpcEvent::testOperation()
{
    return true;
}

