#ifndef TEST_H
#define TEST_H

#include <QTest>

class SpreadSheetModelTest : public QObject {
    Q_OBJECT

private slots:
    void testAddition();
};

void SpreadSheetModelTest::testAddition() {
    QCOMPARE(2 + 2, 4);
}

QTEST_MAIN(SpreadSheetModelTest)

#endif // TEST_H
