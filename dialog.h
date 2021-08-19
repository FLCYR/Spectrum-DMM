#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    void setPowerLabel();
    void setWaveLabel();
    void setPowerDefaultValue();
     void setWaveDefaultValue();
    int type;
signals:
    void rangeChange(double xMin,double xMax,double yMin,double yMax);

private slots:
    void on_btn_ok_clicked();

    void on_btn_canle_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
