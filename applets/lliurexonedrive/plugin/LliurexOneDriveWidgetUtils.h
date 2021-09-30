#ifndef PLASMA_LLIUREX_ONEDRIVE_WIDGET_UTILS_H
#define PLASMA_LLIUREX_ONEDRIVE_WIDGET_UTILS_H

#include <QObject>
#include <QProcess>
#include <QFile>


class QTimer;


class LliurexOneDriveWidgetUtils : public QObject
{
    Q_OBJECT


public:
   
    LliurexOneDriveWidgetUtils(QObject *parent = nullptr);

    QString getUserHome();
    bool isRunning();
    void manageSync();
    bool isSystemdActive();

    QStringList getAccountStatus(int exitCode,QString poutProces,QString perrProcess);

    QString UPLOADING_PENDING_CHANGES="416";
    QString NETWORK_CONNECT_ERROR="-2";
    QString ZERO_SPACE_AVAILABLE="-4";
    QString UNAUTHORIZED_ERROR="-7";
    QString GENERAL_ERROR="-1";
    QString NO_PENDING_SYNC="0";
    QString OUT_OF_SYNC="1"; 


private:

    QString formatFreeSpace(QString freeSpace);
    QFile SYSTEMDTOKEN;
     
};


#endif // PLASMA_LLIUREX_UP_INDICATOR_UTILS_H
