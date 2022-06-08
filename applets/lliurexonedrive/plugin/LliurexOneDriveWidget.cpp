
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
    
{
    
    userHome=m_utils->getUserHome();
    TARGET_FILE.setFileName(userHome+"/.config/lliurex-onedrive-config/onedriveConfig.json");
   
    //initPlasmoid();

    connect(m_timer, &QTimer::timeout, this, &LliurexOneDriveWidget::worker);

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
