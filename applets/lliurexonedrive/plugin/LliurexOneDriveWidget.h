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

public slots:
    
    void worker();
    void launchOneDrive();
    void openHelp();

signals:
   
    void toolTipChanged();
    void subToolTipChanged();
    void iconNameChanged();
    void statusChanged();

private:

    LliurexOneDriveWidgetUtils *m_utils;
    LliurexOneDriveWidgetSpacesModel *m_spacesModel = nullptr;
    void plasmoidMode();
    void initPlasmoid();
    void updateWidget(QString subtooltip,QString icon);
    void checkIfStartIsLocked();
    void checkIsRunning();
    void checkStatus();

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
};

#endif // PLASMA_LLIUREX_ONEDRIVE_WIDGET_H
