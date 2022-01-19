#include "LliurexOneDriveWidgetItem.h"

#include <QDebug>

LliurexOneDriveWidgetItem::LliurexOneDriveWidgetItem()
    : m_fileName()
    , m_filePath()
{
}

QString LliurexOneDriveWidgetItem::fileName() const
{
    return m_fileName;
}

void LliurexOneDriveWidgetItem::setFileName(const QString &name)
{
    m_fileName = name;
}

QString LliurexOneDriveWidgetItem::filePath() const
{
    return m_filePath;
}

void LliurexOneDriveWidgetItem::setFilePath(const QString &path)
{
    m_filePath = path;
}

QString LliurexOneDriveWidgetItem::fileDate() const
{
    return m_fileDate;
}

void LliurexOneDriveWidgetItem::setFileDate(const QString &date)
{
    m_fileDate = date;
}

QString LliurexOneDriveWidgetItem::fileTime() const
{
    return m_fileTime;
}

void LliurexOneDriveWidgetItem::setFileTime(const QString &time)
{
    m_fileTime = time;
}

bool LliurexOneDriveWidgetItem::operator==(const LliurexOneDriveWidgetItem &other) const
{
    return m_fileName == other.m_fileName
        && m_filePath == other.m_filePath
        && m_fileDate == other.m_fileDate
        && m_fileTime == other.m_fileTime;
}

bool LliurexOneDriveWidgetItem::operator!=(const LliurexOneDriveWidgetItem &other) const
{
    return ! (*this == other);
}
