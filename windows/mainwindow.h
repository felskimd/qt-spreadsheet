#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUndoView>
#include <QUndoStack>
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

    void updateFileActionsVisibility();
    void updateEditActionsVisibility();
    void showContextMenu(const QPoint &pos);

    void on_actionClear_triggered();

private:
    void createActions();
    void createMenus();
    void createUndoView();

    QAction *undoAction = nullptr;
    QAction *redoAction = nullptr;
    //other actions

    QUndoStack *undoStack = nullptr;
    QUndoView *undoView = nullptr;
    QMenu *sheetContextMenu = nullptr;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
