
#include <iostream>
#include "LliurexOneDriveWidget.h"
#include "LliurexOneDriveWidgetUtils.h"

#include <KLocalizedString>
#include <KFormat>
#include <KNotification>
#include <KIO/CommandLauncherJob>
#include <QTimer>
#include <QStandardPaths>
#include <QDebug>
#include <QFile>


LliurexOneDriveWidget::LliurexOneDriveWidget(QObject *parent)
    : QObject(parent)
    ,m_timer(new QTimer(this))
    ,m_utils(new LliurexOneDriveWidgetUtils(this))
    ,m_isRunningProcess(new QProcess(this))
    ,m_isDisplayProcess(new QProcess(this))
    ,m_checkProcess(new QProcess(this))
    ,m_isLliurexOneDriveOpen(new QProcess(this))

    
{
    
    userHome=m_utils->getUserHome();
    TARGET_FILE.setFileName(userHome+"/.config/onedrive/refresh_token");
   
    initPlasmoid();

    connect(m_timer, &QTimer::timeout, this, &LliurexOneDriveWidget::worker);
    connect(m_isRunningProcess, (void (QProcess::*)(int, QProcess::ExitStatus))&QProcess::finished,
            this, &LliurexOneDriveWidget::isRunningProcessFinished);
    connect(m_isDisplayProcess, (void (QProcess::*)(int, QProcess::ExitStatus))&QProcess::finished,
            this, &LliurexOneDriveWidget::isDisplayProcessFinished);
    connect(m_checkProcess, (void (QProcess::*)(int, QProcess::ExitStatus))&QProcess::finished,
            this, &LliurexOneDriveWidget::checkProcessFinished);
    connect(m_isLliurexOneDriveOpen, (void (QProcess::*)(int, QProcess::ExitStatus))&QProcess::finished,
            this, &LliurexOneDriveWidget::isLliurexOneDriveOpenProcessFinished);

    m_timer->start(5000);
    worker();
}    

void LliurexOneDriveWidget::initPlasmoid(){

    QString tmpFolder=userHome+"/OneDrive";
    setOneDriveFolder(tmpFolder);
    
}

void LliurexOneDriveWidget::worker(){

    if (!isWorking){
        if (LliurexOneDriveWidget::TARGET_FILE.exists() ) {
           const QString tooltip(i18n("Lliurex OneDrive"));
           setToolTip(tooltip);
           checkIsRunning();
           checkIsLliurexOneDriveOpen();
        }else{
            setStatus(PassiveStatus);
            previousError=false;
            previousErrorCode="";
            warning=false;
            checkExecuted=false;
            changeSyncStatus=false;
        }
        
    }

}

void LliurexOneDriveWidget::checkIsRunning(){

    if (m_isRunningProcess->state() != QProcess::NotRunning) {
        m_isRunningProcess->kill();
    }
    QString cmd="ps -ef | grep 'onedrive --monitor' | grep -v 'grep'";
    m_isRunningProcess->start("/bin/sh", QStringList()<< "-c" 
                       << cmd,QIODevice::ReadOnly);
  
}    

void LliurexOneDriveWidget::isRunningProcessFinished(int exitCode, QProcess::ExitStatus exitStatus){

    Q_UNUSED(exitCode);

    if (exitStatus!=QProcess::NormalExit){
        isRunning=false;
        return;
    }

    QString stdout=QString::fromLocal8Bit(m_isRunningProcess->readAllStandardOutput());
    QStringList pout=stdout.split("\n");
    
    if (pout[0].size()>0){
        if (tryChangeStatus){
            tryChangeStatus=false;
            initClientStatus=true;
            m_utils->manageSync(initClientStatus);
        }else{
            isRunning=true;
            if (changeSyncStatus){
                if (initClientStatus==isRunning){
                    showSyncNotification();
                }
                changeSyncStatus=false;
            } 
        }     
    }else{
        if (tryChangeStatus){
            tryChangeStatus=false;
            initClientStatus=false;
            m_utils->manageSync(initClientStatus);

        }else{
            isRunning=false;
            if (changeSyncStatus){
                if (initClientStatus==isRunning){
                    showSyncNotification();
                }
                changeSyncStatus=false;
            } 
        }    
     
    }
    
    setSyncStatus(isRunning);
    setStatus(ActiveStatus); 
    
    if (isRunning){
        if ((!previousError) & (!warning) & (!checkExecuted)){
            QString subtooltip(i18n("Starting the synchronization"));
            updateWidget(subtooltip,"onedrive-starting");
        }
        checkStatus();
    }else{
        QString subtooltip(i18n("Synchronization is stopped"));
        updateWidget(subtooltip,"onedrive-pause");
        previousError=false;
        previousErrorCode="";
        warning=false;
        checkExecuted=false;
        changeSyncStatus=false;
    }
}

void LliurexOneDriveWidget::checkStatus(){

    lastCheck=lastCheck+5;
    if (lastCheck>90){
        isWorking=true;
        checkIsDisplayRunning();
       
    }
}

void LliurexOneDriveWidget::checkIsDisplayRunning(){

    if (m_isDisplayProcess->state() != QProcess::NotRunning) {
        m_isDisplayProcess->kill();
    }
    QString cmd="ps -ef | grep 'onedrive --display-sync-status --verbose' | grep -v 'grep'";
    m_isDisplayProcess->start("/bin/sh", QStringList()<< "-c" 
                       << cmd,QIODevice::ReadOnly);
  
}    

void LliurexOneDriveWidget::isDisplayProcessFinished(int exitCode, QProcess::ExitStatus exitStatus){

    Q_UNUSED(exitCode);

    if (exitStatus!=QProcess::NormalExit){
        isWorking=false;
        return;
    }
    QString stdout=QString::fromLocal8Bit(m_isDisplayProcess->readAllStandardOutput());
    QStringList pout=stdout.split("\n");

    if (pout[0].size()>0){
        isWorking=false;
        return;
    }else{
        lastCheck=0;
        if (m_checkProcess->state() != QProcess::NotRunning) {
            m_checkProcess->kill();
        }
        m_checkProcess->setProgram("/usr/bin/onedrive");
        QStringList arguments={"--display-sync-status", "--verbose"};
        m_checkProcess->setArguments(arguments);
        m_checkProcess->start(QIODevice::ReadOnly);;
    }
}        

void LliurexOneDriveWidget::checkProcessFinished(int exitCode, QProcess::ExitStatus exitStatus){

    QStringList result;
    bool showNotification=false;
    QString msgError;
    bool processError=true;

    if (exitStatus!=QProcess::NormalExit){
        isWorking=false;
        checkExecuted=true;
        return;
    }
    QString stdout=QString::fromLocal8Bit(m_checkProcess->readAllStandardOutput());
    QString stderr=QString::fromLocal8Bit(m_checkProcess->readAllStandardError());

    result=m_utils->getAccountStatus(exitCode,stdout,stderr);

    QStringList warningCode;
    warningCode<<m_utils->UPLOADING_PENDING_CHANGES<<m_utils->OUT_OF_SYNC;
    QStringList errorCode;
    errorCode<<m_utils->GENERAL_ERROR<<m_utils->ZERO_SPACE_AVAILABLE<<m_utils->UNAUTHORIZED_ERROR<<m_utils->NETWORK_CONNECT_ERROR<<m_utils->SERVICE_UNAVAILABLE;

    if (warningCode.contains(result[0])){
        warning=true;
        QString subtooltip(i18n("Changes pending of synchronization"));
        updateWidget(subtooltip,"onedrive-waiting");
        previousError=false;
        previousErrorCode="";
        countRepeatGeneralError=0;

    }else if (errorCode.contains(result[0])){
        warning=false;
        if (result[0]==m_utils->GENERAL_ERROR){
            countRepeatGeneralError+=1;
            if (countRepeatGeneralError<2){
                processError=false;
            }
        }else{
            countRepeatGeneralError=0;
        }

        if (processError){

            msgError=m_utils->getErrorMessage(result[0]);
            QString subtooltip(msgError);
            updateWidget(subtooltip,"onedrive-error");

            if (previousError){
                if (previousErrorCode!=result[0]){
                    showNotification=true;
                    previousErrorCode=result[0];
                }
            }else{
                previousError=true;
                previousErrorCode=result[0];
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
    
    }else{
        QString subtooltip(i18n("All remote changes are synchronized"));
        updateWidget(subtooltip,"onedrive-running");
        warning=false;
        previousError=false;
        previousErrorCode="";
        countRepeatGeneralError=0;
    }
   
    isWorking=false;
    checkExecuted=true;
    setFreeSpace(result[1]);
} 

void LliurexOneDriveWidget::checkIsLliurexOneDriveOpen(){

    if (m_isLliurexOneDriveOpen->state() != QProcess::NotRunning) {
        m_isLliurexOneDriveOpen->kill();
    }
    QString cmd="ps -ef | grep 'lliurex-onedrive' | grep -v 'grep'";
    m_isLliurexOneDriveOpen->start("/bin/sh", QStringList()<< "-c" 
                       << cmd,QIODevice::ReadOnly);
  
}

void LliurexOneDriveWidget::isLliurexOneDriveOpenProcessFinished(int exitCode, QProcess::ExitStatus exitStatus){

    Q_UNUSED(exitCode);

    if (exitStatus!=QProcess::NormalExit){
        setLliurexOneDriveOpen(true);
        return;
    }
    QString stdout=QString::fromLocal8Bit(m_isLliurexOneDriveOpen->readAllStandardOutput());
    QStringList pout=stdout.split("\n");
    
    if (pout[0].size()>0){
        setLliurexOneDriveOpen(true);
    }else{
        setLliurexOneDriveOpen(false);
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

void LliurexOneDriveWidget::openFolder()
{
            
    QString command="xdg-open "+m_oneDriveFolder;
    KIO::CommandLauncherJob *job = nullptr;
    job = new KIO::CommandLauncherJob(command);
    job->start();
}

void LliurexOneDriveWidget::manageSync(){

    changeSyncStatus=true;
    tryChangeStatus=true;
    checkIsRunning();
}

void LliurexOneDriveWidget::showSyncNotification(){

    if (initClientStatus){
        QString subtooltip(i18n("Unable to stop synchronization"));
        m_errorNotification = KNotification::event(QStringLiteral("ErrorStop"), subtooltip, {}, "onedrive-widget", nullptr, KNotification::CloseOnTimeout , QStringLiteral("llxonedrive"));
    }else{
        QString subtooltip(i18n("Unable to start synchronization"));
        m_errorNotification = KNotification::event(QStringLiteral("ErrorStart"), subtooltip, {}, "onedrive-widget", nullptr, KNotification::CloseOnTimeout , QStringLiteral("llxonedrive"));
    }
            
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

void LliurexOneDriveWidget::setSyncStatus(bool &syncStatus)
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

void LliurexOneDriveWidget::openHelp()
{
            
    QString command="xdg-open https://wiki.edu.gva.es/lliurex/tiki-index.php?page=Lliurex-Onedrive";
    KIO::CommandLauncherJob *job = nullptr;
    job = new KIO::CommandLauncherJob(command);
    job->start();
}




