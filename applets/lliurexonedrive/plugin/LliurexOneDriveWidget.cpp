
#include <iostream>
#include "LliurexOneDriveWidget.h"
#include "LliurexOneDriveWidgetFileItem.h"
#include "LliurexOneDriveWidgetFilesModel.h"
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
    ,m_isLliurexOneDriveOpen(new QProcess(this))
    ,m_getLatestFiles(new QProcess(this))
    ,m_filesModel(new LliurexOneDriveWidgetFilesModel(this))
   
{
    
    userHome=m_utils->getUserHome();
    TARGET_FILE.setFileName(userHome+"/.config/lliurex-onedrive-config/onedriveConfig.json");
    OLD_TARGET_FILE.setFileName(userHome+"/.config/onedrive/refresh_token");

    //initPlasmoid();

    connect(m_timer, &QTimer::timeout, this, &LliurexOneDriveWidget::worker);
    connect(m_isLliurexOneDriveOpen, (void (QProcess::*)(int, QProcess::ExitStatus))&QProcess::finished,
            this, &LliurexOneDriveWidget::isLliurexOneDriveOpenProcessFinished);
    connect(m_getLatestFiles, (void (QProcess::*)(int, QProcess::ExitStatus))&QProcess::finished,
            this, &LliurexOneDriveWidget::getLatestFilesFinished);

    m_timer->start(5000);
    worker();
}    

void LliurexOneDriveWidget::worker(){

    bool isPassiveStatus=true;
    bool spaceIdMatch=false;
    updateGlobalStatus=false;

    if (!isWorking){
        if (!LliurexOneDriveWidget::OLD_TARGET_FILE.exists()){
            if (LliurexOneDriveWidget::TARGET_FILE.exists() ) {
                isWorking=true;
                QString onedriveConfigPath=userHome+"/.config/lliurex-onedrive-config/onedriveConfig.json";
                oneDriveSpacesConfig=m_utils->getSpacesInfo(onedriveConfigPath);
                if (oneDriveSpacesConfig.length()>0){
                    QVector<LliurexOneDriveWidgetSpaceItem> items;
                    for (QVariantList::iterator j=oneDriveSpacesConfig.begin();j!=oneDriveSpacesConfig.end();j++){
                        QVariantList tmpList=(*j).toList();
                        LliurexOneDriveWidgetSpaceItem item;
                        item.setId(tmpList[0].toString());
                        item.setName(tmpList[5].toString());
                        item.setStatus(tmpList[8].toString());
                        item.setIsRunning(tmpList[10].toBool());
                        item.setLocalFolderWarning(tmpList[11].toBool());
                        items.append(item);
                        if ((m_currentIndex!=0)&&(spaceId!="")){
                            if (spaceId==tmpList[0].toString()){
                                spaceIdMatch=true;
                                setFreeSpace(tmpList[9].toString());
                                setSyncStatus(tmpList[10].toBool());
                                if (tmpList[11].toBool()){
                                    setLliurexOneDriveOpen(true);
                                }else{
                                    if (isLliurexOneDriveOpen){
                                        setLliurexOneDriveOpen(true);
                                    }else{
                                        setLliurexOneDriveOpen(false);

                                    }
                                }
                                
                            }

                        }
                    }
                    if (oneDriveSpacesConfig!=oneDriveSpacesConfigPrev){
                        m_spacesModel->clear();
                        m_spacesModel->updateItems(items);
                        oneDriveSpacesConfigPrev=oneDriveSpacesConfig;
                        updateGlobalStatus=true;
                    }
                    isPassiveStatus=false;

                }else{
                    m_spacesModel->clear();
                    isPassiveStatus=true;
                }

            }
            if (!isPassiveStatus){
                const QString tooltip(i18n("Lliurex OneDrive"));
                setToolTip(tooltip);
                checkIfStartIsLocked();
                setStatus(ActiveStatus);
                if ((spaceId!="")&&(!spaceIdMatch)){
                    manageNavigation(0);
                }
            }else{
                setStatus(PassiveStatus);
                previousError=false;
                previousStatusError.clear();
                warning=false;
                isWorking=false;
                oneDriveSpacesConfigPrev.clear();
                manageNavigation(0);
            }
        }else{
            setStatus(ActiveStatus);
            const QString tooltip(i18n("Lliurex OneDrive"));
            setToolTip(tooltip);
            const QString subtooltip=i18n("Old OneDrive configuration detected. Access LliureX-OneDrive to run the migration process");
            updateWidget(subtooltip,"onedrive-error");
            showMigrationNotification();
            isWorking=false;

        }
    }
}

void LliurexOneDriveWidget::showMigrationNotification(){

    lastMigrationCheck=lastMigrationCheck+5;
    if (lastMigrationCheck>360){
        QString msg=i18n("Old OneDrive configuration detected.It is necessary to migrate");
        m_errorNotification = KNotification::event(QStringLiteral("MigrationWarning"), msg, {}, "onedrive-widget", nullptr, KNotification::CloseOnTimeout , QStringLiteral("llxonedrive"));
        QString name = i18n("Open Lliurex OneDrive");
        m_errorNotification->setDefaultAction(name);
        m_errorNotification->setActions({name});
        connect(m_errorNotification, QOverload<unsigned int>::of(&KNotification::activated), this, &LliurexOneDriveWidget::launchOneDrive);
        lastMigrationCheck=0;
    }
}

void LliurexOneDriveWidget::checkIfStartIsLocked(){

    QString subtooltip="";
    checkIsLliurexOneDriveOpen();
    checkIsRunning();
    if (m_utils->isLocalFolderWarning){
        warning=false;
        previousError=false;
        checkExecuted=false;
        subtooltip=i18n("Detected problems with local folder of one or more spaces");
        updateWidget(subtooltip,"onedrive-error");
        if (showStartLockMessage){
            m_errorNotification = KNotification::event(QStringLiteral("ErrorFolder"), subtooltip, {}, "onedrive-widget", nullptr, KNotification::CloseOnTimeout , QStringLiteral("llxonedrive"));
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
    }else{
        isWorking=false;
    }
}

void LliurexOneDriveWidget::checkIsLliurexOneDriveOpen(){

    if (m_isLliurexOneDriveOpen->state() != QProcess::NotRunning) {
        m_isLliurexOneDriveOpen->kill();
    }
    QString cmd="ps -ef | grep '/usr/bin/lliurex-onedrive' | grep -v 'grep'";
    m_isLliurexOneDriveOpen->start("/bin/sh", QStringList()<< "-c" 
                       << cmd,QIODevice::ReadOnly);
  
}

void LliurexOneDriveWidget::isLliurexOneDriveOpenProcessFinished(int exitCode, QProcess::ExitStatus exitStatus){

    Q_UNUSED(exitCode);

    if (exitStatus!=QProcess::NormalExit){
        isLliurexOneDriveOpen=true;
        return;
    }
    QString stdout=QString::fromLocal8Bit(m_isLliurexOneDriveOpen->readAllStandardOutput());
    QStringList pout=stdout.split("\n");
    
    if (pout[0].size()>0){
        isLliurexOneDriveOpen=true;
    }else{
        isLliurexOneDriveOpen=false;
    }

} 

void LliurexOneDriveWidget::checkStatus(){

    lastCheck=lastCheck+5;
    if ((lastCheck>90) || (updateGlobalStatus)){
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
            QString subtooltip(msgError);
            updateWidget(subtooltip,"onedrive-error");
            if(processError){
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
                    m_errorNotification = KNotification::event(globalStatus, subtooltip, {}, "onedrive-widget", nullptr, KNotification::CloseOnTimeout , QStringLiteral("llxonedrive"));
                    QString name = i18n("Open Lliurex OneDrive");
                    m_errorNotification->setDefaultAction(name);
                    m_errorNotification->setActions({name});
                    connect(m_errorNotification, QOverload<unsigned int>::of(&KNotification::activated), this, &LliurexOneDriveWidget::launchOneDrive);
                }
            }
  
        }
        checkHddFreeSpaceStatus();
        isWorking=false;
        lastCheck=0;
        checkExecuted=true;

    }
}

void LliurexOneDriveWidget::checkHddFreeSpaceStatus(){

    setHddFreeSpaceStatus(m_utils->checkLocalFreeSpace());
    lastHddCheck=lastHddCheck+5;
    bool showNotification=false;
    QString subtooltip="";
    QString hddStatus="";
    
    if (m_hddFreeSpaceStatus!="HDD_OK"){
        if (m_hddFreeSpaceStatus=="HDD_Warning"){
            hddStatus="WarningHDD";
            subtooltip=i18n("The available space in HDD is less than 10 GB");
            updateWidget(subtooltip,"onedrive-waiting");
            previousHddError=false;
            if (previousHddWarning){
                if (lastHddCheck>720){
                    showNotification=true;
                    lastHddCheck=0;
                }
            }else{
                previousHddWarning=true;
                showNotification=true;
            }  

        }else{
            hddStatus="ErrorHDD";
            subtooltip=i18n("The available space in HDD is less than 5 GB. No more files will be synced");
            updateWidget(subtooltip,"onedrive-error");
            previousHddWarning=false;
            if (previousHddError){
                if (lastHddCheck>360){
                    showNotification=true;
                    lastHddCheck=0;
                }
            }else{
                previousHddError=true;
                showNotification=true;
            }
        }

        if (showNotification){
            m_errorNotification = KNotification::event(hddStatus, subtooltip, {}, "onedrive-widget", nullptr, KNotification::CloseOnTimeout , QStringLiteral("llxonedrive"));
            QString name = i18n("Open Lliurex OneDrive");
            m_errorNotification->setDefaultAction(name);
            m_errorNotification->setActions({name});
            connect(m_errorNotification, QOverload<unsigned int>::of(&KNotification::activated), this, &LliurexOneDriveWidget::launchOneDrive);
        }
    }else{
        previousHddError=false;
        previousHddWarning=false;
    }
    isWorking=false;
    lastCheck=0;
    checkExecuted=true;

}

void LliurexOneDriveWidget::updateWidget(QString subtooltip,QString icon){

    setSubToolTip(subtooltip);
    setIconName(icon);
}   


LliurexOneDriveWidget::TrayStatus LliurexOneDriveWidget::status() const
{
    return m_status;
}


void LliurexOneDriveWidget::launchOneDrive(bool seeSpace)
{
    KIO::CommandLauncherJob *job = nullptr;
    QString cmd="lliurex-onedrive";
    if (seeSpace){
        if (spaceId!=""){
            cmd="lliurex-onedrive "+spaceId;
        }
    }
    job = new KIO::CommandLauncherJob(cmd);
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

    cleanSpaceInfoVars();
    for (QVariantList::iterator j=oneDriveSpacesConfig.begin();j!=oneDriveSpacesConfig.end();j++){
        QVariantList tmpList=(*j).toList();
        if (idSpace==tmpList[0].toString()){
            spaceId=idSpace;
            setSpaceMail(tmpList[1].toString());
            setSpaceType(tmpList[3].toString());
            spaceLocalFolder=tmpList[4].toString();
            setOneDriveFolder(tmpList[5].toString());
            spaceConfigPath=tmpList[6].toString();
            spaceSystemd=tmpList[7].toString();
            setFreeSpace(tmpList[9].toString());
            setSyncStatus(tmpList[10].toBool());
            if (tmpList[11].toBool()){
                setLliurexOneDriveOpen(true);   
            }else{
                if (isLliurexOneDriveOpen){
                    setLliurexOneDriveOpen(true);
                }else{
                    setLliurexOneDriveOpen(false);
                }
            }
            setClickedSyncBtn(false);
            manageNavigation(1);
            break;
        }
    }
   
}

void LliurexOneDriveWidget::manageNavigation(int stackIndex){

    setCurrentIndex(stackIndex);
    if (stackIndex==0){
        cleanSpaceInfoVars();
    }
}

void LliurexOneDriveWidget::openFolder()
{
            
    QString command="xdg-open "+spaceLocalFolder;
    KIO::CommandLauncherJob *job = nullptr;
    job = new KIO::CommandLauncherJob(command);
    job->start();
}

void LliurexOneDriveWidget::manageSync(){

    QString cmd;
    setClickedSyncBtn(true);
    if (m_syncStatus){
        cmd="systemctl --user stop "+spaceSystemd; 
    }else{
        m_utils->restoreSyncListFile(spaceConfigPath);
        cmd="systemctl --user start "+spaceSystemd;
    }
    
    KIO::CommandLauncherJob *job = nullptr;
    job = new KIO::CommandLauncherJob(cmd);
    job->start();

}

void LliurexOneDriveWidget::cleanSpaceInfoVars(){

    spaceConfigPath="";
    spaceLocalFolder="";
    spaceSystemd="";
    spaceId="";
}

void LliurexOneDriveWidget::getLatestFiles(){

    if (m_getLatestFiles->state() != QProcess::NotRunning) {
        m_getLatestFiles->kill();
    }
    m_filesModel->clear();
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
        m_filesModel->clear();
        setShowSearchFiles(false);     
        return;
    }

    QString stdout=QString::fromLocal8Bit(m_getLatestFiles->readAllStandardOutput());
    
    if (stdout!=""){
        searchFilesPout=stdout.split("\n");
        if (searchFilesPout.size()>0){
            QVector<LliurexOneDriveWidgetFileItem> items;
            latestFiles=m_utils->getFiles(searchFilesPout,spaceLocalFolder);
            
            for (int i=0;i<latestFiles.length();i++){
                LliurexOneDriveWidgetFileItem item;
                item.setFileName(latestFiles[i][0]);
                item.setFilePath(latestFiles[i][1]);
                item.setFileDate(latestFiles[i][2]);
                item.setFileTime(latestFiles[i][3]);
                items.append(item);
                
            }
            m_filesModel->updateItems(items);

        }else{
            m_filesModel->clear();

        }
    }else{
        m_filesModel->clear();
    }
    setShowSearchFiles(false);
}

void LliurexOneDriveWidget::goToFile(const QString &filePath)
{
            
    QString command="dolphin --select '"+filePath+"'";
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
        setClickedSyncBtn(false);
    }
}

QString LliurexOneDriveWidget::hddFreeSpaceStatus() const
{
    return m_hddFreeSpaceStatus;
}

void LliurexOneDriveWidget::setHddFreeSpaceStatus(const QString &hddFreeSpaceStatus)
{
    if (m_hddFreeSpaceStatus != hddFreeSpaceStatus) {
        m_hddFreeSpaceStatus = hddFreeSpaceStatus;
        emit hddFreeSpaceStatusChanged();
    }
}

bool LliurexOneDriveWidget::clickedSyncBtn()
{
    return m_clickedSyncBtn;
}

void LliurexOneDriveWidget::setClickedSyncBtn(bool clickedSyncBtn)
{
    if (m_clickedSyncBtn != clickedSyncBtn) {
        m_clickedSyncBtn = clickedSyncBtn;
        emit clickedSyncBtnChanged();
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

LliurexOneDriveWidgetFilesModel *LliurexOneDriveWidget::filesModel() const
{
    return m_filesModel;
}
