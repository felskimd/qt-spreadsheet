#pragma once

#include "common.h"
#include "formula.h"

#include <functional>
#include <optional>
#include <unordered_set>

class Sheet;

class Cell : public CellInterface {
public:
    Cell(SheetInterface& sheet, Position pos);
    ~Cell();

    void Set(std::string text);
    void Clear();
    Position GetPosition() const { return pos_; }
    Value GetValue() const override;
    std::string GetStringValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
    SheetInterface& sheet_;
    std::unordered_set<Position> parents_;
    Position pos_;

    void InvalidateCache() const;
    void InvalidationDFS(std::unordered_set<Position>& visited) const;
    bool CreatesCyclicDependencies(FormulaInterface* formula) const;
    bool CyclicDependenciesDFS(std::unordered_set<Position>& visited,
                               std::unordered_set<Position>& path,
                               FormulaInterface* formula) const;
    void SetParent(Position pos);
    void ResetParent(Position pos);
    void SetParentToChilds() const;
    void ResetParentToChilds() const;

    class Impl {
    public:
        virtual Value GetValue(const SheetInterface& sheet) const = 0;
        virtual std::string GetText() const = 0;
        virtual std::vector<Position> GetReferencedCells() const;
        virtual void InvalidateCache() const { }
    };

    class EmptyImpl : public Impl {
    public:
        Value GetValue(const SheetInterface& /*sheet*/) const override;
        std::string GetText() const override;
    };

    class TextImpl : public Impl {
    public:
        TextImpl(std::string&& value, bool apo)
            :value_(std::move(value)), apo_(apo)
        { }

        Value GetValue(const SheetInterface& /*sheet*/) const override;
        std::string GetText() const override;

    private:
        std::string value_;
        bool apo_;
    };

    class FormulaImpl : public Impl {
    public:
        FormulaImpl(std::unique_ptr<FormulaInterface>&& value)
            :value_(std::move(value))
        { }

        Value GetValue(const SheetInterface& sheet) const override;
        std::string GetText() const override;
        std::vector<Position> GetReferencedCells() const override;
        void InvalidateCache() const override;

    private:
        std::unique_ptr<FormulaInterface> value_;
        mutable std::optional<FormulaInterface::Value> cache_;
    };
};
