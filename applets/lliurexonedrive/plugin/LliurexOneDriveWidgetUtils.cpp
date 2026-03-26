
#include "LliurexOneDriveWidgetUtils.h"
#include "LliurexOneDriveWidgetSpaceItem.h"
#include "LliurexOneDriveWidgetFileItem.h"

#include <QDateTime>
#include <QLocale>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPointer>
#include <QtConcurrent>
#include <QFutureWatcher>

struct SpaceTask {
    QJsonObject inputObj;
    QVariantMap result;
};

LliurexOneDriveWidgetUtils::LliurexOneDriveWidgetUtils(QObject *parent)
    : QObject(parent)
       
{
    qRegisterMetaType<QVector<LliurexOneDriveWidgetSpaceItem>>("QVector<LliurexOneDriveWidgetSpaceItem>");

}    

QString LliurexOneDriveWidgetUtils::getUserHome() {

    user=qgetenv("USER");
    return QStringLiteral("/home/") + user;

}

void LliurexOneDriveWidgetUtils::getSpacesInfo(QString onedriveConfigPath) {
    
    QPointer<LliurexOneDriveWidgetUtils> safeThis(this);

    QtConcurrent::run([safeThis, onedriveConfigPath]() {
        if (!safeThis) return;

        QFileInfo fileInfo(onedriveConfigPath);
        QDateTime lastMod = fileInfo.lastModified();

        if (lastMod > safeThis->m_lastJsonUpdate) {
            QFile tmpConfig(onedriveConfigPath);
            if (tmpConfig.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QJsonDocument doc = QJsonDocument::fromJson(tmpConfig.readAll());
                safeThis->m_cachedSpacesList = doc.object().value("spacesList").toArray();
                safeThis->m_lastJsonUpdate = lastMod;
                tmpConfig.close();
            }
        }
        
        QList<SpaceTask> tasks;
        for (const QJsonValue &val :  safeThis->m_cachedSpacesList) {
            QJsonObject obj = val.toObject();
            QString configPath = obj.value("configPath").toString();
            if (QFile::exists(configPath + "/refresh_token")) {
                tasks.append({obj, QVariantMap()});
            }
        }

        QtConcurrent::blockingMap(tasks, [safeThis](SpaceTask &task) {
            if (safeThis) {
                task.result = safeThis->processSingleSpace(task.inputObj);
            }
        });

        QMap<QString, QVariantMap> spacesInfo;
        QVector<LliurexOneDriveWidgetSpaceItem> spacesModel;
        spacesModel.reserve(tasks.size());
        
        QList<int> spacesStatusCode;
        int runningCount = 0, warningCount = 0, updateCount = 0;

        for (const SpaceTask &task : tasks) {
            const QVariantMap &res = task.result;
            if (res.isEmpty()) continue;

            QString id = res["id"].toString();
            int statusInt = res["statusInt"].toInt();
            bool isRunning = res["isRunning"].toBool();
            bool folderWarning = res["folderWarning"].toBool();
            bool updateReq = res["updateReq"].toBool();

            LliurexOneDriveWidgetSpaceItem item;
            item.setId(id);
            item.setName(res["name"].toString());
            item.setStatus(QString::number(statusInt));
            item.setIsRunning(isRunning);
            item.setLocalFolderWarning(folderWarning);
            item.setUpdateRequiredWarning(updateReq);
            item.setFilesPendingUpload(res["pendingUploads"].toString());

            spacesStatusCode.append(statusInt);
            if (isRunning) runningCount++;
            if (folderWarning) warningCount++;
            if (updateReq) updateCount++;

            spacesInfo.insert(id, res);
            spacesModel.append(std::move(item));
        }

        auto [status, errorCodes] = safeThis->getGlobalStatus(tasks.size(), spacesStatusCode);
        
        SpacesUpdateData updateInfo;
        updateInfo.info=spacesInfo;
        updateInfo.model=spacesModel;
        updateInfo.isRunning=(runningCount > 0);
        updateInfo.folderWarning = (warningCount > 0);
        updateInfo.updateRequired = (updateCount > 0);
        updateInfo.status = status;
        updateInfo.statusErrorCodes = errorCodes;
        emit safeThis->getSpacesInfoFinished(updateInfo);
    });
}

QVariantMap LliurexOneDriveWidgetUtils::processSingleSpace(const QJsonObject &obj)  {
    
    QVariantMap res;
    QString configPath = obj.value("configPath").toString();
    
    QStringList statusResult = this->readStatusToken(configPath);

    bool isRunning = this->checkIfSpaceSyncIsRunning(configPath);
    QList<bool> checkFolder =this->checkLocalFolder(configPath);
    bool updateReq = this->checkUpdateRequired(configPath);
    
    int statusInt = statusResult.value(1, "0").toInt();
    bool folderWarning = (checkFolder.value(0, false) || checkFolder.value(1, false));
    QString localFolder = obj.value("localFolder").toString();

    res["id"] = obj.value("id").toString();
    res["name"] = QFileInfo(localFolder).baseName();
    res["email"] = obj.value("email").toString();
    res["localFolder"] = localFolder;
    res["configPath"] = configPath;
    res["statusInt"] = statusInt;
    res["isRunning"] = isRunning;
    res["folderWarning"] = folderWarning;
    res["updateReq"] = updateReq;
    res["freeSpace"] = statusResult.value(2, "");
    res["pendingUploads"] = statusResult.value(4, "0");
    res["accountType"] = obj.value("accountType").toString();
    res["spaceType"] = obj.value("spaceType").toString();
    res["systemd"] = obj.value("systemd").toString();
    res["created"] = obj.value("created").toString();

    return res;
}

bool LliurexOneDriveWidgetUtils::checkIfSpaceSyncIsRunning(QString spaceConfigPath) {

   return QFile::exists(spaceConfigPath + "/.run/runToken");
    
}

QStringList LliurexOneDriveWidgetUtils::readStatusToken(QString spaceConfigPath) {

    QStringList result;

    QFile tokenFile(spaceConfigPath + "/.run/statusToken");

    if (tokenFile.exists() && tokenFile.open(QIODevice::ReadOnly)) {
        QTextStream content(&tokenFile);
        while (!content.atEnd()) {
            result.append(content.readLine().trimmed());
        }
        tokenFile.close();
    } else {
        result << "False" << "3" << "" << " " << "0";
    }
    return result;
}

QList<bool> LliurexOneDriveWidgetUtils::checkLocalFolder(QString spaceConfigPath){

    bool localFolderEmpty = QFile::exists(spaceConfigPath + "/.run/localFolderEmptyToken");
    bool localFolderRemoved = QFile::exists(spaceConfigPath + "/.run/localFolderRemovedToken");

    return {localFolderEmpty, localFolderRemoved};
}


QString LliurexOneDriveWidgetUtils::formatFreeSpace(QString freespace){

    long int value=freespace.toLong();
    QString valueText=QLocale().formattedDataSize(value,2,QLocale::DataSizeTraditionalFormat);
    return valueText;
}

QPair<QString, QList<int>> LliurexOneDriveWidgetUtils::getGlobalStatus(int totalSpaces,QList<int> spacesStatusCode){

    int warningCount = 0;
    int errorCount = 0;
    int generalErrorCount = 0;
    int okCount = 0;
    QList<int> spacesStatusErrorCode;

    for (int code : spacesStatusCode) {
        if (warningCode.contains(code)) {
            warningCount++;
        } else if (errorCode.contains(code)) {
            if (code == MICROSOFT_API_ERROR || code == UNABLE_CONNECT_MICROSOFT_ERROR) {
                generalErrorCount++;
            } else {
                errorCount++;
            }
            if (!spacesStatusErrorCode.contains(code)) {
                spacesStatusErrorCode.append(code);
            }
        } else {
            okCount++;
        }
    }

    QString status;
    if (okCount == totalSpaces && totalSpaces > 0) {
        status = "OK";
        spacesStatusErrorCode.clear();
    } else if (errorCount > 0) {
        status = "Error";
    } else if (generalErrorCount > 0) {
        status = "GeneralError";
    } else {
        status = "Warning";
        spacesStatusErrorCode.clear();
    }

    return {status, spacesStatusErrorCode};

}

QVector<LliurexOneDriveWidgetFileItem> LliurexOneDriveWidgetUtils::getFiles(const QString& spaceLocalFolder, int limit) {
   
    QVector<LliurexOneDriveWidgetFileItem> items;
    if (spaceLocalFolder.isEmpty() || !QDir(spaceLocalFolder).exists()) return items;

    QProcess process;
    
    QStringList args;
    args << spaceLocalFolder 
         << "-not" << "-path" << "*/.*" 
         << "-type" << "f" 
         << "-not" << "-name" << ".directory"
         << "-printf" << "%T@ %p\n";

    process.start(QStringLiteral("find"), args);
    
    if (!process.waitForFinished(5000)) {
        process.kill();
        return items;
    }

    QByteArray output = process.readAllStandardOutput();
    QList<QByteArray> lines = output.split('\n');

    std::sort(lines.begin(), lines.end(), [](const QByteArray &a, const QByteArray &b) {
        return a > b; 
    });

    int count = 0;
    QDir baseDir(spaceLocalFolder);

    for (const QByteArray &line : lines) {
        if (line.isEmpty() || count >= limit) continue;

        int firstSpace = line.indexOf(' ');
        if (firstSpace == -1) continue;

        double timestamp = line.left(firstSpace).toDouble();
        QString filePath = QString::fromUtf8(line.mid(firstSpace + 1));
        
        QFileInfo info(filePath);
        if (!info.exists()) continue;

        QDateTime lastMod = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(timestamp));

        LliurexOneDriveWidgetFileItem item;

        item.setFileName(baseDir.relativeFilePath(info.absoluteFilePath()));
        item.setFilePath(info.absoluteFilePath());
        item.setFileDate(lastMod.date().toString(Qt::ISODate));
        item.setFileTime(lastMod.time().toString(QStringLiteral("HH:mm:ss")));
        item.setFileId(info.absoluteFilePath() + QLatin1Char('_') + QString::number(lastMod.toSecsSinceEpoch()));

        items.append(std::move(item));
        count++;
    }

    return items;
}

void LliurexOneDriveWidgetUtils::fetchUploadedFiles(const QString &unit, const QString &since) {
 
    if (m_process) {
        m_process->disconnect(); 
        if (m_process->state() != QProcess::NotRunning) {
            m_process->kill(); 
            m_process->waitForFinished(500); 
        }
        m_process->deleteLater(); 

    }

    m_process = new QProcess(this);
    
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), 
            this, &LliurexOneDriveWidgetUtils::onProcessFinished);

    QString cmd = QString("journalctl --user-unit=%1 -r -o short-full --since '%2' | grep 'Uploading' | grep '...done' | head -30")
                  .arg(unit, since);

    m_process->start("/bin/sh", {"-c", cmd});
}

void LliurexOneDriveWidgetUtils::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    
    QByteArray data = m_process->readAllStandardOutput();

    QMetaObject::invokeMethod(this, [this, data]() {
        auto items = parseData(data);
        emit fetchUploadedFilesFinished(items);
    }, Qt::QueuedConnection);
}

QVector<LliurexOneDriveWidgetFileItem> LliurexOneDriveWidgetUtils::parseData(const QByteArray &data) {    
    
    QVector<LliurexOneDriveWidgetFileItem> items;
    QString stdoutStr = QString::fromLocal8Bit(data);
    QStringList lines = stdoutStr.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        
        if (!line.contains("Uploading") || !line.contains(" ... done")) continue;

        QString timePart = line.section(' ', 1, 2); 
        QString namePart = line.section("Uploading ", 1, 1).section(" ... done", 0, 0);
        
        QString relativePath = namePart;
        if (namePart.contains("LLIUREX_ONEDRIVE_BACKUP/")) {
            relativePath = namePart.split("LLIUREX_ONEDRIVE_BACKUP/").last();
        }

        LliurexOneDriveWidgetFileItem item;
        item.setFileName(relativePath);
        item.setFilePath(relativePath);
        
        QString dateStr = timePart.section(' ', 0, 0);
        item.setFileDate(LliurexOneDriveWidgetUtils::formatFileDate(dateStr));
        item.setFileTime(timePart.section(' ', 1, 1));
        
        item.setFileId(QString("%1_%2").arg(relativePath, timePart));
        items.append(item);
    }
    return items;
}

QString LliurexOneDriveWidgetUtils::formatFileDate(QString fileDate){

    QDate myDate = QDate::fromString(fileDate, Qt::ISODate);
    
    if (!myDate.isValid()) {
        return fileDate; 
    }
    
    return myDate.toString("dd/MM/yyyy");

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

bool LliurexOneDriveWidgetUtils::checkUpdateRequired(QString spaceConfigPath) {

    QList<bool> result;
    bool updateRequired=false;

    QDir spaceConfigFolder(spaceConfigPath);
    updateRequiredToken.setFileName(spaceConfigPath+"/.run/updateRequiredToken");

    if (spaceConfigFolder.exists()){
        if (updateRequiredToken.exists()){
            updateRequired=true;
        }
     }
    return updateRequired;
}
