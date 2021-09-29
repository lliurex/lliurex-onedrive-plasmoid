
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
#include <QThread>


LliurexOneDriveWidget::LliurexOneDriveWidget(QObject *parent)
    : QObject(parent)
    ,m_timer(new QTimer(this))
    ,m_utils(new LliurexOneDriveWidgetUtils(this))
    
{
    
    QString tmpHome=m_utils->getUserHome();
    TARGET_FILE.setFileName(tmpHome+"/.config/onedrive/refresh_token");
   
    initPlasmoid();

    connect(m_timer, &QTimer::timeout, this, &LliurexOneDriveWidget::worker);
    m_timer->start(5000);
    worker();
}    

void LliurexOneDriveWidget::initPlasmoid(){

    QString tmpHome=m_utils->getUserHome(); 
    QString tmpFolder=tmpHome+"/OneDrive";
    setOneDriveFolder(tmpFolder);
    
}

void LliurexOneDriveWidget::worker(){

    if (!is_working){
        if (LliurexOneDriveWidget::TARGET_FILE.exists() ) {
           const QString tooltip(i18n("Lliurex OneDrive"));
           setToolTip(tooltip);
           bool isrunning=m_utils->isRunning(); 
           setSyncStatus(isrunning);
           setStatus(ActiveStatus); 

           if (isrunning){
               if ((!previousError) & (!warning)){
                    QString subtooltip(i18n("Starting the synchronization"));
                    updateWidget(subtooltip,"onedrive");
               }
               checkStatus();
           }else{
               QString subtooltip(i18n("Synchronization is stopped"));
               updateWidget(subtooltip,"onedrive-stop");
               previousError=false;
               previousErrorCode="";
               warning=false;
           }

        }else{
            setStatus(PassiveStatus);
            previousError=false;
            previousErrorCode="";
            warning=false;

        }
        
    }

}

void LliurexOneDriveWidget::checkStatus(){

    last_check=last_check+5;
    if (last_check>90){
        last_check=0;
        adbus=new AsyncDbus(this);
        QObject::connect(adbus,SIGNAL(message(QStringList)),this,SLOT(dbusDone(QStringList)));
        adbus->start();
       
    }
}    

QStringList LliurexOneDriveWidget::runCheckAccount(){

    return m_utils->getAccountStatus();

}

void LliurexOneDriveWidget::dbusDone(QStringList result){

    is_working=false;
    bool showNotification=false;
        
    adbus->exit(0);
    if (adbus->wait()){
        delete adbus;
    }

    QStringList warningCode;
    warningCode<<m_utils->UPLOADING_PENDING_CHANGES<<m_utils->OUT_OF_SYNC;
    QStringList errorCode;
    errorCode<<m_utils->GENERAL_ERROR<<m_utils->ZERO_SPACE_AVAILABLE<<m_utils->UNAUTHORIZED_ERROR<<m_utils->NETWORK_CONNECT_ERROR;
    
    if (warningCode.contains(result[0])){
        warning=true;
        QString subtooltip(i18n("Changes pending of synchronization"));
        updateWidget(subtooltip,"onedrive-pending");
        previousError=false;
        previousErrorCode="";

    }else if (errorCode.contains(result[0])){
        warning=false;
        QString subtooltip(i18n("OneDrive client return an error.\nOpen Lliurex OneDrive for more information"));
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
            m_errorNotification = KNotification::event(QStringLiteral("ErrorStatus"), subtooltip, {}, "lliurex-onedrive", nullptr, KNotification::CloseOnTimeout , QStringLiteral("llxonedrive"));
            QString name = i18n("Open Lliurex OneDrive");
            m_errorNotification->setDefaultAction(name);
            m_errorNotification->setActions({name});
            connect(m_errorNotification, QOverload<unsigned int>::of(&KNotification::activated), this, &LliurexOneDriveWidget::launchOneDrive);
        }
    
    }else{
        QString subtooltip(i18n("All remote changes are synchronized"));
        updateWidget(subtooltip,"onedrive");
        warning=false;
        previousError=false;
        previousErrorCode="";
    }
   
    setFreeSpace(result[1]);

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

    bool result=m_utils->manageSync();

    if (!result){
        bool isrunning=m_utils->isRunning();
        if (isrunning){
            QString subtooltip(i18n("Unable to stop synchronization"));
            m_errorNotification = KNotification::event(QStringLiteral("ErrorStop"), subtooltip, {}, "lliurex-onedrive", nullptr, KNotification::CloseOnTimeout , QStringLiteral("llxonedrive"));
     
        }else{
            QString subtooltip(i18n("Unable to start synchronization"));
            m_errorNotification = KNotification::event(QStringLiteral("ErrorStart"), subtooltip, {}, "lliurex-onedrive", nullptr, KNotification::CloseOnTimeout , QStringLiteral("llxonedrive"));
        }
        connect(m_errorNotification, QOverload<unsigned int>::of(&KNotification::activated), this, &LliurexOneDriveWidget::launchOneDrive);
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



