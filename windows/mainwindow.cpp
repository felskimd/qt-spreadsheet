#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdialog.h"
#include "importdialog.h"
#include "exportdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //createActions();
    //createMenus();
    //createUndoView();

    updateActionsVisibility();
    //setup tableView
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, &QTableView::customContextMenuRequested, this, &MainWindow::showContextMenu);
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

void MainWindow::showContextMenu(const QPoint &pos) {
    //ui->menuEdit->actions();
    auto* menu = new QMenu(this);
    menu->addActions(ui->menuEdit->actions());

    //add logic
    // auto* copyAction = new QAction("Copy");
    // connect(copyAction, SIGNAL(triggered()), this, SLOT());
    // menu->addAction(copyAction);

    // auto* pasteAction = new QAction("Paste");
    // connect(pasteAction, SIGNAL(triggered()), this, SLOT());
    // menu->addAction(pasteAction);

    // QMenu* submenu = menu->addMenu("Move");
    // QMenu* submenuSelected = submenu->addMenu("Selected");
    // QMenu* submenuSelectedAndAfter = submenu->addMenu("Selected and all after");

    // menu->addSeparator();
    // auto* clearAction = new QAction("Clear");
    // connect(clearAction, &QAction::triggered, this, &MainWindow::on_actionClear_triggered);
    // menu->addAction(clearAction);

    menu->popup(ui->tableView->mapToGlobal(pos));
}

void MainWindow::on_actionClear_triggered()
{
    ui->tableView->DeleteItems();
}

