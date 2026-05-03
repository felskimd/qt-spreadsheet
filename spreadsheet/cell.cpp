#include "cell.h"

#include <cassert>
#include <string>
#include <optional>

Cell::Cell(SheetInterface& sheet, Position pos)
    :impl_(std::make_unique<EmptyImpl>()),
    sheet_(sheet),
    pos_(pos)
{ }

Cell::~Cell() {}

void Cell::Set(std::string text) {
    if (impl_->GetText() == text) {
        return;
    }
    std::unique_ptr<Impl>temp;
    if (text.size() == 0) {
        temp = std::make_unique<EmptyImpl>(); 
    }
    else if (text[0] == FORMULA_SIGN && text.size() > 1) {
        auto formula = ParseFormula(text.substr(1));
        if (CreatesCyclicDependencies(formula.get())) {
            throw CircularDependencyException("Creates circular exception on \"" + text + '"');
        }
        temp = std::make_unique<FormulaImpl>(std::move(formula));
    }
    else if (text[0] == ESCAPE_SIGN) {
        temp = std::make_unique<TextImpl>(std::move(text), true);
    }
    else {
        temp = std::make_unique<TextImpl>(std::move(text), false);
    }
    InvalidateCache();
    ResetParentToChilds();
    impl_.swap(temp);
    SetParentToChilds();
}

void Cell::Clear() {
    InvalidateCache();
    ResetParentToChilds();
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue(sheet_);
}

std::string Cell::GetStringValue() const {
    const auto value = GetValue();
    if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    }
    else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    }
    else {
        return std::get<FormulaError>(value).ToString().data();
    }
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
}

void Cell::InvalidateCache() const {
    std::unordered_set<Position> visited;
    InvalidationDFS(visited);
}

void Cell::InvalidationDFS(std::unordered_set<Position>& visited) const {
    visited.insert(pos_);
    impl_->InvalidateCache();
    for (const auto parent : parents_) {
        if (visited.count(parent) == 0) {
            const auto* cell = reinterpret_cast<const Cell*>(sheet_.GetCell(parent));
            if (cell) {
                cell->InvalidationDFS(visited);
            }
        }
    }
}

bool Cell::CreatesCyclicDependencies(FormulaInterface* formula) const {
    std::unordered_set<Position> visited;
    std::unordered_set<Position> path;
    return CyclicDependenciesDFS(visited, path, formula);
}

bool Cell::CyclicDependenciesDFS(std::unordered_set<Position>& visited,
            std::unordered_set<Position>& path,
            FormulaInterface* formula) const {
    visited.insert(pos_);
    path.insert(pos_);
    if (formula) {
        for (const auto child : formula->GetReferencedCells()) {
            if (path.count(child) == 1) {
                return true;
            }
            const auto* cell = reinterpret_cast<const Cell*>(sheet_.GetCell(child));
            if (cell && cell->CyclicDependenciesDFS(visited, path, nullptr)) {
                return true;
            }
            //return false;
        }
    }
    for (const auto child : GetReferencedCells()) {
        if (path.count(child) == 1) {
            return true;
        }
        if (visited.count(child) == 0) {
            const auto* cell = reinterpret_cast<const Cell*>(sheet_.GetCell(child));
            if (cell && cell->CyclicDependenciesDFS(visited, path, nullptr)) {
                return true;
            }
        }
    }
    path.erase(pos_);
    return false;
}

void Cell::SetParentToChilds() const {
    for (auto child : impl_->GetReferencedCells()) {
        auto* cell_int = sheet_.GetCell(child);
        if (!cell_int) {
            sheet_.SetCell(child, "");
        }
        auto* cell = reinterpret_cast<Cell*>(sheet_.GetCell(child));
        cell->SetParent(pos_);
    }
}

void Cell::ResetParentToChilds() const {
    for (auto child : impl_->GetReferencedCells()) {
        auto* cell = reinterpret_cast<Cell*>(sheet_.GetCell(child));
        cell->ResetParent(pos_);
    }
}

void Cell::SetParent(Position pos) {
    parents_.insert(pos);
}

void Cell::ResetParent(Position pos) {
    parents_.erase(pos);
}

std::vector<Position> Cell::Impl::GetReferencedCells() const {
    return std::vector<Position>();
}

CellInterface::Value Cell::EmptyImpl::GetValue(const SheetInterface& /*sheet*/) const {
    return "";
}

std::string Cell::EmptyImpl::GetText() const {
    return "";
}

CellInterface::Value Cell::TextImpl::GetValue(const SheetInterface& /*sheet*/) const {
    if (apo_) {
        return value_.substr(1);
    }
    else {
        return value_;
    }
}

std::string Cell::TextImpl::GetText() const {
    if (apo_ && value_.size() == 1) {
        return "\'";
    }
    else {
        return value_;
    }
}

CellInterface::Value Cell::FormulaImpl::GetValue(const SheetInterface& sheet) const {
    auto result = value_->Evaluate(sheet);
    if (std::holds_alternative<double>(result)) {
        return std::get<double>(result);
    }
    else {
        return std::get<FormulaError>(result);
    }
}

std::string Cell::FormulaImpl::GetText() const {
    return "=" + value_->GetExpression();
}

std::vector<Position> Cell::FormulaImpl::GetReferencedCells() const {
    return value_->GetReferencedCells();
}

void Cell::FormulaImpl::InvalidateCache() const {
    if (cache_) {
        cache_.reset();
    }
}
