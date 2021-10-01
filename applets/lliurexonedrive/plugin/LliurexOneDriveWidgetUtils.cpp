
#include <iostream>
#include <string>
#include "LliurexOneDriveWidgetUtils.h"
#include <QLocale>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStandardPaths>
#include <sys/types.h>
#include <KIO/CommandLauncherJob>


LliurexOneDriveWidgetUtils::LliurexOneDriveWidgetUtils(QObject *parent)
    : QObject(parent)
       
{
  
}    

QString LliurexOneDriveWidgetUtils::getUserHome() {

    QString user=qgetenv("USER");
    QString userHome="/home/"+user;

    return userHome;

}

bool LliurexOneDriveWidgetUtils::isRunning(){

    QProcess process;
    QString cmd="ps -ef | grep '/usr/bin/onedrive --monitor' | grep -v 'grep'";
    process.start("/bin/sh", QStringList()<< "-c" 
                       << cmd,QIODevice::ReadOnly);
    process.waitForFinished(-1);
    QString stdout=QString::fromLocal8Bit(process.readAllStandardOutput());
    QStringList pout=stdout.split("\n");
        
    if (pout[0].size()>0){
        return true;
    }else{
        return false;
    }
   
}

bool LliurexOneDriveWidgetUtils::isSystemdActive(){

    QProcess process;
    QString cmd="systemctl --user is-active onedrive.service";
    process.start("/bin/sh",QStringList()<<"-c"<<cmd,QIODevice::ReadOnly);
    process.waitForFinished(-1);
    QString stdout=QString::fromLocal8Bit(process.readAllStandardOutput());
    QStringList pout=stdout.split("\n");
    
    if (pout[0]=="active"){
        return true;
    }else{
        return false;
    }


}

void LliurexOneDriveWidgetUtils::manageSync(){

    SYSTEMDTOKEN.setFileName(getUserHome()+"/.config/systemd/user/onedrive.service");
    QString cmd="";

    bool initClientStatus=isRunning();

    if (!initClientStatus){
        if (!SYSTEMDTOKEN.exists()){
            cmd="systemctl --user start onedrive.service";
            
        }else{
            cmd="/usr/bin/onedrive --monitor &";
        }
    }else{
        if (isSystemdActive()){
            cmd="systemctl --user stop onedrive.service";
        }else{
            cmd="ps -ef | grep '/usr/bin/onedrive --monitor' | grep -v grep | awk '{print $2}' | xargs kill -9";             
        }
    }
    KIO::CommandLauncherJob *job = nullptr;
    job = new KIO::CommandLauncherJob(cmd);
    job->start();
    
}

QString LliurexOneDriveWidgetUtils::formatFreeSpace(QString freespace){

    long int value=freespace.toLong();
    QString valueText=QLocale().formattedDataSize(value,2,QLocale::DataSizeTraditionalFormat);
    return valueText;
}

QStringList LliurexOneDriveWidgetUtils::getAccountStatus(int exitCode,QString poutProces,QString perrProcess){

    QString code="";
    QString freeSpace="";
    QStringList result;
    QString uploadingRef="416";
    QString zeroSpaceRef="zero space available";
    QString unauthorizedRef="Unauthorized";
    QString networkRef="Cannot connect to";
    QString allSyncRef="No pending";
    QString outSyncRef="out of sync";
    QString freeSpaceRef="Free Space";
    QString uploading="Uploading";

    
    QStringList pout=poutProces.split("\n");
    QStringList perror=perrProcess.split("\n");

    if (exitCode!=0){
        for(int i=0 ; i < pout.length() ; i++){
            if (perror[i].contains(uploadingRef)){
                code=UPLOADING_PENDING_CHANGES;
            }else if (perror[i].contains(zeroSpaceRef)){
                code=ZERO_SPACE_AVAILABLE;   
            }else if (perror[i].contains(unauthorizedRef)){
                code=UNAUTHORIZED_ERROR;
            }else if (perror[i].contains(networkRef)){ 
                code=NETWORK_CONNECT_ERROR;
            }else{
                code=GENERAL_ERROR;
            }
        }
    }else{ 
        for(int i=0 ; i < pout.length() ; i++){

            if (pout[i].contains(uploading)){
                code=UPLOADING_PENDING_CHANGES;
                break;
            }
            if (pout[i].contains(allSyncRef)){
                code=NO_PENDING_SYNC;
            }else if (pout[i].contains(outSyncRef)){
                code=OUT_OF_SYNC;
            }else if (pout[i].contains(freeSpaceRef)){
                QString tmp_value=pout[i].split(":")[1];
                freeSpace=formatFreeSpace(tmp_value);
            }
        }
    }
    

    result<<code<<freeSpace;
    return result;

}

bool LliurexOneDriveWidgetUtils::isOneDriveDisplayRunning(){

    QProcess process;
    QString cmd="ps -ef | grep '/usr/bin/onedrive --display-sync-status --verbose' | grep -v 'grep'";
    process.start("/bin/sh", QStringList()<< "-c" 
                       << cmd,QIODevice::ReadOnly);
    process.waitForFinished(-1);
    QString stdout=QString::fromLocal8Bit(process.readAllStandardOutput());
    QStringList pout=stdout.split("\n");
        
    if (pout[0].size()>0){
        return true;
    }else{
        return false;
    }
   
}

