#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}

namespace {
    class Formula : public FormulaInterface {
    public:
        explicit Formula(std::string expression) try
            : ast_(ParseFormulaAST(expression)) {
        }
        catch (...) {
            throw FormulaException("");
        }

        Value Evaluate(const SheetInterface& sheet) const override {
            try {
                return ast_.Execute(sheet);
            }
            catch (const FormulaError& fe) {
                return fe;
            }
        }

        std::string GetExpression() const override {
            std::ostringstream formula_string_stream;
            ast_.PrintFormula(formula_string_stream);
            return formula_string_stream.str();
        }

        virtual std::vector<Position> GetReferencedCells() const override {
            auto cells = ast_.GetCells();
            std::vector<Position> result;
            for (const auto cell : cells) {
                if (result.size() == 0 || !(result[result.size() - 1] == cell)) {
                    result.push_back(cell);
                }
            }
            return result;
        }

    private:
        FormulaAST ast_;
    };
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}