#include "newdialog.h"
#include "ui_newdialog.h"

NewDialog::NewDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewDialog)
{
    ui->setupUi(this);
}

NewDialog::~NewDialog()
{
    delete ui;
}

void NewDialog::on_name_ledit_textChanged(const QString &arg1)
{
    if (ui->name_ledit->text().size() == 0) {
        ui->buttonBox->hide();
    }
    else {
        ui->buttonBox->show();
    }
}

void NewDialog::on_buttonBox_accepted()
{
    SheetMeta result;
    result.name = ui->name_ledit->text();
    emit onFinish(result);
    accept();
}
