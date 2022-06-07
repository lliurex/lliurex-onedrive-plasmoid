
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
            setStatus(ActiveStatus);
        }else{
            setStatus(PassiveStatus);
        }
        isWorking=false;
        
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
