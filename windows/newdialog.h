#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include "qt_common.h"

#include <QDialog>

namespace Ui {
class NewDialog;
}

class NewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewDialog(QWidget *parent = nullptr);
    ~NewDialog();

private slots:
    void on_name_ledit_textChanged(const QString &arg1);
    void on_buttonBox_accepted();

signals:
    void onFinish(const SheetMeta& data);

private:
    Ui::NewDialog *ui;
};

#endif // NEWDIALOG_H
