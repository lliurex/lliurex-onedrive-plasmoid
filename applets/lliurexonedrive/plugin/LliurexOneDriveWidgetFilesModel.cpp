
#include "LliurexOneDriveWidgetFilesModel.h"

#include <QDebug>

LliurexOneDriveWidgetFilesModel::LliurexOneDriveWidgetFilesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

namespace {
    /**
     * QML data roles.
     */
    enum {
        FileNameRole = Qt::UserRole,
        FilePathRole,
        FileDateRole,
        FileTimeRole
    };
}

QHash<int, QByteArray> LliurexOneDriveWidgetFilesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FileNameRole] = "fileName";
    roles[FilePathRole] = "filePath";
    roles[FileDateRole] = "fileDate";
    roles[FileTimeRole] = "fileTime";

    return roles;
}

QVariant LliurexOneDriveWidgetFilesModel::data(const QModelIndex &index, int role) const
{
    if (! index.isValid() || index.row() >= m_items.size()) {
        return QVariant();
    }

    const auto item = m_items[index.row()];

    switch (role) {
        case FileNameRole: return item.fileName();
        case FilePathRole: return item.filePath();
        case FileDateRole: return item.fileDate();
        case FileTimeRole: return item.fileTime();
       
    }

    return QVariant();
}

int LliurexOneDriveWidgetFilesModel::rowCount(const QModelIndex &index) const
{
    if (! index.isValid()) {
        return m_items.size();
    }

    return 0;
}

bool LliurexOneDriveWidgetFilesModel::setData(const QModelIndex &index, const QVariant &variant, int role)
{
    Q_UNUSED(role)

    const int row = index.row();
    if (index.isValid() && row < m_items.size()) {
        const LliurexOneDriveWidgetFileItem item = variant.value<LliurexOneDriveWidgetFileItem>();

        // This assert makes sure that changing items modify the correct item:
        // therefore, the unique identifier 'filePath()' is used. If that
        // is not the case, the newly inserted row must have an empty filePath().
        Q_ASSERT(item.filePath() == m_items[row].filePath()
            || m_items[row].filePath().isEmpty());

        if (m_items[row] != item) {
            m_items[row] = item;
            emit dataChanged(index, index);
            return true;
        }
    }

    return false;
}

bool LliurexOneDriveWidgetFilesModel::insertRows(int row, int count, const QModelIndex &parent)
{
    // only top-level items are supported
    if (parent.isValid()) {
        return false;
    }

    beginInsertRows(QModelIndex(), row, row + count - 1);
    m_items.insert(row, count, LliurexOneDriveWidgetFileItem());
    endInsertRows();

    return true;
}

bool LliurexOneDriveWidgetFilesModel::removeRows(int row, int count, const QModelIndex &parent)
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

void LliurexOneDriveWidgetFilesModel::clear()
{
    beginResetModel();
    m_items.clear();
    endResetModel();
}

namespace {
    QStringList filePaths(const QVector<LliurexOneDriveWidgetFileItem> &items)
    {
        QStringList list;
        for (auto & item : items) {
            list.append(item.filePath());
        }
        return list;
    }

    int indexOfFilePath(const QString &filePath, const QVector<LliurexOneDriveWidgetFileItem> &items)
    {
        for (int i = 0; i < items.size(); ++i) {
            if (filePath == items[i].filePath()) {
                return i;
            }
        }
        return -1;
    }
}

void LliurexOneDriveWidgetFilesModel::updateItems(const QVector<LliurexOneDriveWidgetFileItem> &items)
{
    QStringList unusedFilePaths = filePaths(m_items);
    // merge existing and new file paths
    for (auto & item : items) {
        // remove still used item from unused list
        unusedFilePaths.removeOne(item.filePath());
        // insert or modify m_items
        int row = indexOfFilePath(item.filePath(), m_items);
        if (row < 0) {
            // new item: append on end
            row = m_items.size();
            insertRow(row);
        }
        setData(createIndex(row, 0), QVariant::fromValue(item));
    }

    // remove mount points, that do not exist anymore

    for (const auto & filePath : unusedFilePaths) {
        const int row = indexOfFilePath(filePath, m_items);
        Q_ASSERT(row >= 0);
        removeRow(row);
    
    }
}
