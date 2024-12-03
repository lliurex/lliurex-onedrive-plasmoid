#include "LliurexOneDriveWidgetSpaceItem.h"

#include <QDebug>

LliurexOneDriveWidgetSpaceItem::LliurexOneDriveWidgetSpaceItem()
    : m_id()
    , m_name()
    , m_status()
    , m_isRunning()
    , m_localFolderWarning()
    , m_updateRequiredWarning()
{
}

QString LliurexOneDriveWidgetSpaceItem::id() const
{
    return m_id;
}

void LliurexOneDriveWidgetSpaceItem::setId(const QString &id)
{
    m_id = id;
}

QString LliurexOneDriveWidgetSpaceItem::name() const
{
    return m_name;
}

void LliurexOneDriveWidgetSpaceItem::setName(const QString &name)
{
    m_name = name;
}

QString LliurexOneDriveWidgetSpaceItem::status() const
{
    return m_status;
}

void LliurexOneDriveWidgetSpaceItem::setStatus(const QString &status)
{
    m_status = status;
}

bool LliurexOneDriveWidgetSpaceItem::isRunning() const
{
    return m_isRunning;
}

void LliurexOneDriveWidgetSpaceItem::setIsRunning(const bool &isRunning)
{
    m_isRunning = isRunning;
}

bool LliurexOneDriveWidgetSpaceItem::localFolderWarning() const
{
    return m_localFolderWarning;
}

void LliurexOneDriveWidgetSpaceItem::setLocalFolderWarning(const bool &localFolderWarning)
{
    m_localFolderWarning = localFolderWarning;
}

bool LliurexOneDriveWidgetSpaceItem::updateRequiredWarning() const
{    
    return m_updateRequiredWarning;
}

void LliurexOneDriveWidgetSpaceItem::setUpdateRequiredWarning(const bool &updateRequiredWarning)
{
    m_updateRequiredWarning = updateRequiredWarning;
}

bool LliurexOneDriveWidgetSpaceItem::operator==(const LliurexOneDriveWidgetSpaceItem &other) const
{
    return m_id==other.m_id
        && m_name == other.m_name
        && m_status == other.m_status
        && m_isRunning == other.m_isRunning
        && m_localFolderWarning == other.m_localFolderWarning
        && m_updateRequiredWarning==other.m_updateRequiredWarning;
}

bool LliurexOneDriveWidgetSpaceItem::operator!=(const LliurexOneDriveWidgetSpaceItem &other) const
{
    return ! (*this == other);
}
