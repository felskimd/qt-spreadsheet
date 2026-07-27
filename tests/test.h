#ifndef TEST_H
#define TEST_H

#include <QTest>

#include "../spreadsheet/formula.h"

inline Position operator"" _pos(const char* str, std::size_t) {
    return Position::FromString(str);
}

class SpreadSheetModelTest : public QObject {
    Q_OBJECT

private slots:
    void TestPositionAndStringConversion() {
        auto testSingle = [](Position pos, std::string_view str) {
            QCOMPARE_EQ(pos.ToString(), str);
            QCOMPARE_EQ(Position::FromString(str), pos);
        };

        for (int i = 0; i < 25; ++i) {
            testSingle(Position{ i, i }, char('A' + i) + std::to_string(i + 1));
        }

        testSingle(Position{ 0, 0 }, "A1");
        testSingle(Position{ 0, 1 }, "B1");
        testSingle(Position{ 0, 25 }, "Z1");
        testSingle(Position{ 0, 26 }, "AA1");
        testSingle(Position{ 0, 27 }, "AB1");
        testSingle(Position{ 0, 51 }, "AZ1");
        testSingle(Position{ 0, 52 }, "BA1");
        testSingle(Position{ 0, 53 }, "BB1");
        testSingle(Position{ 0, 77 }, "BZ1");
        testSingle(Position{ 0, 78 }, "CA1");
        testSingle(Position{ 0, 701 }, "ZZ1");
        testSingle(Position{ 0, 702 }, "AAA1");
        testSingle(Position{ 136, 2 }, "C137");
        testSingle(Position{ Position::MAX_ROWS - 1, Position::MAX_COLS - 1 }, "XFD16384");
    }

    void TestPositionToStringInvalid() {
        QCOMPARE_EQ((Position{ -1, -1 }).ToString(), "");
        QCOMPARE_EQ((Position{ -10, 0 }).ToString(), "");
        QCOMPARE_EQ((Position{ 1, -3 }).ToString(), "");
    }

    void TestStringToPositionInvalid() {
        QVERIFY(!Position::FromString("").IsValid());
        QVERIFY(!Position::FromString("A").IsValid());
        QVERIFY(!Position::FromString("1").IsValid());
        QVERIFY(!Position::FromString("e2").IsValid());
        QVERIFY(!Position::FromString("A0").IsValid());
        QVERIFY(!Position::FromString("A-1").IsValid());
        QVERIFY(!Position::FromString("A+1").IsValid());
        QVERIFY(!Position::FromString("R2D2").IsValid());
        QVERIFY(!Position::FromString("C3PO").IsValid());
        QVERIFY(!Position::FromString("XFD16385").IsValid());
        QVERIFY(!Position::FromString("XFE16384").IsValid());
        QVERIFY(!Position::FromString("A1234567890123456789").IsValid());
        QVERIFY(!Position::FromString("ABCDEFGHIJKLMNOPQRS8").IsValid());
    }

    void TestEmpty() {
        auto sheet = CreateSheet();
        QCOMPARE_EQ(sheet->GetPrintableSize(), (Size{ 0, 0 }));
    }

    void TestInvalidPosition() {
        auto sheet = CreateSheet();
        try {
            sheet->SetCell(Position{ -1, 0 }, "");
        }
        catch (const InvalidPositionException&) {
        }
        try {
            sheet->GetCell(Position{ 0, -2 });
        }
        catch (const InvalidPositionException&) {
        }
        try {
            sheet->ClearCell(Position{ Position::MAX_ROWS, 0 });
        }
        catch (const InvalidPositionException&) {
        }
    }

    void TestSetCellPlainText() {
        auto sheet = CreateSheet();

        auto checkCell = [&](Position pos, std::string text) {
            sheet->SetCell(pos, text);
            CellInterface* cell = sheet->GetCell(pos);
            QVERIFY(cell != nullptr);
            QCOMPARE_EQ(cell->GetText(), text);
            QCOMPARE_EQ(std::get<std::string>(cell->GetValue()), text);
        };

        checkCell("A1"_pos, "Hello");
        checkCell("A1"_pos, "World");
        checkCell("B2"_pos, "Purr");
        checkCell("A3"_pos, "Meow");

        const SheetInterface& constSheet = *sheet;
        QCOMPARE_EQ(constSheet.GetCell("B2"_pos)->GetText(), "Purr");

        sheet->SetCell("A3"_pos, "'=escaped");
        CellInterface* cell = sheet->GetCell("A3"_pos);
        QCOMPARE_EQ(cell->GetText(), "'=escaped");
        QCOMPARE_EQ(std::get<std::string>(cell->GetValue()), "=escaped");
    }

    void TestClearCell() {
        auto sheet = CreateSheet();

        sheet->SetCell("C2"_pos, "Me gusta");
        sheet->ClearCell("C2"_pos);
        QVERIFY(sheet->GetCell("C2"_pos) == nullptr);

        sheet->ClearCell("A1"_pos);
        sheet->ClearCell("J10"_pos);
    }

    void TestFormulaArithmetic() {
        auto sheet = CreateSheet();
        auto evaluate = [&](std::string expr) {
            return std::get<double>(ParseFormula(std::move(expr))->Evaluate(*sheet));
        };

        QCOMPARE_EQ(evaluate("1"), 1);
        QCOMPARE_EQ(evaluate("42"), 42);
        QCOMPARE_EQ(evaluate("2 + 2"), 4);
        QCOMPARE_EQ(evaluate("2 + 2*2"), 6);
        QCOMPARE_EQ(evaluate("4/2 + 6/3"), 4);
        QCOMPARE_EQ(evaluate("(2+3)*4 + (3-4)*5"), 15);
        QCOMPARE_EQ(evaluate("(12+13) * (14+(13-24/(1+1))*55-46)"), 575);
    }

    void TestFormulaReferences() {
        auto sheet = CreateSheet();
        auto evaluate = [&](std::string expr) {
            return std::get<double>(ParseFormula(std::move(expr))->Evaluate(*sheet));
        };

        sheet->SetCell("A1"_pos, "1");
        QCOMPARE_EQ(evaluate("A1"), 1);
        sheet->SetCell("A2"_pos, "2");
        QCOMPARE_EQ(evaluate("A1+A2"), 3);

        // Тест на нули:
        sheet->SetCell("B3"_pos, "");
        QCOMPARE_EQ(evaluate("A1+B3"), 1);  // Ячейка с пустым текстом
        QCOMPARE_EQ(evaluate("A1+B1"), 1);  // Пустая ячейка
        QCOMPARE_EQ(evaluate("A1+E4"), 1);  // Ячейка за пределами таблицы
    }

    void TestFormulaExpressionFormatting() {
        auto reformat = [](std::string expr) {
            return ParseFormula(std::move(expr))->GetExpression();
        };

        QCOMPARE_EQ(reformat("  1  "), "1");
        QCOMPARE_EQ(reformat("  -1  "), "-1");
        QCOMPARE_EQ(reformat("2 + 2"), "2+2");
        QCOMPARE_EQ(reformat("(2*3)+4"), "2*3+4");
        QCOMPARE_EQ(reformat("(2*3)-4"), "2*3-4");
        QCOMPARE_EQ(reformat("( ( (  1) ) )"), "1");
    }

    void TestFormulaReferencedCells() {
        QVERIFY(ParseFormula("1")->GetReferencedCells().empty());

        auto a1 = ParseFormula("A1");
        QCOMPARE_EQ(a1->GetReferencedCells(), (std::vector{ "A1"_pos }));

        auto b2c3 = ParseFormula("B2+C3");
        QCOMPARE_EQ(b2c3->GetReferencedCells(), (std::vector{ "B2"_pos, "C3"_pos }));

        auto tricky = ParseFormula("A1 + A2 + A1 + A3 + A1 + A2 + A1");
        QCOMPARE_EQ(tricky->GetExpression(), "A1+A2+A1+A3+A1+A2+A1");
        QCOMPARE_EQ(tricky->GetReferencedCells(), (std::vector{ "A1"_pos, "A2"_pos, "A3"_pos }));
    }

    void TestErrorValue() {
        auto sheet = CreateSheet();
        sheet->SetCell("E2"_pos, "A1");
        sheet->SetCell("E4"_pos, "=E2");
        QCOMPARE_EQ(sheet->GetCell("E4"_pos)->GetValue(),
                     CellInterface::Value(FormulaError::Category::Value));

        sheet->SetCell("E2"_pos, "3D");
        QCOMPARE_EQ(sheet->GetCell("E4"_pos)->GetValue(),
                     CellInterface::Value(FormulaError::Category::Value));
    }

    void TestErrorArithmetic() {
        auto sheet = CreateSheet();

        constexpr double max = std::numeric_limits<double>::max();

        sheet->SetCell("A1"_pos, "=1/0");
        QCOMPARE_EQ(sheet->GetCell("A1"_pos)->GetValue(),
                     CellInterface::Value(FormulaError::Category::Arithmetic));

        sheet->SetCell("A1"_pos, "=1e+200/1e-200");
        QCOMPARE_EQ(sheet->GetCell("A1"_pos)->GetValue(),
                     CellInterface::Value(FormulaError::Category::Arithmetic));

        sheet->SetCell("A1"_pos, "=0/0");
        QCOMPARE_EQ(sheet->GetCell("A1"_pos)->GetValue(),
                     CellInterface::Value(FormulaError::Category::Arithmetic));

        {
            std::ostringstream formula;
            formula << '=' << max << '+' << max;
            sheet->SetCell("A1"_pos, formula.str());
            QCOMPARE_EQ(sheet->GetCell("A1"_pos)->GetValue(),
                         CellInterface::Value(FormulaError::Category::Arithmetic));
        }

        {
            std::ostringstream formula;
            formula << '=' << -max << '-' << max;
            sheet->SetCell("A1"_pos, formula.str());
            QCOMPARE_EQ(sheet->GetCell("A1"_pos)->GetValue(),
                         CellInterface::Value(FormulaError::Category::Arithmetic));
        }

        {
            std::ostringstream formula;
            formula << '=' << max << '*' << max;
            sheet->SetCell("A1"_pos, formula.str());
            QCOMPARE_EQ(sheet->GetCell("A1"_pos)->GetValue(),
                         CellInterface::Value(FormulaError::Category::Arithmetic));
        }
    }

    void TestEmptyCellTreatedAsZero() {
        auto sheet = CreateSheet();
        sheet->SetCell("A1"_pos, "=B2");
        QCOMPARE_EQ(sheet->GetCell("A1"_pos)->GetValue(), CellInterface::Value(0.0));
    }

    void TestFormulaInvalidPosition() {
        auto sheet = CreateSheet();
        auto try_formula = [&](const std::string& formula) {
            try {
                sheet->SetCell("A1"_pos, formula);
                QVERIFY(false);
            }
            catch (const FormulaException&) {
                // we expect this one
            }
        };

        try_formula("=X0");
        try_formula("=ABCD1");
        try_formula("=A123456");
        try_formula("=ABCDEFGHIJKLMNOPQRS1234567890");
        try_formula("=XFD16385");
        try_formula("=XFE16384");
        try_formula("=R2D2");
    }

    void TestPrint() {
        auto sheet = CreateSheet();
        sheet->SetCell("A2"_pos, "meow");
        sheet->SetCell("B2"_pos, "=35");

        QCOMPARE_EQ(sheet->GetPrintableSize(), (Size{ 2, 2 }));

        std::ostringstream texts;
        sheet->PrintTexts(texts);
        QCOMPARE_EQ(texts.str(), "\t\nmeow\t=35\n");

        std::ostringstream values;
        sheet->PrintValues(values);
        QCOMPARE_EQ(values.str(), "\t\nmeow\t35\n");
    }

    void TestCellReferences() {
        auto sheet = CreateSheet();
        sheet->SetCell("A1"_pos, "1");
        sheet->SetCell("A2"_pos, "=A1");
        sheet->SetCell("B2"_pos, "=A1");

        QVERIFY(sheet->GetCell("A1"_pos)->GetReferencedCells().empty());
        QCOMPARE_EQ(sheet->GetCell("A2"_pos)->GetReferencedCells(), std::vector{ "A1"_pos });
        QCOMPARE_EQ(sheet->GetCell("B2"_pos)->GetReferencedCells(), std::vector{ "A1"_pos });

        // Ссылка на пустую ячейку
        sheet->SetCell("B2"_pos, "=B1");
        QVERIFY(sheet->GetCell("B1"_pos)->GetReferencedCells().empty());
        QCOMPARE_EQ(sheet->GetCell("B2"_pos)->GetReferencedCells(), std::vector{ "B1"_pos });

        sheet->SetCell("A2"_pos, "");
        QVERIFY(sheet->GetCell("A1"_pos)->GetReferencedCells().empty());
        QVERIFY(sheet->GetCell("A2"_pos)->GetReferencedCells().empty());

        // Ссылка на ячейку за пределами таблицы
        sheet->SetCell("B1"_pos, "=C3");
        QCOMPARE_EQ(sheet->GetCell("B1"_pos)->GetReferencedCells(), std::vector{ "C3"_pos });
    }

    void TestFormulaIncorrect() {
        auto isIncorrect = [](std::string expression) {
            try {
                ParseFormula(std::move(expression));
            }
            catch (const FormulaException&) {
                return true;
            }
            return false;
        };

        QVERIFY(isIncorrect("A2B"));
        QVERIFY(isIncorrect("3X"));
        QVERIFY(isIncorrect("A0++"));
        QVERIFY(isIncorrect("((1)"));
        QVERIFY(isIncorrect("2+4-"));
    }

    void TestCellCircularReferences() {
        auto sheet = CreateSheet();
        sheet->SetCell("E2"_pos, "=E4");
        sheet->SetCell("E4"_pos, "=X9");
        sheet->SetCell("X9"_pos, "=M6");
        sheet->SetCell("M6"_pos, "Ready");

        bool caught = false;
        try {
            sheet->SetCell("M6"_pos, "=E2");
        }
        catch (const CircularDependencyException&) {
            caught = true;
        }

        QVERIFY(caught);
        QCOMPARE_EQ(sheet->GetCell("M6"_pos)->GetText(), "Ready");
    }
};

QTEST_MAIN(SpreadSheetModelTest)

#endif // TEST_H
