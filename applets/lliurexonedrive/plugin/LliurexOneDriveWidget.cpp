#include "LliurexOneDriveWidget.h"
#include "LliurexOneDriveWidgetFileItem.h"
#include "LliurexOneDriveWidgetFilesModel.h"
#include "LliurexOneDriveWidgetSpacesModel.h"
#include "LliurexOneDriveWidgetSpaceItem.h"
#include "LliurexOneDriveWidgetUtils.h"

#include <KLocalizedString>
#include <KNotification>
#include <KIO/CommandLauncherJob>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QtConcurrent>


LliurexOneDriveWidget::LliurexOneDriveWidget(QObject *parent)
    : QObject(parent)
    ,m_utils(new LliurexOneDriveWidgetUtils(this))
    ,m_spacesModel(new LliurexOneDriveWidgetSpacesModel(this))
    ,m_filesModel(new LliurexOneDriveWidgetFilesModel(this))
    ,m_timer(new QTimer(this))
    ,m_isLliurexOneDriveOpen(new QProcess(this))
   
{
    
    userHome=m_utils->getUserHome();
    TARGET_FILE.setFileName(userHome+QStringLiteral("/.config/lliurex-onedrive-config/onedriveConfig.json"));
    OLD_TARGET_FILE.setFileName(userHome+QStringLiteral("/.config/onedrive/refresh_token"));

    qRegisterMetaType<LliurexOneDriveWidgetSpacesModel*>("LliurexOneDriveWidgetSpacesModel*");
    qRegisterMetaType<LliurexOneDriveWidgetFilesModel*>("LliurexOneDriveWidgetFilesModel*");
    qRegisterMetaType<QMap<QString, QVariantMap>>("QMap<QString, QVariantMap>");
    qRegisterMetaType<QVector<LliurexOneDriveWidgetSpaceItem>>("QVector<LliurexOneDriveWidgetSpaceItem>");
    qRegisterMetaType<QList<int>>("QList<int>");
    qRegisterMetaType<SpacesUpdateData>("SpacesUpdateData");
    qRegisterMetaType<QVector<LliurexOneDriveWidgetFileItem>>("QVector<LliurexOneDriveWidgetFileItem>");
    
    connect(m_timer, &QTimer::timeout, this, &LliurexOneDriveWidget::worker);
    connect(m_utils, &LliurexOneDriveWidgetUtils::getSpacesInfoFinished,this,&LliurexOneDriveWidget::handleGetSpacesInfoFinished);
    connect(m_isLliurexOneDriveOpen, (void (QProcess::*)(int, QProcess::ExitStatus))&QProcess::finished,
            this, &LliurexOneDriveWidget::isLliurexOneDriveOpenProcessFinished);
    connect(&m_watcher, &QFutureWatcherBase::finished, this, &LliurexOneDriveWidget::getLatestFilesFinished);
    connect(m_utils,&LliurexOneDriveWidgetUtils::fetchUploadedFilesFinished,this,&LliurexOneDriveWidget::handleUploadedFilesFinished);
  
    m_timer->start(5000);
    worker();
}    

void LliurexOneDriveWidget::worker(){

    bool isPassiveStatus=true;
    bool spaceIdMatch=false;
    updateGlobalStatus=false;

    if (!isWorking){
        if (LliurexOneDriveWidget::TARGET_FILE.exists() ) {
            isWorking=true;
            QString onedriveConfigPath=userHome+"/.config/lliurex-onedrive-config/onedriveConfig.json";
            m_utils->getSpacesInfo(onedriveConfigPath);
           
        }else{
            setStatus(PassiveStatus);
            previousError=false;
            previousStatusError.clear();
            warning=false;
            isWorking=false;
            oneDriveSpacesConfig.clear();
            manageNavigation(0);
        }
        
    }
}

void LliurexOneDriveWidget::handleGetSpacesInfoFinished(const SpacesUpdateData &data) 
{

    isLocalFolderWarning = data.folderWarning;
    areSpacesSyncRunning = data.isRunning;
    isUpdateRequiredWarning = data.updateRequired;
    globalStatus = data.status;
    statusErrorChanged = false;

    if (previousStatusError != data.statusErrorCodes) {
        previousStatusError = data.statusErrorCodes;
        statusErrorChanged = !previousStatusError.isEmpty();
    }

    if (data.info.isEmpty()) {
        m_spacesModel->clear();
        setStatus(PassiveStatus);
        previousError = false;
        previousStatusError.clear();
        warning = false;
        isWorking = false;
        oneDriveSpacesConfig.clear();
        manageNavigation(0);
        return;
    }

    if (oneDriveSpacesConfig != data.info){
        oneDriveSpacesConfig = data.info;
        m_spacesModel->clear();
        m_spacesModel->updateItems(data.model);
        updateGlobalStatus = true;
    }

    bool spaceIdMatch = false;

    if (m_currentIndex != 0 && !spaceId.isEmpty()) {
        auto it = oneDriveSpacesConfig.find(spaceId);
        if (it != oneDriveSpacesConfig.end()) {
            spaceIdMatch = true;
            const QVariantMap &spaceInfo = it.value();

            setFreeSpace(spaceInfo.value("freeSpace").toString());
            setSyncStatus(spaceInfo.value("isRunning").toBool());
            getLogSize(spaceInfo.value("configPath").toString());

            bool shouldBeOpen = spaceInfo.value("folderWarning").toBool() || 
                                spaceInfo.value("updateReq").toBool() || 
                                isLliurexOneDriveOpen;
            setLliurexOneDriveOpen(shouldBeOpen);
        }
    }

    setToolTip(i18n("Lliurex OneDrive"));
    checkIfStartIsLocked();
    setStatus(ActiveStatus);

    if (!spaceId.isEmpty() && !spaceIdMatch) {
        manageNavigation(0);
    }
}

void LliurexOneDriveWidget::sendNotification(const QString &id, const QString &msg) {
    
    m_errorNotification = KNotification::event(id, msg, {}, "onedrive-widget", nullptr, 
                                              KNotification::CloseOnTimeout, QStringLiteral("llxonedrive"));
    QString name = i18n("Open Lliurex OneDrive");
    m_errorNotification->setDefaultAction(name);
    m_errorNotification->setActions({name});
    connect(m_errorNotification, QOverload<unsigned int>::of(&KNotification::activated), this, &LliurexOneDriveWidget::launchOneDrive);
}

void LliurexOneDriveWidget::checkIfStartIsLocked() {
    checkIsLliurexOneDriveOpen();
    checkIsRunning();

    if (isLocalFolderWarning) {
        warning = previousError = checkExecuted = isWorking = false;
        QString subtooltip = i18n("Detected problems with local folder of one or more spaces");
        updateWidget(subtooltip, "onedrive-error");

        if (showLocalFolderWarning) {
            sendNotification(QStringLiteral("ErrorFolder"), subtooltip);
            showLocalFolderWarning = false;
        }
        return;
    }

    if (isUpdateRequiredWarning) {
        QString subtooltip = i18n("New version of OneDrive client detected. Your attention is required");
        updateWidget(subtooltip, "onedrive-waiting");
        isWorking = false;
        
        lastUpdateCheck += 5;
        if (lastUpdateCheck > 360) {
            sendNotification(QStringLiteral("UpdateWarning"), subtooltip);
            lastUpdateCheck = 0;
        }
        return;
    }

    showLocalFolderWarning = true;
    isWorking = false;
}

void LliurexOneDriveWidget::checkIsRunning() {
    if (isLocalFolderWarning) {
        isWorking = false;
        return;
    }

    if (!areSpacesSyncRunning) {
        updateWidget(i18n("Synchronization is stopped"), "onedrive-pause");
        previousError = warning = checkExecuted = isWorking = false;
        previousStatusError.clear();
        return;
    }

    if (!previousError && !warning && !checkExecuted) {
        updateWidget(i18n("Starting the synchronization"), "onedrive-starting");
    }
    checkStatus();
}

void LliurexOneDriveWidget::checkStatus() {
    lastCheck += 5;
    if (lastCheck <= 90 && !updateGlobalStatus) return;

    bool processError = true;
    QString subtooltip;

    if (globalStatus == "OK") {
        subtooltip = i18n("All remote changes are synchronized");
        updateWidget(subtooltip, "onedrive-running");
        warning = previousError = false;
        countRepeatGeneralError = countRepeatError = lastErrorNotification = 0;
        previousStatusError.clear();
    } 
    else if (globalStatus == "Warning") {
        warning = true;
        subtooltip = i18n("One or more spaces have changes pending of synchronization");
        updateWidget(subtooltip, "onedrive-waiting");
        previousError = false;
        countRepeatGeneralError = countRepeatError = lastErrorNotification = 0;
    } 
    else {
        warning = false;
        if (globalStatus == "GeneralError") {
            countRepeatGeneralError++;
            countRepeatError = 0;
            subtooltip = i18n("Unable to connect with Microsoft OneDrive");
        } else {
            countRepeatError++;
            countRepeatGeneralError = 0;
            subtooltip = i18n("OneDrive has reported an error in one or more spaces");
        }

        if (countRepeatGeneralError < 2 && countRepeatError < 2) processError = false;

        updateWidget(subtooltip, "onedrive-error");

        if (processError) {
            bool shouldNotify = !previousError || statusErrorChanged || (++lastErrorNotification > 40);
            if (shouldNotify) {
                lastErrorNotification = 0;
                previousError = true;
                sendNotification(globalStatus, subtooltip);
            }
        }
    }

    checkHddFreeSpaceStatus();
    isWorking = false;
    lastCheck = 0;
    checkExecuted = true;
}

void LliurexOneDriveWidget::checkIsLliurexOneDriveOpen(){

    if (m_isLliurexOneDriveOpen->state() != QProcess::NotRunning) {
        return; 
    }

    m_isLliurexOneDriveOpen->start("pgrep", {"-f", "/usr/bin/lliurex-onedrive"});

}

void LliurexOneDriveWidget::isLliurexOneDriveOpenProcessFinished(int exitCode, QProcess::ExitStatus exitStatus){

    isLliurexOneDriveOpen = (exitStatus == QProcess::NormalExit && exitCode == 0);

} 

void LliurexOneDriveWidget::checkHddFreeSpaceStatus(){

    setHddFreeSpaceStatus(m_utils->checkLocalFreeSpace());
   
    bool showNotification=false;
    QString subtooltip="";
    QString hddStatus="";
    
    if (m_hddFreeSpaceStatus!="HDD_OK"){
        lastHddCheck+=1;
        if (m_hddFreeSpaceStatus=="HDD_Warning"){
            hddStatus="WarningHDD";
            subtooltip=i18n("The available space in HDD is less than 10 GB");
            updateWidget(subtooltip,"onedrive-waiting");
            previousHddError=false;
            if (previousHddWarning){
                if (lastHddCheck>40){
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
                if (lastHddCheck>20){
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

void LliurexOneDriveWidget::openHelp()
{
            
    QString command="xdg-open https://wiki.edu.gva.es/lliurex/tiki-index.php?page=Lliurex-Onedrive";
    KIO::CommandLauncherJob *job = nullptr;
    job = new KIO::CommandLauncherJob(command);
    job->start();
}

void LliurexOneDriveWidget::goToSpace(QString const &idSpace ){

    cleanSpaceInfoVars();
    if (oneDriveSpacesConfig.contains(idSpace)){
        QVariantMap spaceInfo=oneDriveSpacesConfig.value(idSpace);
        spaceId=idSpace;
        setSpaceMail(spaceInfo["email"].toString());
        setSpaceType(spaceInfo["spaceType"].toString());
        spaceLocalFolder=spaceInfo["localFolder"].toString();
        setOneDriveFolder(spaceInfo["name"].toString());
        spaceConfigPath=spaceInfo["configPath"].toString();
        spaceSystemd=spaceInfo["systemd"].toString();
        try{
            spaceCreated=spaceInfo["created"].toString();
        }catch(...){
            spaceCreated="";
        }
        setFreeSpace(spaceInfo["freeSpace"].toString());
        setSyncStatus(spaceInfo["isRunning"].toBool());
        if (spaceInfo["folderWarning"].toBool() || spaceInfo["updateReq"].toBool()){
            setLliurexOneDriveOpen(true);   
        }else{
            if (isLliurexOneDriveOpen){
                setLliurexOneDriveOpen(true);
            }else{
                setLliurexOneDriveOpen(false);
            }
        }
        getLogSize(spaceConfigPath);
        setClickedSyncBtn(false);
        manageNavigation(1);
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

void LliurexOneDriveWidget::getLatestFiles() {

    if (m_watcher.isRunning()) return;

    m_filesModel->clear();
    setShowSearchFiles(true);
    QFuture<QVector<LliurexOneDriveWidgetFileItem>> future = QtConcurrent::run(
        LliurexOneDriveWidgetUtils::getFiles, 
        spaceLocalFolder, 
        10
    );
    m_watcher.setFuture(future);
}

void LliurexOneDriveWidget::getLatestFilesFinished() {

    QVector<LliurexOneDriveWidgetFileItem> items = m_watcher.result();
    
    if (!items.isEmpty()) {
        m_filesModel->updateItems(items);
    } else {
        m_filesModel->clear();
    }
    
    setShowSearchFiles(false);
}

void LliurexOneDriveWidget::getLatestUploadedFiles() {

    m_filesModel->clear();
    setShowSearchFiles(true);
    m_utils->fetchUploadedFiles(spaceSystemd,spaceCreated);
}

void LliurexOneDriveWidget::handleUploadedFilesFinished(const QVector<LliurexOneDriveWidgetFileItem> &items){

    m_filesModel->updateItems(items);
    setShowSearchFiles(false);
}

void LliurexOneDriveWidget::goToFile(const QString &filePath)
{
            
    QString command="dolphin --select '"+filePath+"'";
    KIO::CommandLauncherJob *job = nullptr;
    job = new KIO::CommandLauncherJob(command);
    job->start();
}

void LliurexOneDriveWidget::updateWidget(QString subtooltip,QString icon){

    setSubToolTip(subtooltip);
    setIconName(icon);
}   

LliurexOneDriveWidget::TrayStatus LliurexOneDriveWidget::status() const
{
    return m_status;
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

LliurexOneDriveWidgetSpacesModel *LliurexOneDriveWidget::spacesModel() const
{
    return m_spacesModel;
}

void LliurexOneDriveWidget::cleanSpaceInfoVars(){

    spaceConfigPath="";
    spaceLocalFolder="";
    spaceSystemd="";
    spaceId="";
    spaceCreated="";
}

void LliurexOneDriveWidget::getLogSize(QString configPath)
{

     QString user=userHome.split("/")[2];
     logFilePath=configPath+"/log/"+user+".onedrive.log";
     QString logSize=m_utils->getLogFileSize(logFilePath);
     setLogFileSize(logSize);

}

void LliurexOneDriveWidget::openLogFile()
{
            
    recentFile.setFileName(logFilePath);
    if (recentFile.exists()){
        QString command="xdg-open "+logFilePath;
        KIO::CommandLauncherJob *job = nullptr;
        job = new KIO::CommandLauncherJob(command);
        job->start();
    }
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

QString LliurexOneDriveWidget::logFileSize() const
{
    return m_logFileSize;
}

void LliurexOneDriveWidget::setLogFileSize(const QString &logFileSize)
{
    if (m_logFileSize != logFileSize) {
        m_logFileSize = logFileSize;
        emit logFileSizeChanged();
    }
}




