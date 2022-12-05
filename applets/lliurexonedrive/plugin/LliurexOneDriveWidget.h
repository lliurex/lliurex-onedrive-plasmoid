#ifndef PLASMA_LLIUREX_ONEDRIVE_WIDGET_H
#define PLASMA_LLIUREX_ONEDRIVE_WIDGET_H

#include <QObject>
#include <QProcess>
#include <QPointer>
#include <KNotification>
#include <QDir>
#include <QFile>
#include <KIO/CommandLauncherJob>
#include "LliurexOneDriveWidgetUtils.h"

class QTimer;
class KNotification;
class LliurexOneDriveWidgetFilesModel;
class LliurexOneDriveWidgetSpacesModel;

class LliurexOneDriveWidget : public QObject
{
    Q_OBJECT


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
    Q_ENUMS(TrayStatus)

public:
    LliurexOneDriveWidget(QObject *parent = nullptr);


public:
    /**
     * System tray icon states.
     */
    enum TrayStatus {
        ActiveStatus = 0,
        PassiveStatus
    };

public:
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

private:

    LliurexOneDriveWidgetUtils *m_utils;
    LliurexOneDriveWidgetSpacesModel *m_spacesModel = nullptr;
    void plasmoidMode();
    void initPlasmoid();
    void updateWidget(QString subtooltip,QString icon);
    void checkIfStartIsLocked();
    void checkIsRunning();
    void checkStatus();
    void cleanSpaceInfoVars();
    void showMigrationNotification();
    void checkHddFreeSpaceStatus();

    QTimer *m_timer = nullptr;
    TrayStatus m_status = PassiveStatus;
    QString m_iconName = QStringLiteral("onedrive-pause");
    QString userHome;
    QString m_toolTip;
    QString m_subToolTip;
    QFile TARGET_FILE;
    bool isWorking=false;
    int lastCheck=80;
    int lastHddCheck=0;
    int lastErrorNotification=0;
    int countRepeatGeneralError;
    int countRepeatError;
    bool previousError=false;
    bool previousHddWarning=false;
    bool previousHddError=false;
    bool checkExecuted=false;
    bool warning=false;
    bool showStartLockMessage=true;
    QPointer<KNotification> m_errorNotification;
    QList <int> previousStatusError;
    QVariantList oneDriveSpacesConfig;
    QVariantList oneDriveSpacesConfigPrev;
    QString spaceId;
    QString spaceConfigPath;
    QString spaceLocalFolder;
    QString spaceSystemd;
    int m_currentIndex=0;
    QString m_spaceMail;
    QString m_spaceType;
    QString m_oneDriveFolder;
    QString m_freeSpace;
    QString m_hddFreeSpaceStatus="HDD_OK";
    bool m_syncStatus=false;
    bool m_lliurexOneDriveOpen=false;
    LliurexOneDriveWidgetFilesModel *m_filesModel = nullptr;
    QProcess *m_isLliurexOneDriveOpen=nullptr;
    bool isLliurexOneDriveOpen=false;
    QProcess *m_getLatestFiles=nullptr;
    bool m_showSearchFiles=false;
    QFile recentFile;
    QFile OLD_TARGET_FILE;
    int lastMigrationCheck=350;
    bool m_clickedSyncBtn=false;
    bool updateGlobalStatus=false;

private slots:

     void checkIsLliurexOneDriveOpen();
     void isLliurexOneDriveOpenProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
     void getLatestFilesFinished(int exitCode, QProcess::ExitStatus exitStatus);

};

#endif // PLASMA_LLIUREX_ONEDRIVE_WIDGET_H
