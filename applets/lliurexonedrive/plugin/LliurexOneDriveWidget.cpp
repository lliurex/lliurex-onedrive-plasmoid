
#include <iostream>
#include "LliurexOneDriveWidget.h"
#include "LliurexOneDriveWidgetItem.h"
#include "LliurexOneDriveWidgetModel.h"
#include "LliurexOneDriveWidgetSpacesModel.h"
#include "LliurexOneDriveWidgetSpaceItem.h"
#include "LliurexOneDriveWidgetUtils.h"

#include <KLocalizedString>
#include <KFormat>
#include <KNotification>
#include <KIO/CommandLauncherJob>
#include <QTimer>
#include <QStandardPaths>
#include <QDebug>
#include <QFile>
#include <QTextStream>


LliurexOneDriveWidget::LliurexOneDriveWidget(QObject *parent)
    : QObject(parent)
    ,m_timer(new QTimer(this))
    ,m_utils(new LliurexOneDriveWidgetUtils(this))
    ,m_spacesModel(new LliurexOneDriveWidgetSpacesModel(this))
    ,m_getLatestFiles(new QProcess(this))
    ,m_model(new LliurexOneDriveWidgetModel(this))
   
{
    
    userHome=m_utils->getUserHome();
    TARGET_FILE.setFileName(userHome+"/.config/lliurex-onedrive-config/onedriveConfig.json");
   
    //initPlasmoid();

    connect(m_timer, &QTimer::timeout, this, &LliurexOneDriveWidget::worker);
    connect(m_getLatestFiles, (void (QProcess::*)(int, QProcess::ExitStatus))&QProcess::finished,
            this, &LliurexOneDriveWidget::getLatestFilesFinished);

    m_timer->start(5000);
    worker();
}    

void LliurexOneDriveWidget::worker(){

    bool isPassiveStatus=true;

    if (!isWorking){
        if (LliurexOneDriveWidget::TARGET_FILE.exists() ) {
            isWorking=true;
            m_spacesModel->clear();
            QVariantList spacesInfo;
            QString onedriveConfigPath=userHome+"/.config/lliurex-onedrive-config/onedriveConfig.json";
            spacesInfo=m_utils->getSpacesInfo(onedriveConfigPath);
            
            if (spacesInfo.length()>0){
                QVector<LliurexOneDriveWidgetSpaceItem> items;
                for (QVariantList::iterator j=spacesInfo.begin();j!=spacesInfo.end();j++){
                    QVariantList tmpList=(*j).toList();
                    LliurexOneDriveWidgetSpaceItem item;
                    item.setId(tmpList[0].toString());
                    item.setName(tmpList[1].toString());
                    item.setStatus(tmpList[2].toString());
                    item.setIsRunning(tmpList[3].toBool());
                    item.setLocalFolderWarning(tmpList[4].toBool());
                    items.append(item);
            
                }
                m_spacesModel->updateItems(items);
                isPassiveStatus=false;

            }else{
                m_spacesModel->clear();
            }

        }
        if (!isPassiveStatus){
            const QString tooltip(i18n("Lliurex OneDrive"));
            setToolTip(tooltip);
            checkIfStartIsLocked();
            setStatus(ActiveStatus);
        }else{
            setStatus(PassiveStatus);
            previousError=false;
            previousStatusError.clear();
            warning=false;
            isWorking=false;
        }
    
    }

}

void LliurexOneDriveWidget::checkIfStartIsLocked(){

    QString subtooltip="";
    checkIsRunning();
    if (m_utils->isLocalFolderWarning){
        warning=false;
        previousError=false;
        checkExecuted=false;
        subtooltip=i18n("Synchronization of one or more spaces is stopped. Detected problems with local folder");
        updateWidget(subtooltip,"onedrive-error");
        if (showStartLockMessage){
            m_errorNotification = KNotification::event(QStringLiteral("ErrorStatus"), subtooltip, {}, "onedrive-widget", nullptr, KNotification::CloseOnTimeout , QStringLiteral("llxonedrive"));
            QString name = i18n("Open Lliurex OneDrive");
            m_errorNotification->setDefaultAction(name);
            m_errorNotification->setActions({name});
            connect(m_errorNotification, QOverload<unsigned int>::of(&KNotification::activated), this, &LliurexOneDriveWidget::launchOneDrive);
            showStartLockMessage=false;
        }
    }else{
        showStartLockMessage=true;
        isWorking=false;
    }

}

void LliurexOneDriveWidget::checkIsRunning(){

    if (!m_utils->isLocalFolderWarning){
        if (m_utils->areSpacesSyncRunning){
            if ((!previousError) & (!warning) & (!checkExecuted)){
                QString subtooltip(i18n("Starting the synchronization"));
                updateWidget(subtooltip,"onedrive-starting");
            }
            checkStatus();
        }else{
            QString subtooltip(i18n("Synchronization is stopped"));
            updateWidget(subtooltip,"onedrive-pause");
            previousError=false;
            previousStatusError.clear();
            warning=false;
            checkExecuted=false;
            isWorking=false;
        }              
    }
}

void LliurexOneDriveWidget::checkStatus(){

    lastCheck=lastCheck+5;
    if (lastCheck>90){
        bool showNotification=false;
        QString msgError;
        bool processError=true;

        QString globalStatus=m_utils->getGlobalStatus();

        if (globalStatus=="OK"){
            QString subtooltip(i18n("All remote changes are synchronized"));
            updateWidget(subtooltip,"onedrive-running");
            warning=false;
            previousError=false;
            previousStatusError.clear();
            countRepeatGeneralError=0;

        }else if (globalStatus=="Warning"){
            warning=true;
            QString subtooltip(i18n("One or more spaces have changes pending of synchronization"));
            updateWidget(subtooltip,"onedrive-waiting");
            previousError=false;
            previousStatusError.clear();
            countRepeatGeneralError=0;
        }else{
            warning=false;
            if (globalStatus=="GeneralError"){
                countRepeatGeneralError+=1;
                msgError=i18n("Unable to connect with Microsoft OneDrive");
                if (countRepeatGeneralError<2){
                    processError=false;
                }
            }else if (globalStatus=="Error"){
                msgError=i18n("OneDrive has reported an error in one or more spaces");
                countRepeatGeneralError=0;
  
            }

            if(processError){
;               QString subtooltip(msgError);
                updateWidget(subtooltip,"onedrive-error");
                
                if (previousError){
                    if (previousStatusError!=m_utils->spacesStatusErrorCode){
                        showNotification=true;
                        previousStatusError=m_utils->spacesStatusErrorCode;
                    }
                }else{
                    previousError=true;
                    previousStatusError=m_utils->spacesStatusErrorCode;
                    showNotification=true;
                }
                if (showNotification){
                    m_errorNotification = KNotification::event(QStringLiteral("ErrorStatus"), subtooltip, {}, "onedrive-widget", nullptr, KNotification::CloseOnTimeout , QStringLiteral("llxonedrive"));
                    QString name = i18n("Open Lliurex OneDrive");
                    m_errorNotification->setDefaultAction(name);
                    m_errorNotification->setActions({name});
                    connect(m_errorNotification, QOverload<unsigned int>::of(&KNotification::activated), this, &LliurexOneDriveWidget::launchOneDrive);
                }
            }
  
        }
        isWorking=false;
        lastCheck=0;
        checkExecuted=true;
    }
}

void LliurexOneDriveWidget::updateWidget(QString subtooltip,QString icon){

    setSubToolTip(subtooltip);
    setIconName(icon);
}   


LliurexOneDriveWidget::TrayStatus LliurexOneDriveWidget::status() const
{
    return m_status;
}


void LliurexOneDriveWidget::launchOneDrive()
{
    KIO::CommandLauncherJob *job = nullptr;
    job = new KIO::CommandLauncherJob(QStringLiteral("lliurex-onedrive"));
    job->start();

    if (m_errorNotification) { m_errorNotification->close(); }
   
}

void LliurexOneDriveWidget::setStatus(LliurexOneDriveWidget::TrayStatus status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}

QString LliurexOneDriveWidget::iconName() const
{
    return m_iconName;
}

void LliurexOneDriveWidget::setIconName(const QString &name)
{
    if (m_iconName != name) {
        m_iconName = name;
        emit iconNameChanged();
    }
}

QString LliurexOneDriveWidget::toolTip() const
{
    return m_toolTip;
}

void LliurexOneDriveWidget::setToolTip(const QString &toolTip)
{
    if (m_toolTip != toolTip) {
        m_toolTip = toolTip;
        emit toolTipChanged();
    }
}

QString LliurexOneDriveWidget::subToolTip() const
{
    return m_subToolTip;
}

void LliurexOneDriveWidget::setSubToolTip(const QString &subToolTip)
{
    if (m_subToolTip != subToolTip) {
        m_subToolTip = subToolTip;
        emit subToolTipChanged();
    }
}

void LliurexOneDriveWidget::openHelp()
{
            
    QString command="xdg-open https://wiki.edu.gva.es/lliurex/tiki-index.php?page=Lliurex-Onedrive";
    KIO::CommandLauncherJob *job = nullptr;
    job = new KIO::CommandLauncherJob(command);
    job->start();
}

LliurexOneDriveWidgetSpacesModel *LliurexOneDriveWidget::spacesModel() const
{
    return m_spacesModel;
}

void LliurexOneDriveWidget::goToSpace(QString const &idSpace ){

    spaceConfigPath="";
    spaceLocalFolder="";
    spaceSystemd="";

    QStringList spaceInfo=m_utils->readSpaceInfo(idSpace);
    if (spaceInfo.length()>0){
        setSpaceMail(spaceInfo[0]);
        setSpaceType(spaceInfo[1]);
        setSpaceSharePoint(spaceInfo[2]);
        setSpaceLibrary(spaceInfo[3]);
        spaceLocalFolder=spaceInfo[4];
        QFileInfo fi(spaceLocalFolder);
        setOneDriveFolder(fi.baseName());
        spaceConfigPath=spaceInfo[5];
        spaceSystemd=spaceInfo[6];
        getSpaceStatusDetails();
   }

}

void LliurexOneDriveWidget::getSpaceStatusDetails(){

    QFile spaceTokenStatus;
    spaceTokenStatus.setFileName(spaceConfigPath+"/.statusToken");
    if (spaceTokenStatus.exists()){
        QStringList spaceDetails=m_utils->readStatusToken(spaceConfigPath);
        QString freeSpace=m_utils->formatFreeSpace(spaceDetails[2]);
        setFreeSpace(freeSpace);
        setSyncStatus(true);
        setLliurexOneDriveOpen(false);
    }else{
        setFreeSpace("");
        setSyncStatus(false);
    }
    setCurrentIndex(1);

}

void LliurexOneDriveWidget::manageNavigation(int stackIndex){

    setCurrentIndex(stackIndex);

}

void LliurexOneDriveWidget::openFolder()
{
            
    QString command="xdg-open "+spaceLocalFolder;
    KIO::CommandLauncherJob *job = nullptr;
    job = new KIO::CommandLauncherJob(command);
    job->start();
}

void LliurexOneDriveWidget::manageSync(){

    qDebug()<<"Cambiando estado";
}

void LliurexOneDriveWidget::getLatestFiles(){

    if (m_getLatestFiles->state() != QProcess::NotRunning) {
        m_getLatestFiles->kill();
    }
    m_model->clear();
    setShowSearchFiles(true);
    QString cmd="find "+spaceLocalFolder+" -type f -mmin +1 -printf '%T+\t%s\t%p\n' 2>/dev/null | sort -r | more";
    m_getLatestFiles->start("/bin/sh", QStringList()<< "-c" 
                       << cmd,QIODevice::ReadOnly);
}

void LliurexOneDriveWidget::getLatestFilesFinished(int exitCode, QProcess::ExitStatus exitStatus){

    Q_UNUSED(exitCode);
    QList<QStringList> latestFiles;
    QStringList searchFilesPout;

    if (exitStatus!=QProcess::NormalExit){
        m_model->clear();
        setShowSearchFiles(false);     
        return;
    }

    QString stdout=QString::fromLocal8Bit(m_getLatestFiles->readAllStandardOutput());
    
    if (stdout!=""){
        searchFilesPout=stdout.split("\n");
        if (searchFilesPout.size()>0){
            QVector<LliurexOneDriveWidgetItem> items;
            latestFiles=m_utils->getFiles(searchFilesPout,spaceLocalFolder);
            
            for (int i=0;i<latestFiles.length();i++){
                LliurexOneDriveWidgetItem item;
                item.setFileName(latestFiles[i][0]);
                item.setFilePath(latestFiles[i][1]);
                item.setFileDate(latestFiles[i][2]);
                item.setFileTime(latestFiles[i][3]);
                items.append(item);
                
            }
            m_model->updateItems(items);

        }else{
            m_model->clear();

        }
    }else{
        m_model->clear();
    }
    setShowSearchFiles(false);
}

void LliurexOneDriveWidget::goToFile(const QString &filePath)
{
            
    QString command="dolphin --select "+filePath;
    KIO::CommandLauncherJob *job = nullptr;
    job = new KIO::CommandLauncherJob(command);
    job->start();
}

bool LliurexOneDriveWidget::checkIfFileExists(const QString &filePath)
{
    recentFile.setFileName(filePath);
    
    if (recentFile.exists()){
        return true;
    }else{
        return false;
    }
}

int LliurexOneDriveWidget::currentIndex()
{
    return m_currentIndex;
}

void LliurexOneDriveWidget::setCurrentIndex(int currentIndex)
{
    if (m_currentIndex != currentIndex) {
        m_currentIndex = currentIndex;
        emit currentIndexChanged();
    }
}

QString LliurexOneDriveWidget::spaceMail() const
{
    return m_spaceMail;
}

void LliurexOneDriveWidget::setSpaceMail(const QString &spaceMail)
{
    if (m_spaceMail != spaceMail) {
        m_spaceMail = spaceMail;
        emit spaceMailChanged();
    }
}

QString LliurexOneDriveWidget::spaceType() const
{
    return m_spaceType;
}

void LliurexOneDriveWidget::setSpaceType(const QString &spaceType)
{
    if (m_spaceType != spaceType) {
        m_spaceType = spaceType;
        emit spaceTypeChanged();
    }
}

QString LliurexOneDriveWidget::spaceSharePoint() const
{
    return m_spaceSharePoint;
}

void LliurexOneDriveWidget::setSpaceSharePoint(const QString &spaceSharePoint)
{
    if (m_spaceSharePoint != spaceSharePoint) {
        m_spaceSharePoint = spaceSharePoint;
        emit spaceSharePointChanged();
    }
}

QString LliurexOneDriveWidget::spaceLibrary() const
{
    return m_spaceLibrary;
}

void LliurexOneDriveWidget::setSpaceLibrary(const QString &spaceLibrary)
{
    if (m_spaceLibrary != spaceLibrary) {
        m_spaceLibrary = spaceLibrary;
        emit spaceLibraryChanged();
    }
}


QString LliurexOneDriveWidget::oneDriveFolder() const
{
    return m_oneDriveFolder;
}

void LliurexOneDriveWidget::setOneDriveFolder(const QString &oneDriveFolder)
{
    if (m_oneDriveFolder != oneDriveFolder) {
        m_oneDriveFolder = oneDriveFolder;
        emit oneDriveFolderChanged();
    }
}

QString LliurexOneDriveWidget::freeSpace() const
{
    return m_freeSpace;
}

void LliurexOneDriveWidget::setFreeSpace(const QString &freeSpace)
{
    if (m_freeSpace != freeSpace) {
        m_freeSpace = freeSpace;
        emit freeSpaceChanged();
    }
}

bool LliurexOneDriveWidget::syncStatus()
{
    return m_syncStatus;
}

void LliurexOneDriveWidget::setSyncStatus(bool syncStatus)
{
    if (m_syncStatus != syncStatus) {
        m_syncStatus = syncStatus;
        emit syncStatusChanged();
    }
}


bool LliurexOneDriveWidget::lliurexOneDriveOpen()
{
    return m_lliurexOneDriveOpen;
}

void LliurexOneDriveWidget::setLliurexOneDriveOpen(bool lliurexOneDriveOpen)
{
    if (m_lliurexOneDriveOpen != lliurexOneDriveOpen) {
        m_lliurexOneDriveOpen = lliurexOneDriveOpen;
        emit lliurexOneDriveOpenChanged();
    }
}


bool LliurexOneDriveWidget::showSearchFiles()
{
    return m_showSearchFiles;
}

void LliurexOneDriveWidget::setShowSearchFiles(bool showSearchFiles)
{
    if (m_showSearchFiles != showSearchFiles) {
        m_showSearchFiles = showSearchFiles;
        emit showSearchFilesChanged();
    }
}

LliurexOneDriveWidgetModel *LliurexOneDriveWidget::model() const
{
    return m_model;
}