#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <functional>
#include <iostream>
#include <utility>

using namespace std::literals;

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("");
    }
    if (cells_.count(pos) == 0) {
        cells_[pos] = std::make_unique<Cell>(*this, pos);
    }
    cells_[pos]->Set(text);
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (pos.IsValid()) {
        if (const auto found = cells_.find(pos); found != cells_.end()) {
            return found->second.get();
        }
        else {
            return nullptr;
        }
    }
    else {
        throw InvalidPositionException("");
    }
}

CellInterface* Sheet::GetCell(Position pos) {
    return const_cast<CellInterface*>(
        static_cast<const Sheet&>(*this).GetCell(pos)
    );
}

void Sheet::ClearCell(Position pos) {
    if (pos.IsValid()) {
        cells_.erase(pos);
    }
    else {
        throw InvalidPositionException("");
    }
}

Size Sheet::GetPrintableSize() const {
    int min_rows = 0;
    int min_cols = 0;
    for (const auto& [pos, val] : cells_) {
        if (pos.row + 1 > min_rows) {
            min_rows = pos.row + 1;
        }
        if (pos.col + 1 > min_cols) {
            min_cols = pos.col + 1;
        }
    }
    return { min_rows, min_cols };
}

void Sheet::PrintValues(std::ostream& output) const {
    PrintCells(output, [&](const CellInterface& cell) {
        const auto value = cell.GetValue();
        if (std::holds_alternative<double>(value)) {
            output << std::get<double>(value);
        }
        else if (std::holds_alternative<std::string>(value)) {
            output << std::get<std::string>(value);
        }
        else {
            output << std::get<FormulaError>(value);
        }
    });
}

void Sheet::PrintTexts(std::ostream& output) const {
    PrintCells(output, [&](const CellInterface& cell) {
        output << cell.GetText();
        });
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}

void Sheet::PrintCells(std::ostream& output,
    const std::function<void(const CellInterface&)>& printCell) const {

    const auto min = Sheet::GetPrintableSize();
    for (int i = 0; i < min.rows; ++i) {
        bool first = true;
        for (int j = 0; j < min.cols; ++j) {
            if (!first) {
                output << "\t";
            }
            else {
                first = false;
            }
            Position pos = { i,j };
            if (cells_.count(pos) != 0) {
                printCell(*cells_.at(pos).get());
            }
        }
        output << "\n";
    }
}
