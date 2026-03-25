#include "plugin.h"
#include "LliurexOneDriveWidget.h"
#include <QtQml>

void LliurexOneDriveWidgetPlugin::registerTypes (const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.plasma.private.lliurexonedrive"));
    qmlRegisterType<LliurexOneDriveWidget>(uri, 2, 0, "LliurexOneDriveWidget");
}
