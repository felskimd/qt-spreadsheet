#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdialog.h"
#include "importdialog.h"
#include "exportdialog.h"
#include "sheet_model.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updateActionsVisibility();
}

MainWindow::~MainWindow() {
    delete ui;
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
    updateActionsVisibility();
}

void MainWindow::on_actionImport_triggered() {
    ImportDialog import_dialog(this);
    connect(&import_dialog, &ImportDialog::onFinish, this, &MainWindow::handleImportDialogData);
    import_dialog.exec();
}

void MainWindow::handleImportDialogData(SheetModel* sheet) {
    ui->tableView->setModel(sheet);
    updateActionsVisibility();
}

void MainWindow::on_actionExport_triggered() {
    ExportDialog export_dialog(this);
    auto table_model = reinterpret_cast<SheetModel*>(ui->tableView->model());
    export_dialog.SetName(table_model->GetName());
    export_dialog.SetSheet(table_model->GetSheet());
    export_dialog.exec();
}

void MainWindow::updateActionsVisibility() {
    bool no_sheet = ui->tableView->model() == nullptr;
    ui->actionExport->setDisabled(no_sheet);
    ui->menuEdit->setDisabled(no_sheet);
}
