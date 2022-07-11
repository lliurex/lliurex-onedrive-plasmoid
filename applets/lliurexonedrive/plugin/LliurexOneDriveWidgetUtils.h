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
    QString getGlobalStatus();
    QString checkLocalFreeSpace();
    QList<QStringList> getFiles(QStringList info,QString spaceLocalFolder);
    void restoreSyncListFile(QString spaceConfigPath);
    
    QJsonArray onedriveConfig;
    QFile localFolderEmptyToken;
    QFile localFolderRemovedToken;
    QFile spaceStatusToken;
    QFile spaceRunToken;
    QFile freeSpaceWarningToken;
    QFile freeSpaceErrorToken;
    int totalSpaces;
    QList<int> spacesStatusCode;
    QList<int> spacesStatusErrorCode;
    bool isLocalFolderWarning=false;
    bool areSpacesSyncRunning=false;

    int MICROSOFT_API_ERROR=-1;
    int UNABLE_CONNECT_MICROSOFT_ERROR=-2;
    int ZERO_SPACE_AVAILABLE_ERROR=-4;
    int UNAUTHORIZED_ERROR=-7;
    int SERVICE_UNAVAILABLE=-9;

    int ALL_SYNCHRONIZE_MSG=0;
    int OUT_OF_SYNC_MSG=2;
    int WITH_OUT_CONFIG=1;
    int INFORMATION_NOT_AVAILABLE=3;
    int UPLOADING_PENDING_CHANGES=4;
    QList <int> warningCode={UPLOADING_PENDING_CHANGES,OUT_OF_SYNC_MSG};
    QList <int> errorCode={MICROSOFT_API_ERROR,UNABLE_CONNECT_MICROSOFT_ERROR,ZERO_SPACE_AVAILABLE_ERROR,UNAUTHORIZED_ERROR,SERVICE_UNAVAILABLE};

private:

    QString formatFreeSpace(QString freeSpace);
    QString formatFileDate(QString fileDate);
    QFile syncList;
    QFile syncListHash;

};
#endif // PLASMA_LLIUREX_UP_INDICATOR_UTILS_H
