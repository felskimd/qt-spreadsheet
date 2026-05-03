#include <QFileDialog>

#include "exportdialog.h"
#include "../features/import_export.h"
#include "ui_exportdialog.h"

ExportDialog::ExportDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ExportDialog)
{
    ui->setupUi(this);
    for (const auto& [type, func] : features::Exporter::Exporters) {
        ui->type_cbox->addItem(type);
    }
    updateExportAvailable();
}

void ExportDialog::updateExportAvailable() {
    ui->result_val->setText(folder_path_ + '/' + ui->name_ledit->text() + '.' + ui->type_cbox->currentText());
    if (folder_path_.isEmpty() || ui->name_ledit->text().isEmpty()) {
        ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setDisabled(true);
        ui->status_val->setText("Folder or name is empty");
    }
    else {
        ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(true);
        if (QFile::exists(ui->result_val->text())) {
            ui->status_val->setText("File exists! It will be overrided!");
        }
        else {
            ui->status_val->setText("Ok");
        }
    }
}

ExportDialog::~ExportDialog() {
    delete ui;
}

void ExportDialog::on_folder_btn_clicked() {
    folder_path_ = QFileDialog::getExistingDirectory(this);
    updateExportAvailable();
}


void ExportDialog::on_name_ledit_textChanged(const QString &arg1) {
    updateExportAvailable();
}


void ExportDialog::on_type_cbox_currentIndexChanged(int index) {
    updateExportAvailable();
}

void ExportDialog::SetName(const QString& name) {
    ui->name_ledit->setText(name);
}

void ExportDialog::SetSheet(const Sheet& sheet) {
    sheet_ = &sheet;
}

void ExportDialog::on_buttonBox_accepted() {
    features::Exporter::Export(QDir{folder_path_}, ui->name_ledit->text(), *sheet_, ui->type_cbox->currentText());
    accept();
}

