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
class LliurexOneDriveWidgetModel;
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
    Q_PROPERTY(QString spaceSharePoint READ spaceSharePoint NOTIFY spaceSharePointChanged)
    Q_PROPERTY(QString spaceLibrary READ spaceLibrary NOTIFY spaceLibraryChanged)
    Q_PROPERTY(QString oneDriveFolder READ oneDriveFolder NOTIFY oneDriveFolderChanged)
    Q_PROPERTY(bool syncStatus READ syncStatus NOTIFY syncStatusChanged)
    Q_PROPERTY(QString freeSpace READ freeSpace NOTIFY freeSpaceChanged)
    Q_PROPERTY(bool lliurexOneDriveOpen READ lliurexOneDriveOpen NOTIFY lliurexOneDriveOpenChanged)
    Q_PROPERTY(bool showSearchFiles READ showSearchFiles NOTIFY showSearchFilesChanged)
    Q_PROPERTY(LliurexOneDriveWidgetModel* model READ model CONSTANT)

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

    QString spaceSharePoint() const;
    void setSpaceSharePoint(const QString &spaceSharePoint);

    QString spaceLibrary() const;
    void setSpaceLibrary(const QString &spaceLibrary);

    QString oneDriveFolder() const;
    void setOneDriveFolder(const QString &oneDriveFolder);

    QString freeSpace() const;
    void setFreeSpace(const QString &freeSpace);

    bool syncStatus();
    void setSyncStatus(bool);
    bool lliurexOneDriveOpen();
    void setLliurexOneDriveOpen(bool);
    bool showSearchFiles();
    void setShowSearchFiles(bool);

    LliurexOneDriveWidgetModel *model() const;


public slots:
    
    void worker();
    void launchOneDrive();
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
    void spaceSharePointChanged();
    void spaceLibraryChanged();
    void oneDriveFolderChanged();
    void syncStatusChanged();
    void freeSpaceChanged();
    void lliurexOneDriveOpenChanged();
    void showSearchFilesChanged();

private:

    LliurexOneDriveWidgetUtils *m_utils;
    LliurexOneDriveWidgetSpacesModel *m_spacesModel = nullptr;
    void plasmoidMode();
    void initPlasmoid();
    void updateWidget(QString subtooltip,QString icon);
    void checkIfStartIsLocked();
    void checkIsRunning();
    void checkStatus();
    void getSpaceStatusDetails();

    QTimer *m_timer = nullptr;
    TrayStatus m_status = PassiveStatus;
    QString m_iconName = QStringLiteral("onedrive-pause");
    QString userHome;
    QString m_toolTip;
    QString m_subToolTip;
    QFile TARGET_FILE;
    bool isWorking=false;
    int lastCheck=80;
    int countRepeatGeneralError;
    bool previousError=false;
    bool checkExecuted=false;
    bool warning=false;
    bool showStartLockMessage=true;
    QPointer<KNotification> m_errorNotification;
    QList <int> previousStatusError;
    QString spaceConfigPath;
    QString spaceLocalFolder;
    QString spaceSystemd;
    int m_currentIndex=0;
    QString m_spaceMail;
    QString m_spaceType;
    QString m_spaceSharePoint;
    QString m_spaceLibrary;
    QString m_oneDriveFolder;
    QString m_freeSpace;
    bool m_syncStatus=false;
    bool m_lliurexOneDriveOpen=false;
    LliurexOneDriveWidgetModel *m_model = nullptr;
    QProcess *m_getLatestFiles=nullptr;
    bool m_showSearchFiles=false;
    QFile recentFile;

private slots:
     void getLatestFilesFinished(int exitCode, QProcess::ExitStatus exitStatus);

};

#endif // PLASMA_LLIUREX_ONEDRIVE_WIDGET_H
