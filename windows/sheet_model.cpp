#include <QMessageBox>

#include "sheet_model.h"
#include "../spreadsheet/common.h"

Position ModelIndexToPosition(const QModelIndex& index) {
    return {.row = index.row(), .col = index.column()};
}

SheetModel::SheetModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

QVariant SheetModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        // Заголовки столбцов
        if (section < Position::MAX_COLS)
            return QString::fromStdString(ColumnToString(section));
    } else {
        // Заголовки строк (номера строк)
        return QString::number(section + 1);
    }

    return QVariant();
}

int SheetModel::rowCount(const QModelIndex &parent) const {
    //return meta_.lines;
    return Position::MAX_ROWS;
}

int SheetModel::columnCount(const QModelIndex &parent) const {
    //return meta_.cols;
    return Position::MAX_COLS;
}

QVariant SheetModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();
    auto* cell = sheet_.GetCell(ModelIndexToPosition(index));
    if (cell) {
        if (role == Qt::DisplayRole)
            return QString::fromStdString(cell->GetStringValue());
        if (role == Qt::EditRole)
            return QString::fromStdString(cell->GetText());
    }
    return QVariant();
}

bool SheetModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || role != Qt::EditRole)
        return false;
    try {
        sheet_.SetCell(ModelIndexToPosition(index), value.toString().toStdString());
    }
    catch(CircularDependencyException ex) {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Critical);
        msg.setText(ex.what());
        msg.exec();
        return false;
    }

    emit dataChanged(index, index, {role});
    return true;
}

Qt::ItemFlags SheetModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void SheetModel::SetMeta(const SheetMeta& meta) {
    meta_ = meta;
}

const QString& SheetModel::GetName() const {
    return meta_.name;
}

const Sheet& SheetModel::GetSheet() const {
    return sheet_;
}

Sheet& SheetModel::GetSheet() {
    return sheet_;
}
