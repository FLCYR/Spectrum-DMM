#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTimer>
#include<QLabel>
#include<QVector>
#include<QFile>
#include<QCloseEvent>
#include<QScreen>
#include<QPixmap>
#include"light.h"
#include"dmm6500.h"
#include"dialog.h"
#include"customstyle.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
private:
    //状态栏
    QTimer * timeUpdate;
    QLabel * timeLabel;
    QLabel * spectrumStatusLabel;
    QLabel * dmmStatusLabel;

    //工具栏
    QAction*itemFile;
    QAction*itemConn;
    QAction*itemStart;
    QAction*itemStop;
    QAction*itemSaveData;
    QAction*itemSavePowerPng;
    QAction*itemNoise;
    QAction*itemSingle;
    QAction*itemSaveLightPng;

    //对话框
    Dialog rangeDialog;

    //电流表6500
    DMM6500 dmm; //电流表对象
    double range;//量程
    bool isDmmTrigger;//电流表触发
    QVector<double>xCurrentVector;//存放电流x轴数据
    QVector<double>yCurrentVector;//存放电流y轴数据
    double totalPoints;//总的电流采样点数



    //停止标志
    bool isStop;

    //采样点数，间隔
    double sInterval;
    int samplePoints;
    int preSamplePoints;//前一次的采样点数
    //光谱仪
    Light light;  //光谱仪对象
    QMap<int,QString>calibration; //校准 标定系数
    double ratio;//标定系数
    bool isLightTrigger;//光谱仪触发
    double *lightNoise;// 光噪声


    bool isReady;//设备是否准备好
    //QMenu ICon size
    CustomStyle *style;

public slots:
    void drawPlot(double * x,double * y,unsigned short count);//图表刷新
    void startSeriesMeansure(); //工具栏 开始按钮 开始连续测量槽函数
    void startSingleMeansure();//工具栏 单次测量按钮 槽函数
    void stopMeansure();//停止测试
    void getCurrent(int type,double current);//获取电流
    void connectDevic();
    void setRange(double xMin,double xMax,double yMin,double yMax);
public:
    void createToolBars();
    void setMenuBarInfo();
    void setStatusBarInfo();
    void setTime(); //设置状态栏时间

    void successConnLight(); //光谱仪连接成功状态
    void successConnDmm();//6500连接成功状态

    void setPlotInfo();
    void loadStyleQss();//加载样式文件
    void triggerSelect(); //触发源选择

    void savePower();//保存功率图
    void saveLight(); //保存光谱图
    void saveAllData();//保存全部数据


    void calcDmmData(const QVector<double>&x,const QVector<double>&y);//计算6500数据
    void collectNoise(); //采集噪声

    void setToolsActEnabled(bool flag);//设置工具栏action是否可用
    double calcFWHM(int maxIndex,double maxValue,const QVector<double>&x,const QVector<double>&y);// 计算半高全宽
    void openCaliFile(); //打开系数文件

    void exitProgramm();//退出程序

protected:
 void closeEvent(QCloseEvent * e);//窗口关闭事件
private slots:
    void on_comboBox_range_currentIndexChanged(int index);

    void on_radioBtb_contin_clicked();
    void on_radioBtn_pluse_clicked();
    void on_btn_powerRange_clicked();
    void on_btn_lightRange_clicked();
    void on_checkBox_origin_stateChanged(int arg1);
    void on_checkBox_noise_stateChanged(int arg1);
    void on_checkBox_orig_noise_stateChanged(int arg1);
    void on_action_file_triggered();
    void on_action_saveData_triggered();
    void on_actions_powerAuto_triggered();
    void on_action_lightAuto_triggered();
    void on_action_signle_triggered();
    void on_action_contin_triggered();
    void on_action_stop_triggered();
    void on_action_set_triggered();
    void on_action_help_triggered();
    void on_action_about_triggered();
    void on_spinBox_wave_valueChanged(int arg1);
    void on_action_savePowerPng_triggered();
    void on_action_saveLightPng_triggered();
};
#endif // MAINWINDOW_H




