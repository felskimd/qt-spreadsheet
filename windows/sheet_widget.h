#ifndef SHEET_WIDGET_H
#define SHEET_WIDGET_H

#include <QTableView>

enum Direction {
    up,
    down,
    right,
    left
};

class SheetWidget : public QTableView
{
    Q_OBJECT
public:
    explicit SheetWidget(QWidget *parent = nullptr);

    void MoveItems(Direction dir);
    void DeleteItems();
    QByteArray CopySelected() const;

signals:
};

#endif // SHEET_WIDGET_H
