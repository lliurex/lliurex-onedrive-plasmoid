#ifndef PLASMA_LLIUREX_ONEDRIVE_WIDGET_SPACE_ITEM_H
#define PLASMA_LLIUREX_ONEDRIVE_WIDGET_SPACE_ITEM_H

#include <QString>
#include <QMetaType>

/**
 * Class that holds all space info.
 */
class LliurexOneDriveWidgetSpaceItem
{
public:
    LliurexOneDriveWidgetSpaceItem();

    QString id() const;
    void setId(const QString &id);

    QString name() const;
    void setName(const QString &name);

    QString status() const;
    void setStatus(const QString &status);

    bool isRunning() const;
    void setIsRunning(const bool &isRunning);

    bool localFolderWarning() const;
    void setLocalFolderWarning(const bool &localFolderWarning);

    bool operator==(const LliurexOneDriveWidgetSpaceItem &other) const;
    bool operator!=(const LliurexOneDriveWidgetSpaceItem &other) const;

private:
    QString m_id;
    QString m_name;
    QString m_status;
    bool m_isRunning;
    bool m_localFolderWarning;
};

Q_DECLARE_METATYPE(LliurexOneDriveWidgetSpaceItem)

#endif // PLASMA_LLIUREX_ONEDRIVE_WIDGET_SPACE_ITEM_H
