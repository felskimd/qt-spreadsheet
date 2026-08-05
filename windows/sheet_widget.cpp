#include "sheet_widget.h"
#include "sheet_model.h"

#include <QMimeData>

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

    const auto &sheet = reinterpret_cast<SheetModel*>(model())->GetSheet();
    Size sheet_size = sheet.GetPrintableSize();
    int start_row = indexes.first().row();
    int start_col = indexes.first().column();
    int end_row = indexes.last().row();
    int end_col = indexes.last().column();
    if (end_row > sheet_size.rows - 1) {
        end_row = sheet_size.rows - 1;
    }
    if (end_col > sheet_size.cols - 1) {
        end_col = sheet_size.cols - 1;
    }

    for (int row = start_row; row <= end_row; ++row) {
        bool first_col = true;
        for (int col = start_col; col <= end_col; ++col) {
            model()->setData(model()->index(row, col), QVariant());
        }
    }
}

QByteArray SheetWidget::CopySelected() const {
    QByteArray data;
    //selectionModel()->currentIndex()
    QModelIndexList indexes = selectedIndexes();
    if (indexes.empty()) {
        return data;
    }
    const auto &sheet = reinterpret_cast<SheetModel*>(model())->GetSheet();
    Size sheet_size = sheet.GetPrintableSize();

    int start_row = indexes.first().row();
    int start_col = indexes.first().column();
    int end_row = indexes.last().row();
    int end_col = indexes.last().column();
    if (end_row > sheet_size.rows - 1) {
        end_row = sheet_size.rows - 1;
    }
    if (end_col > sheet_size.cols - 1) {
        end_col = sheet_size.cols - 1;
    }

    bool first_row = true;
    for (int row = start_row; row <= end_row; ++row) {
        if (first_row) {
            first_row = false;
        }
        else {
            data.push_back('\n');
        }
        bool first_col = true;
        for (int col = start_col; col <= end_col; ++col) {
            if (first_col) {
                first_col = false;
            }
            else {
                data.push_back('\t');
            }
            auto parsed = model()->data(model()->index(row, col), Qt::EditRole).toString().remove('\n');
            data.push_back(parsed.toStdString().data());
        }
    }

    return data;
}

void SheetWidget::Paste(const QMimeData *data) const {
    QModelIndexList indexes = selectedIndexes();
    if (indexes.empty() || !data->hasText()) {
        return;
    }
    int start_row = indexes.first().row();
    int start_col = indexes.first().column();
    auto text = data->text();
    auto rows = text.split('\n');
    int i = 0;
    for (const auto& row : std::as_const(rows)) {
        auto items = row.split('\t');
        int j = 0;
        for (const auto& item : std::as_const(items)) {
            model()->setData(model()->index(start_row + i, start_col + j), QVariant(item));
            ++j;
        }
        ++i;
    }
}
