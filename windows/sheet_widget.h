#ifndef SHEET_WIDGET_H
#define SHEET_WIDGET_H

#include <QTableView>

class SheetWidget : public QTableView
{
    Q_OBJECT
public:
    explicit SheetWidget(QWidget *parent = nullptr);

signals:
};

#endif // SHEET_WIDGET_H
