#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdialog.h"
#include "importdialog.h"
#include "exportdialog.h"

#include <QClipboard>
#include <QMimeData>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //createActions();
    createMenus();
    //createUndoView();

    updateFileActionsVisibility();
    //setup tableView
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, &QTableView::customContextMenuRequested, this, &MainWindow::showContextMenu);
    connect(ui->menuEdit, &QMenu::aboutToShow, this, &MainWindow::updateEditActionsVisibility);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::createMenus() {
    sheetContextMenu = new QMenu(this);
    sheetContextMenu->addActions(ui->menuEdit->actions());
}

void MainWindow::on_actionNew_triggered() {
    NewDialog new_dialog(this);
    connect(&new_dialog, &NewDialog::onFinish, this, &MainWindow::handleNewDialogData);
    new_dialog.exec();
}

void MainWindow::handleNewDialogData(const SheetMeta& data) {
    auto model = new SheetModel(ui->tableView);
    model->SetMeta(data);
    ui->tableView->setModel(model);
    updateFileActionsVisibility();
}

void MainWindow::on_actionImport_triggered() {
    ImportDialog import_dialog(this);
    connect(&import_dialog, &ImportDialog::onFinish, this, &MainWindow::handleImportDialogData);
    import_dialog.exec();
}

void MainWindow::handleImportDialogData(SheetModel* sheet) {
    ui->tableView->setModel(sheet);
    updateFileActionsVisibility();
}

void MainWindow::on_actionExport_triggered() {
    ExportDialog export_dialog(this);
    auto table_model = reinterpret_cast<SheetModel*>(ui->tableView->model());
    export_dialog.SetName(table_model->GetName());
    export_dialog.SetSheet(table_model->GetSheet());
    export_dialog.exec();
}

void MainWindow::updateFileActionsVisibility() {
    bool no_sheet = ui->tableView->model() == nullptr;
    ui->actionExport->setDisabled(no_sheet);
    ui->menuEdit->setDisabled(no_sheet);
}

void MainWindow::updateEditActionsVisibility() {
    bool sheet_items_selected = false;
    bool can_undo_redo = false;
    if (ui->tableView->model() != nullptr) {
        sheet_items_selected = ui->tableView->selectionModel()->hasSelection();
    }
    //add undo/redo logic

    ui->actionClear->setEnabled(sheet_items_selected);
    ui->actionCopy->setEnabled(sheet_items_selected);
    ui->actionCut->setEnabled(sheet_items_selected);
    ui->actionPaste->setEnabled(sheet_items_selected);

    ui->actionUndo->setEnabled(can_undo_redo);
    ui->actionRedo->setEnabled(can_undo_redo);
}

void MainWindow::showContextMenu(const QPoint &pos) {
    updateEditActionsVisibility();
    sheetContextMenu->popup(ui->tableView->mapToGlobal(pos));
}

void MainWindow::on_actionClear_triggered()
{
    ui->tableView->DeleteItems();
}

void MainWindow::on_actionCopy_triggered()
{
    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard) {
        QByteArray data = ui->tableView->CopySelected();
        if (data.isEmpty()) {
            return;
        }
        clipboard->setText(data);
    }
}

