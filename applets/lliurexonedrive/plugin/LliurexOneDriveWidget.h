#ifndef PLASMA_LLIUREX_ONEDRIVE_WIDGET_H
#define PLASMA_LLIUREX_ONEDRIVE_WIDGET_H

#include <QObject>
#include <QProcess>
#include <QPointer>
#include <QFutureWatcher>
#include <QFile>
#include <QVariantMap>

#include <QtQml/qqmlregistration.h>

#include "LliurexOneDriveWidgetUtils.h"
#include "LliurexOneDriveWidgetFileItem.h"
#include "LliurexOneDriveWidgetFilesModel.h"
#include "LliurexOneDriveWidgetSpacesModel.h"

class QTimer;
class KNotification;


class LliurexOneDriveWidget : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    /**
     * System tray icon states.
     */
    enum TrayStatus {
        ActiveStatus = 0,
        PassiveStatus
    };

    Q_PROPERTY(TrayStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString toolTip READ toolTip NOTIFY toolTipChanged)
    Q_PROPERTY(QString subToolTip READ subToolTip NOTIFY subToolTipChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY iconNameChanged)
    Q_PROPERTY(LliurexOneDriveWidgetSpacesModel* spacesModel READ spacesModel CONSTANT)
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString spaceMail READ spaceMail NOTIFY spaceMailChanged)
    Q_PROPERTY(QString spaceType READ spaceType NOTIFY spaceTypeChanged)
    Q_PROPERTY(QString oneDriveFolder READ oneDriveFolder NOTIFY oneDriveFolderChanged)
    Q_PROPERTY(bool syncStatus READ syncStatus NOTIFY syncStatusChanged)
    Q_PROPERTY(QString freeSpace READ freeSpace NOTIFY freeSpaceChanged)
    Q_PROPERTY(bool lliurexOneDriveOpen READ lliurexOneDriveOpen NOTIFY lliurexOneDriveOpenChanged)
    Q_PROPERTY(bool showSearchFiles READ showSearchFiles NOTIFY showSearchFilesChanged)
    Q_PROPERTY(LliurexOneDriveWidgetFilesModel* filesModel READ filesModel CONSTANT)
    Q_PROPERTY(QString hddFreeSpaceStatus READ hddFreeSpaceStatus NOTIFY hddFreeSpaceStatusChanged)
    Q_PROPERTY(bool clickedSyncBtn READ clickedSyncBtn NOTIFY clickedSyncBtnChanged)
    Q_PROPERTY(QString logFileSize READ logFileSize NOTIFY logFileSizeChanged )
    Q_ENUM(TrayStatus)


    explicit LliurexOneDriveWidget(QObject *parent = nullptr);

    TrayStatus status() const;
    void changeTryIconState (int state);
    void setStatus(TrayStatus status);

    QString toolTip() const;
    void setToolTip(const QString &toolTip);

    QString subToolTip() const;
    void setSubToolTip(const QString &subToolTip);

    QString iconName() const;
    void setIconName(const QString &name);

    LliurexOneDriveWidgetSpacesModel *spacesModel() const;

    int currentIndex();
    void setCurrentIndex(int);

    QString spaceMail() const;
    void setSpaceMail(const QString &spaceMail);

    QString spaceType() const;
    void setSpaceType(const QString &spaceType);

    QString oneDriveFolder() const;
    void setOneDriveFolder(const QString &oneDriveFolder);

    QString freeSpace() const;
    void setFreeSpace(const QString &freeSpace);

    QString hddFreeSpaceStatus() const;
    void setHddFreeSpaceStatus(const QString &hddFreeSpaceStatus);

    QString logFileSize() const;
    void setLogFileSize(const QString &logFileSize);

    bool syncStatus();
    void setSyncStatus(bool);

    bool lliurexOneDriveOpen();
    void setLliurexOneDriveOpen(bool);

    bool showSearchFiles();
    void setShowSearchFiles(bool);

    bool clickedSyncBtn();
    void setClickedSyncBtn(bool);

    LliurexOneDriveWidgetFilesModel *filesModel() const;

public slots:
    
    void worker();
    void launchOneDrive(bool seeSpace);
    void openHelp();
    void goToSpace(const QString &idSpace);
    void openFolder();
    void manageSync();
    void manageNavigation(int stackIndex);
    void getLatestFiles();
    void goToFile(const QString &filePath);
    bool checkIfFileExists(const QString &filePath);
    void openLogFile();
    void getLatestUploadedFiles();


signals:
   
    void toolTipChanged();
    void subToolTipChanged();
    void iconNameChanged();
    void statusChanged();
    void currentIndexChanged();
    void spaceMailChanged();
    void spaceTypeChanged();
    void oneDriveFolderChanged();
    void syncStatusChanged();
    void freeSpaceChanged();
    void lliurexOneDriveOpenChanged();
    void showSearchFilesChanged();
    void hddFreeSpaceStatusChanged();
    void clickedSyncBtnChanged();
    void logFileSizeChanged();

private:

    LliurexOneDriveWidgetUtils *m_utils = nullptr;
    LliurexOneDriveWidgetSpacesModel *m_spacesModel = nullptr;
    LliurexOneDriveWidgetFilesModel *m_filesModel = nullptr;
    QTimer *m_timer = nullptr;
    QProcess *m_isLliurexOneDriveOpen = nullptr;
    QPointer<KNotification> m_errorNotification;

    QMap<QString, QVariantMap> oneDriveSpacesConfig;
    QFutureWatcher<QVector<LliurexOneDriveWidgetFileItem>> m_watcher;
    QList<int> previousStatusError;
    QFile TARGET_FILE;
    QFile recentFile;
    QFile OLD_TARGET_FILE;

    QString userHome;
    QString m_toolTip;
    QString m_subToolTip;
    QString m_iconName = QStringLiteral("onedrive-pause");
    QString spaceId;
    QString spaceConfigPath;
    QString spaceLocalFolder;
    QString spaceSystemd;
    QString spaceCreated;
    QString m_spaceMail;
    QString m_spaceType;
    QString m_oneDriveFolder;
    QString m_freeSpace;
    QString m_hddFreeSpaceStatus = "HDD_OK";
    QString logFilePath;
    QString m_logFileSize;
    QString globalStatus;

    TrayStatus m_status = PassiveStatus;
    int m_currentIndex = 0;
    int lastCheck = 80;
    int lastHddCheck = 0;
    int lastErrorNotification = 0;
    int countRepeatGeneralError = 0;
    int countRepeatError = 0;
    int lastMigrationCheck = 350;
    int lastUpdateCheck = 350;

    bool isWorking = false;
    bool previousError = false;
    bool previousHddWarning = false;
    bool previousHddError = false;
    bool checkExecuted = false;
    bool warning = false;
    bool showLocalFolderWarning = true;
    bool m_syncStatus = false;
    bool m_lliurexOneDriveOpen = false;
    bool isLliurexOneDriveOpen = false;
    bool m_showSearchFiles = false;
    bool m_clickedSyncBtn = false;
    bool updateGlobalStatus = false;
    bool isLocalFolderWarning = false;
    bool areSpacesSyncRunning = false;
    bool isUpdateRequiredWarning = false;
    bool statusErrorChanged = false;

    void plasmoidMode();
    void initPlasmoid();
    void updateWidget(QString subtooltip,QString icon);
    void checkIfStartIsLocked();
    void checkIsRunning();
    void checkStatus();
    void cleanSpaceInfoVars();
    void checkHddFreeSpaceStatus();
    void getLogSize(QString configPath);
    void sendNotification (const QString &id, const QString &msg);

    
private slots:

    void checkIsLliurexOneDriveOpen();
    void isLliurexOneDriveOpenProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void getLatestFilesFinished();
    void handleGetSpacesInfoFinished(const SpacesUpdateData &data);
    void handleUploadedFilesFinished(const QVector<LliurexOneDriveWidgetFileItem> &items);

};

#endif // PLASMA_LLIUREX_ONEDRIVE_WIDGET_H
