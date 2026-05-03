#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>
#include "sheet_model.h"

namespace Ui {
class ImportDialog;
}

class ImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDialog(QWidget *parent = nullptr);
    ~ImportDialog();

private slots:
    void on_buttonBox_accepted();
    void on_impor_btn_clicked();

signals:
    void onFinish(SheetModel* data);

private:
    Ui::ImportDialog *ui;
    QString file_path;
};

#endif // IMPORTDIALOG_H
