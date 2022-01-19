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
    void manageSync(bool isRunning);

    QStringList getAccountStatus(int exitCode,QString poutProces,QString perrProcess);
    QString getErrorMessage(QString code);

    QString UPLOADING_PENDING_CHANGES="416";
    QString NETWORK_CONNECT_ERROR="-2";
    QString ZERO_SPACE_AVAILABLE="-4";
    QString UNAUTHORIZED_ERROR="-7";
    QString GENERAL_ERROR="-1";
    QString NO_PENDING_SYNC="0";
    QString OUT_OF_SYNC="1";
    QString SERVICE_UNAVAILABLE="-9";
    QList<QStringList> getFiles(QStringList info);


private:
    
    QProcess *m_isSystemdActive=nullptr;
    QString formatFreeSpace(QString freeSpace);
    QFile SYSTEMDTOKEN;
    QString formatFileDate(QString fileDate);
  
private slots:

    void checkIsSystemdActive();
    void checkIsSystemdActiveFinished(int exitCode, QProcess::ExitStatus exitStatus);

};


#endif // PLASMA_LLIUREX_UP_INDICATOR_UTILS_H
