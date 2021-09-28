#include "plugin.h"
#include "LliurexOneDriveWidget.h"
#include "LliurexOneDriveWidgetUtils.h"

#include <QtQml>

void LliurexOneDriveWidgetPlugin::registerTypes (const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.plasma.private.lliurexonedrive"));
    qmlRegisterType<LliurexOneDriveWidget>(uri, 1, 0, "LliurexOneDriveWidget");
    qmlRegisterType<LliurexOneDriveWidgetUtils>(uri, 1, 0, "LliurexOneDriveWidgetUtils");
}
