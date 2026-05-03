#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qt_common.h"
#include "sheet_model.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();
    void handleNewDialogData(const SheetMeta& data);
    void on_actionImport_triggered();
    void handleImportDialogData(SheetModel* sheet);
    void on_actionExport_triggered();

    void updateActionsVisibility();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
