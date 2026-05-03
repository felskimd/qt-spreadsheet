#ifndef SHEET_MODEL_H
#define SHEET_MODEL_H

#include <QAbstractTableModel>
#include <QHeaderView>
#include <vector>

#include "../spreadsheet/sheet.h"
#include "../spreadsheet/common.h"
#include "qt_common.h"

Position ModelIndexToPosition(const QModelIndex& index);

class SheetModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit SheetModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void SetMeta(const SheetMeta& meta);

    const QString& GetName() const;
    const Sheet& GetSheet() const;
    Sheet& GetSheet();

private:
    Sheet sheet_;
    SheetMeta meta_;
};

#endif // SHEET_MODEL_H
