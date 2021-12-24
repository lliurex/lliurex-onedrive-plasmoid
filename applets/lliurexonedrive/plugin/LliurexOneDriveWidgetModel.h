#ifndef PLASMA_LLIUREX_ONEDRIVE_WIDGET_MODEL_H
#define PLASMA_LLIUREX_ONEDRIVE_WIDGET_MODEL_H

#include <QAbstractListModel>
#include <QVector>

#include "LliurexOneDriveWidgetItem.h"

/**
 * Data model holding disk quota items.
 */
class LliurexOneDriveWidgetModel : public QAbstractListModel
{
    Q_OBJECT

public:
    LliurexOneDriveWidgetModel(QObject *parent = nullptr);

public: // QAbstractListModel overrides
    /**
     * List of available roles for the QML ListView.
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * Returns the data for @p index and given @p role.
     */
    QVariant data(const QModelIndex &index, int role) const override;

    /**
     * Returns the number of items for the toplevel model index, otherwise 0.
     */
    int rowCount(const QModelIndex &index) const override;

    /**
     * Changes the data for @p index to @p variant.
     */
    bool setData(const QModelIndex &index, const QVariant &variant, int role = Qt::EditRole) override;

    /**
     * Inserts @p count rows at position @p row.
     */
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    /**
     * Removes @p count rows at position @p row.
     */
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

public: // additional helper functions
    /**
     * Merges @p items into the existing quota item list. Old items that are
     * not available in @p items anymore are deleted.
     */
    void updateItems(const QVector<LliurexOneDriveWidgetItem> &items);

    /**
     * Clears all items in the model.
     */
    void clear();

private:
    QVector<LliurexOneDriveWidgetItem> m_items;
};

#endif // PLASMA_LLIUREX_ONEDRIVE_WIDGET_MODEL_H
