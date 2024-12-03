
#include "LliurexOneDriveWidgetSpacesModel.h"

#include <QDebug>

LliurexOneDriveWidgetSpacesModel::LliurexOneDriveWidgetSpacesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

namespace {
    /**
     * QML data roles.
     */
    enum {
        IdRole = Qt::UserRole,
        NameRole,
        StatusRole,
        IsRunningRole,
        LocalFolderWarningRole,
        UpdateRequiredWarningRole
    };
}

QHash<int, QByteArray> LliurexOneDriveWidgetSpacesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[StatusRole] = "status";
    roles[IsRunningRole] = "isRunning";
    roles[LocalFolderWarningRole] = "localFolderWarning";
    roles[UpdateRequiredWarningRole] = "updateRequiredWarning";

    return roles;
}

QVariant LliurexOneDriveWidgetSpacesModel::data(const QModelIndex &index, int role) const
{
    if (! index.isValid() || index.row() >= m_items.size()) {
        return QVariant();
    }

    const auto item = m_items[index.row()];

    switch (role) {
        case IdRole: return item.id();
        case NameRole: return item.name();
        case StatusRole: return item.status();
        case IsRunningRole: return item.isRunning();
        case LocalFolderWarningRole: return item.localFolderWarning();
        case UpdateRequiredWarningRole: return item.updateRequiredWarning();
       
    }

    return QVariant();
}

int LliurexOneDriveWidgetSpacesModel::rowCount(const QModelIndex &index) const
{
    if (! index.isValid()) {
        return m_items.size();
    }

    return 0;
}

bool LliurexOneDriveWidgetSpacesModel::setData(const QModelIndex &index, const QVariant &variant, int role)
{
    Q_UNUSED(role)

    const int row = index.row();
    if (index.isValid() && row < m_items.size()) {
        const LliurexOneDriveWidgetSpaceItem item = variant.value<LliurexOneDriveWidgetSpaceItem>();

        // This assert makes sure that changing items modify the correct item:
        // therefore, the unique identifier 'filePath()' is used. If that
        // is not the case, the newly inserted row must have an empty filePath().
        Q_ASSERT(item.id() == m_items[row].id()
            || m_items[row].id().isEmpty());

        if (m_items[row] != item) {
            m_items[row] = item;
            emit dataChanged(index, index);
            return true;
        }
    }

    return false;
}

bool LliurexOneDriveWidgetSpacesModel::insertRows(int row, int count, const QModelIndex &parent)
{
    // only top-level items are supported
    if (parent.isValid()) {
        return false;
    }

    beginInsertRows(QModelIndex(), row, row + count - 1);
    m_items.insert(row, count, LliurexOneDriveWidgetSpaceItem());
    endInsertRows();

    return true;
}

bool LliurexOneDriveWidgetSpacesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    // only top-level items are valid
    if (parent.isValid() || (row + count) >= m_items.size()) {
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row + count - 1);
    m_items.remove(row, count);
    endRemoveRows();

    return true;
}

void LliurexOneDriveWidgetSpacesModel::clear()
{
    beginResetModel();
    m_items.clear();
    endResetModel();
}

namespace {
    QStringList spacesId(const QVector<LliurexOneDriveWidgetSpaceItem> &items)
    {
        QStringList list;
        for (auto & item : items) {
            list.append(item.id());
        }
        return list;
    }

    int indexOfSpaceId(const QString &id, const QVector<LliurexOneDriveWidgetSpaceItem> &items)
    {
        for (int i = 0; i < items.size(); ++i) {
            if (id == items[i].id()) {
                return i;
            }
        }
        return -1;
    }
}

void LliurexOneDriveWidgetSpacesModel::updateItems(const QVector<LliurexOneDriveWidgetSpaceItem> &items)
{
    QStringList unusedSpaceIds = spacesId(m_items);
    // merge existing and new file paths
    for (auto & item : items) {
        // remove still used item from unused list
        unusedSpaceIds.removeOne(item.id());
        // insert or modify m_items
        int row = indexOfSpaceId(item.id(), m_items);
        if (row < 0) {
            // new item: append on end
            row = m_items.size();
            insertRow(row);
        }
        setData(createIndex(row, 0), QVariant::fromValue(item));
    }

    // remove mount points, that do not exist anymore

    for (const auto & id : unusedSpaceIds) {
        const int row = indexOfSpaceId(id, m_items);
        Q_ASSERT(row >= 0);
        removeRow(row);
    
    }
}
