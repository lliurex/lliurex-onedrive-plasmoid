
#ifndef LLIUREX_ONEDRIVE_WIDGET_WATCH_PLUGIN_H
#define LLIUREX_ONEDRIVE_WIDGET_WATCH_PLUGIN_H

#include <QQmlEngine>
#include <QQmlExtensionPlugin>

class LliurexOneDriveWidgetPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
/*    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")*/
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) Q_DECL_OVERRIDE;
};

#endif // LLIUREX_ONEDRIVE_WIDGET_WATCH_PLUGIN_H
