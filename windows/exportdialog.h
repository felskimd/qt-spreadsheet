#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include "../spreadsheet/sheet.h"

#include <QDialog>

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(QWidget *parent = nullptr);
    ~ExportDialog();

    void SetName(const QString& name);
    void SetSheet(const Sheet& sheet);

private slots:
    void updateExportAvailable();
    void on_folder_btn_clicked();
    void on_name_ledit_textChanged(const QString &arg1);
    void on_type_cbox_currentIndexChanged(int index);

    void on_buttonBox_accepted();

private:
    Ui::ExportDialog *ui;
    QString folder_path_;
    const Sheet* sheet_ = nullptr;
};

#endif // EXPORTDIALOG_H
