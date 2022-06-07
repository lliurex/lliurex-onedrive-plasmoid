#ifndef PLASMA_LLIUREX_ONEDRIVE_WIDGET_UTILS_H
#define PLASMA_LLIUREX_ONEDRIVE_WIDGET_UTILS_H

#include <QObject>
#include <QProcess>
#include <QFile>
#include <QJsonArray>


class QTimer;


class LliurexOneDriveWidgetUtils : public QObject
{
    Q_OBJECT


public:
   
    LliurexOneDriveWidgetUtils(QObject *parent = nullptr);

    QString getUserHome();
    QVariantList getSpacesInfo(QString onedriveConfig);
    QList<bool> checkLocalFolder(QString spaceFolderPath);
    QStringList readStatusToken(QString spaceConfigPath);
    bool checkIfSpaceSyncIsRunning(QString spaceConfigPath);

    QJsonArray onedriveConfig;
    QFile localFolderEmptyToken;
    QFile localFolderRemovedToken;
    QFile spaceStatusToken;

};
#endif // PLASMA_LLIUREX_UP_INDICATOR_UTILS_H
