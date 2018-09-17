#ifndef CUSTOMINPUTDIALOG_H
#define CUSTOMINPUTDIALOG_H

#include <QDialog>
#include <string>

using namespace std;

namespace Ui {
class CustomInputDialog;
}

class CustomInputDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CustomInputDialog(QWidget *parent = 0);
    ~CustomInputDialog();
    
    void output(string res);
    
    QString buffer;
    
private slots:
    void on_pushButton_Cancel_clicked();
    
    void on_pushButton_OK_clicked();
    
private:
    Ui::CustomInputDialog *ui;
};

#endif // CUSTOMINPUTDIALOG_H
