
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

    
    spaceRunToken.setFileName(spaceConfigPath+"/.run/runToken");

    if (spaceRunToken.exists()){
        return true;
    }else{
        return false;
    }
    
}

QStringList LliurexOneDriveWidgetUtils::readStatusToken(QString spaceConfigPath){

    QStringList result;

    spaceStatusToken.setFileName(spaceConfigPath+"/.run/statusToken");

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
        result.append("3");
        result.append("");
    }
    return result;

}

QList<bool> LliurexOneDriveWidgetUtils::checkLocalFolder(QString spaceConfigPath){

    QList<bool> result;
    bool localFolderEmpty=false;
    bool localFolderRemoved=false;

    QDir spaceConfigFolder(spaceConfigPath);
    localFolderEmptyToken.setFileName(spaceConfigPath+"/.run/localFolderEmptyToken");
    localFolderRemovedToken.setFileName(spaceConfigPath+"/.run/localFolderRemovedToken");

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
            tmpItem.append(val.toObject().value("email").toString());
            tmpItem.append(val.toObject().value("accountType").toString());
            tmpItem.append(val.toObject().value("spaceType").toString());
            tmpItem.append(val.toObject().value("localFolder").toString());
            QFileInfo fi(val.toObject().value("localFolder").toString());
            tmpItem.append(fi.baseName());
            QString spaceConfigPath=val.toObject().value("configPath").toString();
            tmpItem.append(spaceConfigPath);
            tmpItem.append(val.toObject().value("systemd").toString());
            QStringList statusResult=readStatusToken(spaceConfigPath);
            tmpItem.append(statusResult[1].toInt());
            spacesStatusCode.append(statusResult[1].toInt());
            bool isSpaceRunning=checkIfSpaceSyncIsRunning(spaceConfigPath);
            if (isSpaceRunning){
                areSpacesSyncRunningCount+=1;
            }
            QString spaceFreeSpace=statusResult[2];
            if (spaceFreeSpace!=""){
                tmpItem.append(formatFreeSpace(spaceFreeSpace));
            }else{
                tmpItem.append(spaceFreeSpace);
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

QString LliurexOneDriveWidgetUtils::formatFreeSpace(QString freespace){

    long int value=freespace.toLong();
    QString valueText=QLocale().formattedDataSize(value,2,QLocale::DataSizeTraditionalFormat);
    return valueText;
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

QList<QStringList> LliurexOneDriveWidgetUtils::getFiles(QStringList info, QString spaceLocalFolder){

    QList<QStringList> lastestFiles;

    QString reference=spaceLocalFolder;

    info.removeLast();
    if (!info.isEmpty()){
        for (int i=0;i<10;i++){
            if (i<info.length()){
                QStringList tmpLine=info[i].split(reference)[1].split("/");
                int tmpRef=tmpLine.length()-1;
                QStringList tmpItem;
                if (tmpLine[tmpRef]!=".directory"){
                    tmpItem.append(tmpLine[tmpRef]);
                    tmpItem.append(info[i].split("\t")[2]);
                    QStringList tmpDate;
                    tmpDate=info[i].split("\t")[0].split("+");
                    tmpItem.append(formatFileDate(tmpDate[0]));
                    tmpItem.append(tmpDate[1].split(".")[0]);
                    lastestFiles.append(tmpItem);
                }
            }else{
                break;
            }
        }

        if ((lastestFiles.length()<10) && (info.length()>10)){
            int j=10;
            if (j<info.length()){
                QStringList tmpLine=info[j].split(reference)[1].split("/");
                int tmpRef=tmpLine.length()-1;
                QStringList tmpItem;
                tmpItem.append(tmpLine[tmpRef]);
                tmpItem.append(info[j].split("\t")[2]);
                QStringList tmpDate;
                tmpDate=info[j].split("\t")[0].split("+");
                tmpItem.append(formatFileDate(tmpDate[0]));
                tmpItem.append(tmpDate[1].split(".")[0]);
                lastestFiles.append(tmpItem);          
            }

        }
    }

    return lastestFiles;

}

QString LliurexOneDriveWidgetUtils::formatFileDate(QString fileDate){

    QDate myDate;
    myDate=QDate::fromString(fileDate,"yyyy-MM-dd");
    QString formatDate;
    formatDate=myDate.toString("dd/MM/yyyy");

    return formatDate;

}

void LliurexOneDriveWidgetUtils::restoreSyncListFile(QString spaceConfigPath)
{
    
    syncList.setFileName(spaceConfigPath+"/sync_list.back");
    syncListHash.setFileName(spaceConfigPath+"/.sync_list.hash.back");

    if (syncList.exists()){
        syncList.rename(spaceConfigPath+"/sync_list");
    }

    if (syncListHash.exists()){
        syncListHash.rename(spaceConfigPath+"/.sync_list.hash");
    }

}

QString LliurexOneDriveWidgetUtils::checkLocalFreeSpace(){

    freeSpaceWarningToken.setFileName(getUserHome()+"/.config/lliurex-onedrive-config/.run/hddWarningToken");
    freeSpaceErrorToken.setFileName(getUserHome()+"/.config/lliurex-onedrive-config/.run/hddErrorToken");

    if (freeSpaceWarningToken.exists()){
        return "HDD_Warning";
    }else if (freeSpaceErrorToken.exists()){
        return "HDD_Error";
    }else{
        return "HDD_OK";
    }

}

QString LliurexOneDriveWidgetUtils::getLogFileSize(QString logFileSize){

    logFile.setFileName(logFileSize);
    QString logSize=QString::number(logFile.size());
    if (logFile.exists()){
        return formatFreeSpace(logSize);
    }else{
        return "";
    }

}


