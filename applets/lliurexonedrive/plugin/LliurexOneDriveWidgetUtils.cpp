
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
#include <QDebug>
#include <KLocalizedString>
#include <QDate>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

LliurexOneDriveWidgetUtils::LliurexOneDriveWidgetUtils(QObject *parent)
    : QObject(parent)
       
{
}    

QString LliurexOneDriveWidgetUtils::getUserHome() {

    QString user=qgetenv("USER");
    QString userHome="/home/"+user;

    return userHome;

}
bool LliurexOneDriveWidgetUtils::checkIfSpaceSyncIsRunning(QString spaceConfigPath){

    QFile statusToken;
    statusToken.setFileName(spaceConfigPath+"/.statusToken");

    if (statusToken.exists()){
        return true;
    }else{
        return false;
    }
}

QStringList LliurexOneDriveWidgetUtils::readStatusToken(QString spaceConfigPath){

    QStringList result;

    spaceStatusToken.setFileName(spaceConfigPath+"/.statusToken");

    if (spaceStatusToken.exists()){
        if (spaceStatusToken.open(QIODevice::ReadOnly)){
            QTextStream content(&spaceStatusToken);
            while (!content.atEnd()){
                result.append(content.readLine());
            }
            spaceStatusToken.close();
        }

    }else{
        result.append("False");
        result.append("0");
        result.append("");
    }
    return result;

}

QList<bool> LliurexOneDriveWidgetUtils::checkLocalFolder(QString spaceConfigPath){

    QList<bool> result;
    bool localFolderEmpty=false;
    bool localFolderRemoved=false;

    QDir spaceConfigFolder(spaceConfigPath);
    localFolderEmptyToken.setFileName(spaceConfigPath+"/.localFolderEmptyToken");
    localFolderRemovedToken.setFileName(spaceConfigPath+"/.localFolderRemovedToken");

    if (spaceConfigFolder.exists()){
        if (localFolderEmptyToken.exists()){
            localFolderEmpty=true;
        }
        if (localFolderRemovedToken.exists()){
            localFolderRemoved=true;
        }
    }
    result.append(localFolderEmpty);
    result.append(localFolderRemoved);

    return result;

}


QVariantList LliurexOneDriveWidgetUtils::getSpacesInfo(QString onedriveConfigPath){

    QVariantList spacesInfo;
    int localFolderWarningCount=0;
    int areSpacesSyncRunningCount=0;
    totalSpaces=0;

    QFile tmpConfig;
    spacesStatusCode.clear();

    tmpConfig.setFileName(onedriveConfigPath);
    tmpConfig.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray contentFile=tmpConfig.readAll();
    tmpConfig.close();

    QJsonParseError err;
    auto doc=QJsonDocument::fromJson(contentFile,&err);
    if (err.error != QJsonParseError::NoError){
        qDebug()<<err.errorString();
    }

    QJsonObject obj=doc.object();
    onedriveConfig=obj.value("spacesList").toArray();


    foreach(const QJsonValue &val, onedriveConfig){
        QString tmpTokenPath=val.toObject().value("configPath").toString()+"/refresh_token";
        QFile tmpTokenFile;
        tmpTokenFile.setFileName(tmpTokenPath);
        if (tmpTokenFile.exists()){
            totalSpaces+=1;
            QVariantList tmpItem;
            tmpItem.append(val.toObject().value("id").toString());
            QFileInfo fi(val.toObject().value("localFolder").toString());
            tmpItem.append(fi.baseName());
            QString spaceConfigPath=val.toObject().value("configPath").toString();
            QStringList statusResult=readStatusToken(spaceConfigPath);
            tmpItem.append(statusResult[1].toInt());
            spacesStatusCode.append(statusResult[1].toInt());
            bool isSpaceRunning=checkIfSpaceSyncIsRunning(spaceConfigPath);
            if (isSpaceRunning){
                areSpacesSyncRunningCount+=1;
            }
            tmpItem.append(isSpaceRunning);
            QList<bool>checkFolder=checkLocalFolder(spaceConfigPath);
            if ((!checkFolder[0])&&(!checkFolder[1])){
                tmpItem.append(false);
            }else{
                tmpItem.append(true);
                localFolderWarningCount+=1;
            }
            spacesInfo.push_back(tmpItem);
        }
    }

    if (areSpacesSyncRunningCount>0){
        areSpacesSyncRunning=true;
    }else{
        areSpacesSyncRunning=false;
    }

    if (localFolderWarningCount>0){
        isLocalFolderWarning=true;
    }else{
        isLocalFolderWarning=false;
    }
    return spacesInfo;

}

QString LliurexOneDriveWidgetUtils::getGlobalStatus(){

    int warningCount=0;
    int errorCount=0;
    int generalErrorCount=0;
    int okCount=0;
    bool generalError=false;

    for (int i=0;i<spacesStatusCode.length();i++){
        if (warningCode.contains(spacesStatusCode[i])){
            warningCount+=1;
        }else if (errorCode.contains(spacesStatusCode[i])){
            if ((spacesStatusCode[i]==MICROSOFT_API_ERROR)||(spacesStatusCode[i]==UNABLE_CONNECT_MICROSOFT_ERROR)){
                generalErrorCount+=1;
            }else{
                errorCount+=1;
            }
            if (!spacesStatusErrorCode.contains(spacesStatusCode[i])){
                spacesStatusErrorCode.append(spacesStatusCode[i]);
            }
        }else{
            okCount+=1;
        }
    }    
    if (okCount==totalSpaces){
        spacesStatusErrorCode.clear();
        return "OK";
    }else if (errorCount>0){
        return "Error";
    }else if (generalErrorCount>0){
        return "GeneralError";
    }else{
        spacesStatusErrorCode.clear();
        return "Warning";
    }

}



