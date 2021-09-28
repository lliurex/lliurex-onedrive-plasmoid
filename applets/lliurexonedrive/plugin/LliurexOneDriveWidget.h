#ifndef PLASMA_LLIUREX_ONEDRIVE_WIDGET_H
#define PLASMA_LLIUREX_ONEDRIVE_WIDGET_H

#include <QObject>
#include <QProcess>
#include <QPointer>
#include <KNotification>
#include <QDir>
#include <QFile>
#include <QThread>

#include "LliurexOneDriveWidgetUtils.h"

class QTimer;
class KNotification;
class AsyncDbus;


class LliurexOneDriveWidget : public QObject
{
    Q_OBJECT


    Q_PROPERTY(TrayStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString toolTip READ toolTip NOTIFY toolTipChanged)
    Q_PROPERTY(QString subToolTip READ subToolTip NOTIFY subToolTipChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY iconNameChanged)
    Q_PROPERTY(QString oneDriveFolder READ oneDriveFolder NOTIFY oneDriveFolderChanged)
    Q_PROPERTY(bool syncStatus READ syncStatus NOTIFY syncStatusChanged)
    Q_PROPERTY(QString freeSpace READ freeSpace NOTIFY freeSpaceChanged)

    Q_ENUMS(TrayStatus)

public:
    /**
     * System tray icon states.
     */
    enum TrayStatus {
        ActiveStatus = 0,
        PassiveStatus
    };

    LliurexOneDriveWidget(QObject *parent = nullptr);

    TrayStatus status() const;
    void changeTryIconState (int state);
    void setStatus(TrayStatus status);

    QString oneDriveFolder() const;
    void setOneDriveFolder(const QString &oneDriveFolder);

    QString freeSpace() const;
    void setFreeSpace(const QString &freeSpace);

    bool syncStatus();
    void setSyncStatus(bool &syncStatus);

    QString toolTip() const;
    void setToolTip(const QString &toolTip);

    QString subToolTip() const;
    void setSubToolTip(const QString &subToolTip);

    QString iconName() const;
    void setIconName(const QString &name);

    QStringList runCheckAccount();
    void isAlive();


public slots:
    
    void worker();
    void launchOneDrive();
    void openFolder();
    void manageSync();

signals:
   
    void oneDriveFolderChanged();
    void syncStatusChanged();
    void freeSpaceChanged();
    void statusChanged();
    void toolTipChanged();
    void subToolTipChanged();
    void iconNameChanged();

private:

    AsyncDbus* adbus;
    void plasmoidMode();
    void initPlasmoid();
    void updateWidget(QString subtooltip,QString icon);
    QTimer *m_timer = nullptr;
    TrayStatus m_status = PassiveStatus;
    QString m_iconName = QStringLiteral("onedrive-stop");
    QString m_oneDriveFolder;
    QString m_freeSpace;
    bool m_syncStatus=false;
    QString m_toolTip;
    QString m_subToolTip;
    QFile TARGET_FILE;
    bool is_working=false;
    int last_check=90;
    LliurexOneDriveWidgetUtils* m_utils;
    QPointer<KNotification> m_errorNotification;

private slots:

     void checkStatus();
     void dbusDone(QStringList result);
     
};

/**
 * Class monitoring the file system quota.
 * The monitoring is performed through a timer, running the 'quota'
 * command line tool.
 */

class AsyncDbus: public QThread

{

    Q_OBJECT

public:
    
    LliurexOneDriveWidget* llxonedrive;
    
    AsyncDbus(LliurexOneDriveWidget* lliurexonedrivewidget)
     {
        llxonedrive = lliurexonedrivewidget;
     }

     void run() override
     {      

        QStringList result=llxonedrive->runCheckAccount();
        emit message(result);

     }
     
signals:

    void message(QStringList);



};


#endif // PLASMA_LLIUREX_ONEDRIVE_WIDGET_H
