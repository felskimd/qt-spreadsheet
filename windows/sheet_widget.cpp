#include "sheet_widget.h"

SheetWidget::SheetWidget(QWidget *parent)
    : QTableView{parent}
{}

void SheetWidget::MoveItems(Direction dir) {
    QModelIndexList indexes = selectedIndexes();

    if (indexes.empty()) {
        return;
    }

    for (const auto& index : indexes) {
        auto new_index = model()->index(index.row(), index.column() + 1);
        model()->setData(new_index, index.data(Qt::EditRole));
    }
}

void SheetWidget::DeleteItems() {
    QModelIndexList indexes = selectedIndexes();

    if (indexes.empty()) {
        return;
    }

    for (const auto& index : indexes) {
        model()->setData(index, QVariant());
    }
}
