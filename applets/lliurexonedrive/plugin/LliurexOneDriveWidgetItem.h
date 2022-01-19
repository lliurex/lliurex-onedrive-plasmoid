#ifndef PLASMA_LLIUREX_ONEDRIVE_WIDGET_ITEM_H
#define PLASMA_LLIUREX_ONEDRIVE_WIDGET_ITEM_H

#include <QString>
#include <QMetaType>

/**
 * Class that holds all quota info for one mount point.
 */
class LliurexOneDriveWidgetItem
{
public:
    LliurexOneDriveWidgetItem();

    QString fileName() const;
    void setFileName(const QString &fileName);

    QString filePath() const;
    void setFilePath(const QString &filePath);

    QString fileDate() const;
    void setFileDate(const QString &fileDate);

    QString fileTime() const;
    void setFileTime(const QString &fileTime);


    bool operator==(const LliurexOneDriveWidgetItem &other) const;
    bool operator!=(const LliurexOneDriveWidgetItem &other) const;

private:
    QString m_fileName;
    QString m_filePath;
    QString m_fileDate;
    QString m_fileTime;
};

Q_DECLARE_METATYPE(LliurexOneDriveWidgetItem)

#endif // PLASMA_LLIUREX_ONEDRIVE_WIDGET_ITEM_H
