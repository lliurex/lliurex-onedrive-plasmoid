#include "LliurexOneDriveWidgetFileItem.h"

#include <QDebug>

LliurexOneDriveWidgetFileItem::LliurexOneDriveWidgetFileItem()
    : m_fileName()
    , m_filePath()
{
}

QString LliurexOneDriveWidgetFileItem::fileName() const
{
    return m_fileName;
}

void LliurexOneDriveWidgetFileItem::setFileName(const QString &name)
{
    m_fileName = name;
}

QString LliurexOneDriveWidgetFileItem::filePath() const
{
    return m_filePath;
}

void LliurexOneDriveWidgetFileItem::setFilePath(const QString &path)
{
    m_filePath = path;
}

QString LliurexOneDriveWidgetFileItem::fileDate() const
{
    return m_fileDate;
}

void LliurexOneDriveWidgetFileItem::setFileDate(const QString &date)
{
    m_fileDate = date;
}

QString LliurexOneDriveWidgetFileItem::fileTime() const
{
    return m_fileTime;
}

void LliurexOneDriveWidgetFileItem::setFileTime(const QString &time)
{
    m_fileTime = time;
}

bool LliurexOneDriveWidgetFileItem::operator==(const LliurexOneDriveWidgetFileItem &other) const
{
    return m_fileName == other.m_fileName
        && m_filePath == other.m_filePath
        && m_fileDate == other.m_fileDate
        && m_fileTime == other.m_fileTime;
}

bool LliurexOneDriveWidgetFileItem::operator!=(const LliurexOneDriveWidgetFileItem &other) const
{
    return ! (*this == other);
}
