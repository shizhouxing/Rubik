#include "custominputdialog.h"
#include "ui_custominputdialog.h"

CustomInputDialog::CustomInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomInputDialog)
{
    ui->setupUi(this);
    ui->textEdit->setText("");
    setWindowTitle("Input");
}

CustomInputDialog::~CustomInputDialog()
{
    delete ui;
}

void CustomInputDialog::output(string res)
{
    ui->textEdit->setText(res.c_str());
    setWindowTitle("Solution (Homework mode)");
    show();    
}

void CustomInputDialog::on_pushButton_Cancel_clicked()
{
    buffer = "";
    close();
}

void CustomInputDialog::on_pushButton_OK_clicked()
{
    buffer = ui->textEdit->toPlainText();
    close();
}
