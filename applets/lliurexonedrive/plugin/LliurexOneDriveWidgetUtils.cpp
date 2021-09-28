
#include <iostream>
#include <string>
#include "LliurexOneDriveWidgetUtils.h"
#include <QLocale>

#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStandardPaths>
#include <sys/types.h>
#include <KRun>


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
                       << cmd);
    process.waitForFinished(-1);
    QString stdout=process.readAllStandardOutput();
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
    process.start("/bin/sh",QStringList()<<"-c"<<cmd);
    process.waitForFinished(-1);
    QString stdout=process.readAllStandardOutput();
    QStringList pout=stdout.split("\n");
    
    if (pout[0]=="active"){
        return true;
    }else{
        return false;
    }


}

bool LliurexOneDriveWidgetUtils::manageSync(){

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
    KRun::runCommand(cmd, nullptr);
    
    if (initClientStatus!=isRunning()){
        return true;
    }else{
        return false;
    }
    
    
}

QString LliurexOneDriveWidgetUtils::formatFreeSpace(QString freespace){

    long int value=freespace.toLong();
    QString valueText=QLocale().formattedDataSize(value,2,QLocale::DataSizeTraditionalFormat);
    return valueText;
}

QStringList LliurexOneDriveWidgetUtils::getAccountStatus(){

    QString code="";
    QString freeSpace="Not available";
    QStringList result;

    QProcess process;
    process.start("/usr/bin/onedrive --display-sync-status --verbose");
    process.waitForFinished(-1);
    int exitCode=process.exitCode();
    QString stdout=process.readAllStandardOutput();
    QString stderr=process.readAllStandardError();
    QStringList pout=stdout.split("\n");
    QStringList perror=stderr.split("\n");

    if (exitCode!=0){
        for(int i=0 ; i < pout.length() ; i++){
            if (perror[i].contains('416')){
                code=UPLOADING_PENDING_CHANGES;
            }else if (perror[i].contains('zero space available')){
                code=ZERO_SPACE_AVAILABLE;   
            }else if (perror[i].contains('Unauthorized')){
                code=UNAUTHORIZED_ERROR;
            }else if (perror[i].contains('Cannot connect to')){ 
                code=NETWORK_CONNECT_ERROR;
            }else{
                code=GENERAL_ERROR;
            }
        }
    }else{ 
        for(int i=0 ; i < pout.length() ; i++){
            if (pout[i].contains("No pending")){
                code=NO_PENDING_SYNC;
            }else if (pout[i].contains("out of sync")){
                code=OUT_OF_SYNC;
            }else if (pout[i].contains("Free Space")){
                QString tmp_value=pout[i].split(":")[1];
                freeSpace=formatFreeSpace(tmp_value);
                     
            }
        }
    }
    

    result<<code<<freeSpace;
    return result;

}

