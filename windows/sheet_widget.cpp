#include "sheet_widget.h"

SheetWidget::SheetWidget(QWidget *parent)
    : QTableView{parent}
{}

void SheetWidget::MoveItems(Direction dir) {
    QModelIndexList indexes = selectedIndexes();

    if (indexes.empty()) {
        return;
    }

    for (const auto& index : std::as_const(indexes)) {

        auto new_index = model()->index(index.row(), index.column() + 1);
        model()->setData(new_index, index.data(Qt::EditRole));
    }
}

void SheetWidget::DeleteItems() {
    QModelIndexList indexes = selectedIndexes();

    if (indexes.empty()) {
        return;
    }

    for (const auto& index : std::as_const(indexes)) {
        model()->setData(index, QVariant());
    }
}

QByteArray SheetWidget::CopySelected() const {
    QByteArray data;
    QModelIndexList indexes = selectedIndexes();
    if (indexes.empty()) {
        return data;
    }
    const QModelIndex *prev = nullptr;
    bool first = true;
    for (const auto& index : std::as_const(indexes)) {
        if (prev && prev->row() != index.row()) {
            if (!first) {
                //Перевод строки таблицы
                data.push_back('\n');
            }
            first = true;
        }
        if (first) {
            first = false;
        }
        else {
            data.push_back('\t');
        }
        data.push_back(index.data(Qt::EditRole).toByteArray());
        prev = &index;
    }
    return data;
}
