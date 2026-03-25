#ifndef PLASMA_LLIUREX_ONEDRIVE_WIDGET_UTILS_H
#define PLASMA_LLIUREX_ONEDRIVE_WIDGET_UTILS_H

#include <QObject>
#include <QProcess>
#include <QFile>
#include <QVariantMap>
#include <QVector>
#include <QList>

#include "LliurexOneDriveWidgetSpaceItem.h"
#include "LliurexOneDriveWidgetFileItem.h"

class QJsonObject; 
class QTimer;

struct SpacesUpdateData {
        QMap<QString, QVariantMap> info;
        QVector<LliurexOneDriveWidgetSpaceItem> model;
        QList<int> statusErrorCodes;
        QString status;
        bool isRunning;
        bool folderWarning;
        bool updateRequired;
};
Q_DECLARE_METATYPE(SpacesUpdateData)

class LliurexOneDriveWidgetUtils : public QObject
{
    Q_OBJECT


public:
   
    explicit LliurexOneDriveWidgetUtils(QObject *parent = nullptr);

    QString getUserHome();
    void getSpacesInfo(QString onedriveConfig);
    QList<bool> checkLocalFolder(QString spaceFolderPath);
    QStringList readStatusToken(QString spaceConfigPath);
    bool checkIfSpaceSyncIsRunning(QString spaceConfigPath) ;
    QString checkLocalFreeSpace();
    static QVector<LliurexOneDriveWidgetFileItem>getFiles(const QString& spaceLocalFolder, int limit);
    void fetchUploadedFiles(const QString &unit, const QString &since);
    void restoreSyncListFile(QString spaceConfigPath);
    QString getLogFileSize(QString logFilePath);
    bool checkUpdateRequired(QString spaceConfigPath); 

    QString user;
    QFile localFolderEmptyToken;
    QFile localFolderRemovedToken;
    QFile spaceStatusToken;
    QFile spaceRunToken;
    QFile freeSpaceWarningToken;
    QFile freeSpaceErrorToken;
    QFile updateRequiredToken;
    QFile logFile;

    int totalSpaces;

    int MICROSOFT_API_ERROR=-1;
    int UNABLE_CONNECT_MICROSOFT_ERROR=-2;
    int ZERO_SPACE_AVAILABLE_ERROR=-4;
    int UNAUTHORIZED_ERROR=-7;
    int SERVICE_UNAVAILABLE=-9;
    int FORBIDDEN_USER=-14;

    int ALL_SYNCHRONIZE_MSG=0;
    int OUT_OF_SYNC_MSG=2;
    int WITH_OUT_CONFIG=1;
    int INFORMATION_NOT_AVAILABLE=3;
    int UPLOADING_PENDING_CHANGES=4;
    int UPLOADING_PENDING_CHANGES_BACKUP=5;

    QList <int> warningCode={UPLOADING_PENDING_CHANGES,OUT_OF_SYNC_MSG,UPLOADING_PENDING_CHANGES_BACKUP};
    QList <int> errorCode={MICROSOFT_API_ERROR,UNABLE_CONNECT_MICROSOFT_ERROR,ZERO_SPACE_AVAILABLE_ERROR,UNAUTHORIZED_ERROR,SERVICE_UNAVAILABLE,FORBIDDEN_USER};
 

    signals:
        void getSpacesInfoFinished(const SpacesUpdateData &data);    
        void fetchUploadedFilesFinished(const QVector<LliurexOneDriveWidgetFileItem> &items);
private:

    QPair<QString, QList<int>> getGlobalStatus(int totalSpaces, QList<int> spacesStatusCode);
    QString formatFreeSpace(QString freeSpace);
    static QString formatFileDate(QString fileDate);
    
    QVariantMap processSingleSpace(const QJsonObject &obj); 

    QMap<QString, QVariantMap> oneDriveSpacesConfig;
    QFile syncList;
    QFile syncListHash;
    QProcess *m_process = nullptr;

    static QVector<LliurexOneDriveWidgetFileItem> parseData(const QByteArray &data);

    private slots:
       
       void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};
#endif // PLASMA_LLIUREX_ONEDRIVE_WIDGET_UTILS_H
