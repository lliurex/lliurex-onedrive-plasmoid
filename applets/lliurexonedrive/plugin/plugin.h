
#ifndef LLIUREX_ONEDRIVE_WIDGET_WATCH_PLUGIN_H
#define LLIUREX_ONEDRIVE_WIDGET_WATCH_PLUGIN_H

#include <QQmlEngineExtensionPlugin>

class LliurexOneDriveWidgetPlugin : public QQmlEngineExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)
public:
    using QQmlEngineExtensionPlugin::QQmlEngineExtensionPlugin;
};

#endif // LLIUREX_ONEDRIVE_WIDGET_WATCH_PLUGIN_H
