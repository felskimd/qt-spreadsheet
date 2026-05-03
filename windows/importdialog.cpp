#include <QFileDialog>

#include "importdialog.h"
#include "ui_importdialog.h"
#include "../features/import_export.h"

ImportDialog::ImportDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ImportDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setDisabled(true);
}

ImportDialog::~ImportDialog() {
    delete ui;
}

void ImportDialog::on_buttonBox_accepted() {
    // to qtconcurrent
    if (file_path.isEmpty()) {
        ui->status_lbl->setText("No file selected!");
        return;
    }
    else if (!features::Importer::CanImport(file_path)) {
        ui->status_lbl->setText("Can't import file!");
        return;
    }
    auto* result = new SheetModel;
    features::Importer::Import(file_path, [&result](int row, int col, QString& val) { result->setData(result->index(row, col), val); }); // to qtconcurrent and progressbar
    SheetMeta meta;
    QFileInfo file_info{file_path};
    meta.name = file_info.baseName();
    result->SetMeta(meta);
    emit onFinish(result);
    accept();
}

void ImportDialog::on_impor_btn_clicked() {
    auto folder = QFileDialog::getOpenFileName(this);
    QFileInfo file_info{folder};
    ui->selected_lbl->setText(file_info.fileName());
    file_path = folder;
    if (features::Importer::CanImport(folder)) {
        ui->status_lbl->setText("Ok");
        ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setDisabled(false);
    }
    else {
        ui->status_lbl->setText("Can't import");
        ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setDisabled(true);
    }
}
