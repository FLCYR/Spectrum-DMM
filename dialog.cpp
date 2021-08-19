#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}
//波长显示
void Dialog::setWaveLabel()
{

    ui->label_yMax->setText("Y轴-最大值");
    ui->label_yMin->setText("Y轴-最小值");
}
//功率显示
void Dialog::setPowerLabel()
{

    ui->label_yMax->setText("功率-最大值");
    ui->label_yMin->setText("功率-最小值");
}
//确定按钮
void Dialog::on_btn_ok_clicked()
{
   double xMin=ui->doubleSpinBox_xMin->value();
   double xMax=ui->doubleSpinBox_xMax->value();

   double yMin=ui->doubleSpinBox_yMin->value();
   double yMax=ui->doubleSpinBox_yMax->value();
   emit rangeChange(xMin,xMax,yMin,yMax);
   this->hide();
}
//取消按钮
void Dialog::on_btn_canle_clicked()
{
    this->hide();
}

//设置功率默认值
void Dialog::setPowerDefaultValue()
{

    ui->doubleSpinBox_xMax->setValue(300);
    ui->doubleSpinBox_xMin->setValue(0);

    ui->doubleSpinBox_yMin->setValue(-10);
    ui->doubleSpinBox_yMax->setValue(10);
}
//设置波长默认值
void Dialog::setWaveDefaultValue()
{

    ui->doubleSpinBox_xMin->setValue(100);
    ui->doubleSpinBox_xMax->setValue(1500);

    ui->doubleSpinBox_yMin->setValue(0);
    ui->doubleSpinBox_yMax->setValue(65000);
}
